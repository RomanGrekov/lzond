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

static uint8_t get_cur_mix(float cur_adc_v, float v_r, float v_l);
static const uint8_t* get_mix_text(uint8_t mix_type);
static void calc_q(void);
static void create_t1(void);
static void start_t1(void);
static void stop_t1(void);
static void delete_t1(void);
static void timer1_callback(xTimerHandle xTimer);
static void task_dac_handler(void *pvParameters);

const enum {
	undefined_mix,
	lean_mix,
	rich_mix
};

const enum {
	ref_out,
	in_out,
	cat_off,
	inc,
	dec,
	mid,
	nop
};

static xTimerHandle timer1;
static uint8_t current_mix;
static float v_out, v_in, q;
static uint8_t cm, v_rm;
static uint32_t t_l, t_r;
static uint32_t timer1_val=0;

void taskDacCicle( void *pvParameters )
{
	uint8_t state;
	enum {
		state1_u,
		state1_l,
		state1_r,
		state2,
		state3,
		state3_tp,
		state4,
		state5
	};
	t_l = 500;
	t_r = 500;
	cm = ref_out;
	state = state1_u;
	v_in = 0;
	v_rm = 1;
	create_t1();
    xTaskCreate(task_dac_handler,(signed char*)"DAC handler",configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskDelay((int)my_conf.pause_start / portTICK_RATE_MS);

    while(1){
       switch (state){
       case state1_u:
           current_mix = get_cur_mix(v_in, my_conf.v_r, my_conf.v_l);
           if (current_mix == undefined_mix) state = state1_u;
           else{
               if (current_mix == lean_mix) state = state1_l;
               if (current_mix == rich_mix) state = state1_r;
           }
           break;
       case state1_l:
    	   start_t1();
    	   state = state2;
    	   break;
       case state1_r:
    	   state = state3;
    	   break;
       case state2:
    	   if (timer1_val > my_conf.cat_off){
    		   stop_t1();
    		   cm = cat_off;
    		   state = state1_u;
    	   }
    	   else{
               current_mix = get_cur_mix(v_in, my_conf.v_r, my_conf.v_l);
               if (current_mix == lean_mix) state = state2;
               if (current_mix == undefined_mix){
            	   stop_t1();
            	   t_l = timer1_val;
            	   cm = mid;
            	   calc_q();
            	   state = state1_u;
               }
               if (current_mix == rich_mix){
            	   stop_t1();
            	   t_l = timer1_val;
            	   v_rm = 1;
            	   calc_q();
            	   state = state3;
               }
    	   }
    	   break;
       case state3:
    	   start_t1();
    	   state = state3_tp;
    	   break;
       case state3_tp:
    	   if (timer1_val > my_conf.t_power) state = state4;
    	   else{
               current_mix = get_cur_mix(v_in, my_conf.v_r, my_conf.v_l);
               if (current_mix == rich_mix) state = state3_tp;
               if (current_mix == undefined_mix){
            	   stop_t1();
            	   t_r = timer1_val;
            	   v_rm = 1;
            	   calc_q();
            	   state = state1_u;
               }
               if (current_mix == lean_mix){
            	   stop_t1();
            	   t_r = timer1_val;
            	   v_rm = 1;
            	   calc_q();
            	   state = state1_l;
               }
    	   }
    	   break;
       case state4:
    	   stop_t1();
    	   t_r = timer1_val;
    	   v_rm = 2;
    	   cm = inc;
    	   state = state5;
    	   break;
       case state5:
           current_mix = get_cur_mix(v_in, my_conf.v_r, my_conf.v_l);
           if (current_mix == lean_mix){
        	   cm = dec;
        	   v_rm = 1;
        	   state = state1_l;
           }
           if (current_mix == undefined_mix){
        	   cm = mid;
        	   state = state5;
           }
           if (current_mix == rich_mix){
        	   cm = inc;
        	   state = state5;
           }
           break;
       }
    }
}

static void task_dac_handler(void *pvParameters)
{
	float v_max;
	while(1){
        v_in = get_adc_volts();

        if (v_rm == 1) v_max = my_conf.v_rm1;
        if (v_rm == 2) v_max = my_conf.v_rm2;

        switch (cm){
        case ref_out:
                v_out = my_conf.v_def;
                dac_volts(v_out);
                break;
        case in_out:
                if (v_in <= v_max) v_out = v_in;
                else v_out = v_max;
                dac_volts(v_out);
                break;
        case inc:
                if (v_out <= (v_max - my_conf.v_out_inc_step)) v_out += my_conf.v_out_inc_step;
                dac_volts(v_out);
                vTaskDelay(my_conf.pause_inc / portTICK_RATE_MS);
                break;
        case dec:
                if (v_out >= (my_conf.v_min + my_conf.v_out_dec_step)) v_out -= my_conf.v_out_dec_step;
                dac_volts(v_out);
                vTaskDelay(my_conf.pause_dec / portTICK_RATE_MS);
                break;
        case mid:
                if (v_out > my_conf.v_def) v_out -= my_conf.v_out_mid_step;
                if (v_out <= my_conf.v_def) v_out += my_conf.v_out_mid_step;
                vTaskDelay(my_conf.pause_mid / portTICK_RATE_MS);
                break;
        case cat_off:
        		v_out = my_conf.v_min;
        		dac_volts(v_out);
                break;
        case nop:
                break;
        }
	}
}

static void calc_q(void)
{
	float q_ref;
	if ((check_p1() != 0 && check_p2() != 0) || (check_p1() == 0 && check_p2() == 0)) q_ref = my_conf.q_ref1;
	if (check_p1() == 0 && check_p2() != 0) q_ref = my_conf.q_ref2;
	if (check_p1() != 0 && check_p2() == 0) q_ref = my_conf.q_ref3;

	q = ((float)t_r/((float)(t_r + t_l)/100.0))/100.0;

	if (q >= q_ref) cm = inc;
	else cm = dec;
}

void prvLCDshowparams(void *pvParameters)
{
	static uint8_t chars[16];
	static float v_out_old, v_in_old, q_old;
	static uint8_t current_mix_old, cm_old;
	static portBASE_TYPE xStatus;

	while(1){
		if (v_out_old != v_out || current_mix_old != current_mix || v_in_old != v_in || q_old != q || cm_old != cm){
			log_debug("v_in: %f | mix: %u | cm: %u | q: %f | v_out: %f\n", v_in, current_mix, cm, q, v_out);
			cln_scr();
			xStatus = to_video_mem(0, 0, "in:"); float_to_string_(v_in, chars); chars[4]=0; to_video_mem(3, 0, chars);
			xStatus = to_video_mem(8, 0, "out:"); float_to_string_(v_out, chars); chars[4]=0; to_video_mem(12, 0, chars);
			xStatus = to_video_mem(0, 1, "mix:"); to_video_mem(5, 1, get_mix_text(current_mix));
			if (xStatus == pdFAIL) log_error("Can't put data to video mem\n");
			v_out_old = v_out;
			current_mix_old = current_mix;
			v_in_old = v_in;
			q_old = q;
			cm_old = cm;

			vTaskDelay(200 / portTICK_RATE_MS);
		}
	}
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

static const uint8_t* get_mix_text(uint8_t mix_type){
    switch(mix_type){
        case undefined_mix: return "undefined";
        case lean_mix: return "lean";
        case rich_mix: return "rich";
        default: return "undefined at all";
    };
	return "none";
}

static void timer1_callback(xTimerHandle xTimer){
	if (timer1 < 4294967295) timer1_val += TIMER_STEP;
	else log_error("Timer 1 is overloaded!\n");
}

static void create_t1(void)
{
	timer1_val = 0;
	timer1 = xTimerCreate("timer1", TIMER_STEP / portTICK_RATE_MS, pdTRUE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) log_error("Can't create software timer 1\n");
}

static void start_t1(void)
{
	timer1_val = 0;
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
