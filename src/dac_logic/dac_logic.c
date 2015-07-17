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

enum {
	retcode_ok,
	retcode_timeout,
	retcode_mix_undefined
};

enum {
	undefined_mix,
	lean_mix,
	rich_mix
};

uint8_t timer1_callback_flag;
xTimerHandle timer1;
uint8_t current_mix;
void prvDACRepeater( void *pvParameters );
const uint8_t* get_mix_text_short(uint8_t mix_type);

void taskDacCicle( void *pvParameters )
{
	struct HalfPeriod hp;
	xTimerHandle timer2;
	uint32_t t_lean, t_rich, t_period;
	TaskHandle_t xHandle = NULL;
	float t_rel, mix_connection;
    enum {
            not_defined,
            defined
    };
	uint8_t mix_defined = not_defined;

    my_conf.v_out = my_conf.v_def;
    log_info("Set DAC to default V: %f\n", my_conf.v_out);
    dac_volts(my_conf.v_out);
    vTaskDelay((int)my_conf.start_pause / portTICK_RATE_MS);

    hp.retcode = retcode_timeout;
    log_notice("---Waiting for mix become lean or rich---\n");
    while (hp.retcode != retcode_ok) hp = get_half_period(my_conf.start_timeout, 0);
    log_notice("Mix defined: %s\n", get_mix_text(hp.cur_mix));

    while (1)
    {
    	if (mix_defined == not_defined)
    	{
    		log_notice("---Start repeat ADC input to DAC---\n");
    		if( xHandle == NULL )
    		{
    			xTaskCreate(prvDACRepeater,(signed char*)"DAC repeater",configMINIMAL_STACK_SIZE,
    			            NULL, tskIDLE_PRIORITY + 1, &xHandle);
    			vTaskDelay((int)(hp.period * my_conf.k1));
    		}
    		//else xTaskResume(xHandle);
    	}

    	log_notice("---Get duty circle---\n");
    	hp.retcode = retcode_timeout;
    	while (hp.retcode != retcode_ok)
    	{
    	   hp = get_half_period(my_conf.period_timeout, 1);
    	   if (hp.retcode == retcode_mix_undefined) break;
    	   if (hp.retcode == retcode_timeout) continue;
    	   if (hp.cur_mix == lean_mix) t_lean = hp.period;
    	   if (hp.cur_mix == rich_mix) t_rich = hp.period;
    	   log_info("Mix defined: %s\n", get_mix_text(hp.cur_mix));
    	   hp = get_half_period(my_conf.period_timeout, 1);
    	   if (hp.retcode == retcode_mix_undefined) break;
    	   if (hp.retcode == retcode_timeout) continue;
    	   if (hp.cur_mix == lean_mix) t_lean = hp.period;
    	   if (hp.cur_mix == rich_mix) t_rich = hp.period;
    	   log_info("Mix defined: %s\n", get_mix_text(hp.cur_mix));
    	}
    	if (hp.retcode == retcode_mix_undefined)
    	{
    		continue; //means start from the stage 1
    		log_notice("---Get duty circle failed!---\n");
    	}
    	//if (hp.retcode == retcode_ok) break;
    	log_notice("---Get duty circle - OK---\n");
    	mix_defined = defined;
    	if (xHandle != NULL)
    	{
    		vTaskDelete(xHandle);
    		xHandle = NULL;
    	}

    	if (t_lean <= my_conf.cut_off){
    		log_notice("---Make correction---\n");
    		t_period = t_lean + t_rich;
    		t_rel = t_rich / t_period;
    		mix_connection = 0.7 * 2.0 * t_rel;
    		log_info("t_lean: %d\n", t_lean);
    		log_info("t_rich: %d\n", t_rich);
    		log_info("period: %d\n", t_period);
    		log_info("t_rel: %f\n", t_rel);
    		log_info("mix_connection: %f\n", mix_connection);
    		if (mix_connection > my_conf.v_outref)
    		{
    			if (my_conf.v_out < my_conf.v_outlim_inc) my_conf.v_out = my_conf.v_out + my_conf.v_out_inc_step;
    		}
    		if (mix_connection <= my_conf.v_outref)
    		{
    			if (my_conf.v_out > my_conf.v_outlim_dec) my_conf.v_out = my_conf.v_out - my_conf.v_out_dec_step;
    		}
    	}
    	else
    	{
    		log_notice("---Cut off mode---\n");
    		my_conf.v_out = my_conf.v_outlim_dec;
    	}
    	log_notice("---DAC set v_out: %f---\n", my_conf.v_out);
    	//vTaskSuspend(xHandle);
    	dac_volts(my_conf.v_out);
    	vTaskDelay((int)my_conf.pause_inc / portTICK_RATE_MS);
    }

    while(1);
}

void prvDACRepeater( void *pvParameters )
{
	while(1){
		my_conf.v_out = get_adc_volts();
		dac_volts(my_conf.v_out);
        vTaskDelay(10 / portTICK_RATE_MS);
	}
}

