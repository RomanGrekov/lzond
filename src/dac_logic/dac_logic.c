/*
  * dac_logic.c
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#include "dac_logic.h"
#include "../dac/dac.h"
#include "../configurator/configurator.h"
#include "../usart/usart.h"
#include "../adc/adc.h"
#include "task.h"

uint8_t timer1_callback_flag;

void taskDacCicle( void *pvParameters )
{
	struct HalfPeriod hp;

    read_def_params(&my_conf);
	ulog_raw("Set def level: ", INFO_LEVEL); ulog_float(my_conf.v_def, INFO_LEVEL);
	dac_volts(my_conf.v_def);
	ulog_raw("Start pause: ", INFO_LEVEL); ulog_float(my_conf.start_pause, INFO_LEVEL);
	vTaskDelay(my_conf.start_pause);
    hp = get_half_period(my_conf.start_timeout, 0);
    if (hp.retcode == 0)ulog("retcode is 0", INFO_LEVEL);

    while(1)
    {

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

	ulog("---Get half period---", INFO_LEVEL);
	//Set default retcode to 1
	hp.retcode = 1;
	//Reset callback flag
	timer1_callback_flag = 0;
	//Create software timer
	timer1 = xTimerCreate("timer1", timeout / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) ulog("Can't create software timer 1", ERROR_LEVEL);
	xStatus = xTimerStart(timer1, 10);
	if (xStatus == pdFAIL) ulog("Can't start software timer 1", ERROR_LEVEL);

	//Save time stamp before waiting for mix type
	xTimeBefore = xTaskGetTickCount() * portTICK_RATE_MS;
	//Start checking for mix type
	while (timer1_callback_flag == 0 && cur_mix == undefined_mix){
		cur_adc_v = get_adc_volts();
		cur_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
	}
	//Define time period for checking mix type
	hp.period = xTaskGetTickCount() * portTICK_RATE_MS - xTimeBefore;
	//Stop timer
	if (xTimerIsTimerActive(timer1) == pdTRUE) xTimerStop(timer1, 10);
	ulog_raw("Cur mix: ", INFO_LEVEL); ulog(get_mix_text(cur_mix), INFO_LEVEL);
	ulog_raw("Cur adc: ", INFO_LEVEL); ulog_float(cur_adc_v, INFO_LEVEL);
	//If checking for mix type is successful
    if (cur_mix != undefined_mix){
    	//Check if we need to check for mix change
    	if (need_period == 1){
    		ulog("---period is needed---", INFO_LEVEL);
    		//Save old mix type
            old_mix = cur_mix;
            //Reset callback flag
            timer1_callback_flag = 0;
            //Reset timer
            xStatus = xTimerReset(timer1, 10);
            if (xStatus == pdFAIL) ulog("Can't reset software timer 1", ERROR_LEVEL);
            //Save time stamp before waiting for mix type
            xTimeBefore = xTaskGetTickCount() * portTICK_RATE_MS;
            while (timer1_callback_flag == 0 && cur_mix == old_mix){
                cur_adc_v = get_adc_volts();
                cur_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
            }
            //Define time period for checking mix type
            hp.period = xTaskGetTickCount() * portTICK_RATE_MS - xTimeBefore;
            //Stop timer
            if (xTimerIsTimerActive(timer1) == pdTRUE) xTimerStop(timer1, 10);
            ulog_raw("Cur mix: ", INFO_LEVEL); ulog(get_mix_text(cur_mix), INFO_LEVEL);
            ulog_raw("Cur adc: ", INFO_LEVEL); ulog_float(cur_adc_v, INFO_LEVEL);
            //If checking for mix type is successful
            if (cur_mix != old_mix){
            	//Set retcode
                hp.retcode = 0;
                ulog_raw("Period: ", INFO_LEVEL); ulog_int(hp.period, INFO_LEVEL);
            }
        }
    	//If checking for mix type is successful and not needed second check
    	else{
            //Set retcode
    		hp.retcode = 0;
    		ulog_raw("Period: ", INFO_LEVEL); ulog_int(hp.period, INFO_LEVEL);
    	}
    }
    //Store current voltage
    hp.cur_adc = cur_adc_v;
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
