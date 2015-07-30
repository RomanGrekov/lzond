/*
  * dac_logic.c
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#include "dac_logic.h"
#include "../dac/dac.h"
#include "../configurator/configurator.h"
#include "../log/log.h"
#include "../adc/adc.h"
#include "../xprintf/xprintf.h"
#include "../hd44780/hd44780.h"
#include "task.h"
#include "FreeRTOS.h"
#include "timers.h"

static struct HalfPeriod get_half_period(uint32_t timeout);
void get_period(uint32_t timeout);
static uint8_t get_cur_mix(float cur_adc_v, float v_r, float v_l);
static const uint8_t* get_mix_text(uint8_t mix_type);
static void create_t1(uint32_t ms);
static void start_t1(void);
static void stop_t1(void);
static void delete_t1(void);
void timer1_callback(xTimerHandle xTimer);
void task_adc_repeater( void *pvParameters );
void task_mix_correction( void *pvParameters );

const enum {
	retcode_ok,
	retcode_timeout,
	retcode_mix_undefined
};

const enum {
	undefined_mix,
	lean_mix,
	rich_mix
};

static struct HalfPeriod {
	uint8_t retcode;
	uint32_t period;
};

static struct Period {
	uint32_t t_l;
	uint32_t t_r;
};

uint8_t timer1_callback_flag;
xTimerHandle timer1;
uint8_t current_mix;
static struct Period p;
float v_out, v_in;

void taskDacCicle( void *pvParameters )
{
	struct HalfPeriod hp;
	TaskHandle_t dac_repeater_h = NULL;
	TaskHandle_t mix_corrector_h = NULL;
    enum {
            not_defined,
            defined
    };
	uint8_t mix_defined = not_defined;
	current_mix = undefined_mix;

    v_out = my_conf.v_def;
    log_info("Set DAC to default V: %f\n", v_out);
    dac_volts(v_out);
    vTaskDelay((int)my_conf.start_pause / portTICK_RATE_MS);

    hp.retcode = retcode_timeout;
    log_notice("---Waiting for mix become lean or rich---\n");
    while (hp.retcode != retcode_ok) hp = get_half_period(my_conf.start_timeout);

    while (1)
    {
    	if (mix_defined == not_defined)
    	{
    		log_notice("---Start repeat ADC input to DAC---\n");
    		if( dac_repeater_h == NULL )
    		{
    			xTaskCreate(task_adc_repeater,(signed char*)"DAC repeater",configMINIMAL_STACK_SIZE,
    			            NULL, tskIDLE_PRIORITY + 1, &dac_repeater_h);
    			vTaskDelay((int)(hp.period * my_conf.k1));
    		}
    		//else xTaskResume(xHandle);
    	}

    	log_notice("---Get duty circle---\n");
    	get_period(my_conf.period_timeout);

    	mix_defined = defined;
    	if (dac_repeater_h != NULL)
    	{
    		log_notice("---Stop DAC repeating---\n");
    		vTaskDelete(dac_repeater_h);
    		dac_repeater_h = NULL;
    	}

    	if (mix_corrector_h == NULL){
    		log_notice("---Start mix correction task---\n");
    		xTaskCreate(task_mix_correction,(signed char*)"MIX corrector",configMINIMAL_STACK_SIZE,
    	            	NULL, tskIDLE_PRIORITY + 1, &mix_corrector_h);
    	}

    }
}


void task_mix_correction( void *pvParameters )
{
	uint32_t t_period;
	float t_rel, mix_connection;
	while(1){
    	if (p.t_l <= my_conf.cut_off){
    		log_notice("---Make correction---\n");
    		t_period = p.t_l + p.t_r;
    		t_rel = (float)p.t_r / (float)t_period;
    		mix_connection = 0.7 * 2.0 * t_rel;
    		log_info("t_lean: %d\n", p.t_l);
    		log_info("t_rich: %d\n", p.t_r);
    		log_info("period: %d\n", t_period);
    		log_info("t_rel: %f\n", t_rel);
    		log_info("mix_connection: %f\n", mix_connection);
    		if (mix_connection > my_conf.v_outref)
    		{
    			if (v_out < my_conf.v_outlim_inc) v_out = v_out + my_conf.v_out_inc_step;
    		}
    		if (mix_connection <= my_conf.v_outref)
    		{
    			if (v_out > my_conf.v_outlim_dec) v_out = v_out - my_conf.v_out_dec_step;
    		}
    	}
    	else
    	{
    		log_notice("---Cut off mode---\n");
    		v_out = my_conf.v_outlim_dec;
    	}
    	log_notice("---DAC set v_out: %f---\n", v_out);
    	dac_volts(v_out);
    	vTaskDelay((int)my_conf.pause_inc / portTICK_RATE_MS);
	}
}

void task_adc_repeater( void *pvParameters )
{
	while(1){
		v_out = get_adc_volts();
		v_in = v_out;
		dac_volts(v_out);
	}
}

void prvLCDshowparams(void *pvParameters)
{
	uint8_t chars[16];
	float v_out_old, v_in_old;
	uint8_t current_mix_old;
	portBASE_TYPE xStatus;

	while(1){
		if (v_out_old != v_out || current_mix_old != current_mix || v_in_old != v_in){
			cln_scr();
			xStatus = to_video_mem(0, 0, "in:"); float_to_string_(v_in, chars); chars[4]=0; to_video_mem(3, 0, chars);
			xStatus = to_video_mem(8, 0, "out:"); float_to_string_(v_out, chars); chars[4]=0; to_video_mem(12, 0, chars);
			xStatus = to_video_mem(0, 1, "mix:"); to_video_mem(5, 1, get_mix_text(current_mix));
			if (xStatus == pdFAIL) log_error("Can't put data to video mem\n");
			v_out_old = v_out;
			current_mix_old = current_mix;
			v_in_old = v_in;
			vTaskDelay(500 / portTICK_RATE_MS);
		}
	}

}

struct HalfPeriod get_half_period(uint32_t timeout){
	struct HalfPeriod hp;
	float cur_adc_v;
	TickType_t xTimeBefore, xTimeAfter;

	hp.retcode = retcode_timeout;

	cur_adc_v = get_adc_volts();
	v_in = cur_adc_v;
	current_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
	hp.period = 0;

	if (current_mix == undefined_mix){
       create_t1(timeout);
       start_t1();

       xTimeBefore = xTaskGetTickCount();

       while (timer1_callback_flag == 0 && current_mix == undefined_mix){
               cur_adc_v = get_adc_volts();
               v_in = cur_adc_v;
               current_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
       }
       xTimeAfter = xTaskGetTickCount();
       //Define time period for checking mix type
       hp.period = ((xTimeAfter - xTimeBefore) * portTICK_RATE_MS);
       stop_t1();
       delete_t1();
	}
	log_debug("        Cur mix: %20s\n", get_mix_text(current_mix));
	log_debug("        Cur adc: %20f\n", cur_adc_v);
	//If checking for mix type is successful
    if (current_mix != undefined_mix)	hp.retcode = retcode_ok;
    //Store current voltage
	return hp;
}

void get_period(uint32_t timeout){
	struct HalfPeriod hp;
	float cur_adc_v;
	uint8_t old_mix;
	TickType_t t1, t2, t3;

	while(1){
		old_mix = current_mix;
        hp.retcode = retcode_timeout;
        while (hp.retcode != retcode_ok){
                hp = get_half_period(my_conf.start_timeout);
                if (hp.retcode != retcode_ok){
                        log_error("Can't define mix type!");
                }
        }

        create_t1(timeout);
        start_t1();
        while (timer1_callback_flag == 0 && (current_mix == old_mix || current_mix == undefined_mix)){
            cur_adc_v = get_adc_volts();
            v_in = cur_adc_v;
            current_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
        }
        t1 = xTaskGetTickCount();
        stop_t1();
        delete_t1();
        if (timer1_callback_flag == 1) continue;

		old_mix = current_mix;
        create_t1(timeout);
        start_t1();
        while (timer1_callback_flag == 0 && (current_mix == old_mix || current_mix == undefined_mix)){
            cur_adc_v = get_adc_volts();
            v_in = cur_adc_v;
            current_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
        }
        t2 = xTaskGetTickCount();
        stop_t1();
        delete_t1();
        if (timer1_callback_flag == 1) continue;

        if (current_mix == lean_mix) p.t_l = (t2 - t1) * portTICK_RATE_MS;
        if (current_mix == rich_mix) p.t_r = (t2 - t1) * portTICK_RATE_MS;

		old_mix = current_mix;
        create_t1(timeout);
        start_t1();
        while (timer1_callback_flag == 0 && (current_mix == old_mix || current_mix == undefined_mix)){
            cur_adc_v = get_adc_volts();
            v_in = cur_adc_v;
            current_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
        }
        t3 = xTaskGetTickCount();
        stop_t1();
        delete_t1();
        if (timer1_callback_flag == 1) continue;

        if (current_mix == lean_mix) p.t_l = (t3 - t2) * portTICK_RATE_MS;
        if (current_mix == rich_mix) p.t_r = (t3 - t2) * portTICK_RATE_MS;

        break;
	}
}

void timer1_callback(xTimerHandle xTimer){
	timer1_callback_flag = 1;
}

static uint8_t get_cur_mix(float cur_adc_v, float v_r, float v_l){
	if (cur_adc_v >= v_r){
		return rich_mix;
	}
	else{
		if (cur_adc_v <= v_l){
			return lean_mix;
		}
	}
	return undefined_mix;
}

const uint8_t* get_mix_text(uint8_t mix_type){
    switch(mix_type){
        case undefined_mix: return "undefined";
        case lean_mix: return "lean";
        case rich_mix: return "rich";
        default: return "undefined at all";
    };
	return "none";
}

static void create_t1(uint32_t ms)
{
	if (ms <= 0) ms = 1;
	timer1_callback_flag = 0;
	timer1 = xTimerCreate("timer1", ms / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) log_error("Can't create software timer 1\n");
}

static void start_t1(void)
{
	timer1_callback_flag = 0;
	portBASE_TYPE xStatus;
	xStatus = xTimerStart(timer1, 10);
	if (xStatus == pdFAIL) log_error("Can't start software timer 1\n");
}

static void stop_t1(void)
{
	portBASE_TYPE xStatus;
	if (xTimerIsTimerActive(timer1) == pdTRUE){
		xStatus = xTimerStop(timer1, 10);
		if (xStatus == pdFAIL) log_error("Can't stop software timer 1\n");
	}
}

static void delete_t1(void)
{
	portBASE_TYPE xStatus;
	if (xTimerIsTimerActive(timer1) == pdTRUE){
		xStatus = xTimerStop(timer1, 10);
		if (xStatus == pdFAIL) log_error("Can't stop software timer 1\n");
	}
    xStatus = xTimerDelete(timer1, 100);
    if (xStatus == pdFAIL) log_error("Can't delete software timer 1\n");
}