void prvLCDshowparams(void *pvParameters)
{
	uint8_t chars[16];
	float v_out_old;
	uint8_t current_mix_old;
	portBASE_TYPE xStatus;

	while(1){
		if (v_out_old != my_conf.v_out || current_mix_old != current_mix){
			cln_scr();
			xStatus = to_video_mem(0, 0, "out:"); float_to_string_(my_conf.v_out, chars); chars[4]=0; to_video_mem(4, 0, chars);
			xStatus = to_video_mem(9, 0, "mix: "); to_video_mem(13, 0, get_mix_text_short(current_mix));
			if (xStatus == pdFAIL) log_error("Can't put data to video mem\n");
			v_out_old = my_conf.v_out;
			current_mix_old = current_mix;
			vTaskDelay(500 / portTICK_RATE_MS);
		}
	}

}


struct HalfPeriod get_half_period(uint32_t timeout, uint8_t need_period){
	struct HalfPeriod hp;
	xTimerHandle timer1;
	portBASE_TYPE xStatus;
	float cur_adc_v;
	uint8_t cur_mix = undefined_mix;
	uint8_t old_mix;
	TickType_t xTimeBefore;

	log_debug("------Get half period------\n");
	//Set default retcode to 1
	hp.retcode = retcode_timeout;
	//Reset callback flag
	timer1_callback_flag = 0;
	//Create software timer
	create_t1(timeout);
	start_t1();
	//Save time stamp before waiting for mix type
	xTimeBefore = xTaskGetTickCount() * portTICK_RATE_MS;
	//Start checking for mix type
	while (timer1_callback_flag == 0 && cur_mix == undefined_mix){
		cur_adc_v = get_adc_volts();
		cur_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
	}
	current_mix = cur_mix;
	//Define time period for checking mix type
	hp.period = xTaskGetTickCount() * portTICK_RATE_MS - xTimeBefore;
	stop_t1();
	log_debug("        Cur mix: %20s\n", get_mix_text(cur_mix));
	log_debug("        Cur adc: %20f\n", cur_adc_v);
	//If checking for mix type is successful
    if (cur_mix != undefined_mix){
    	//Check if we need to check for mix change
    	if (need_period == 1){
    		log_debug("------period is needed------\n");
    		//Save old mix type
            old_mix = cur_mix;
            //Reset callback flag
            timer1_callback_flag = 0;
            //Reset timer
            reset_t1();
            //Save time stamp before waiting for mix type
            xTimeBefore = xTaskGetTickCount() * portTICK_RATE_MS;
            while (timer1_callback_flag == 0 && cur_mix == old_mix){
                cur_adc_v = get_adc_volts();
                cur_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
            }
            current_mix = cur_mix;
            //Define time period for checking mix type
            hp.period = xTaskGetTickCount() * portTICK_RATE_MS - xTimeBefore;
            stop_t1();
            log_debug("        Cur mix: %20s\n", get_mix_text(cur_mix));
            log_debug("        Cur adc: %20f\n", cur_adc_v);
            //If checking for mix type is successful
            if (cur_mix != old_mix){
            	if (cur_mix != undefined_mix)
            	{
                    //Set retcode
                    hp.retcode = retcode_ok;
                    log_debug("        Period: %20d\n", hp.period);
            	}
            	else hp.retcode = retcode_mix_undefined;
            }
        }
    	//If checking for mix type is successful and not needed second check
    	else{
            //Set retcode
    		hp.retcode = retcode_ok;
    		log_debug("        Period: %20d\n", hp.period);
    	}
    }
    delete_t1();
    //Store current voltage
    hp.cur_adc = cur_adc_v;
    hp.cur_mix = cur_mix;
	return hp;
}

void timer1_callback(xTimerHandle xTimer){
	timer1_callback_flag = 1;
}

uint8_t get_cur_mix(float cur_adc_v, float v_r, float v_l){
	if (cur_adc_v >= v_r){
		return rich_mix;
	}
	else{
		if (cur_adc_v >= v_l){
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

const uint8_t* get_mix_text_short(uint8_t mix_type){
	uint8_t resp[2];
	resp[0] = get_mix_text(mix_type)[0];
	resp[1]=0;
	return resp;
}

void create_t1(uint32_t ms)
{
	if (ms <= 0) ms = 1;
	timer1_callback_flag = 0;
	timer1 = xTimerCreate("timer1", ms / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) log_error("Can't create software timer 1\n");
}

void start_t1(void)
{
	timer1_callback_flag = 0;
	portBASE_TYPE xStatus;
	xStatus = xTimerStart(timer1, 10);
	if (xStatus == pdFAIL) log_error("Can't start software timer 1\n");
}

void stop_t1(void)
{
	portBASE_TYPE xStatus;
	if (xTimerIsTimerActive(timer1) == pdTRUE){
		xStatus = xTimerStop(timer1, 10);
		if (xStatus == pdFAIL) log_error("Can't stop software timer 1\n");
	}
}

void reset_t1(void)
{
	timer1_callback_flag = 0;
	portBASE_TYPE xStatus;
    xStatus = xTimerReset(timer1, 10);
    if (xStatus == pdFAIL) log_error("Can't reset software timer 1\n");
}

void delete_t1(void)
{
	portBASE_TYPE xStatus;
	if (xTimerIsTimerActive(timer1) == pdTRUE){
		xStatus = xTimerStop(timer1, 10);
		if (xStatus == pdFAIL) log_error("Can't stop software timer 1\n");
	}
    xStatus = xTimerDelete(timer1, 100);
    if (xStatus == pdFAIL) log_error("Can't delete software timer 1\n");
}
