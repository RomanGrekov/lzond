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
	xTimerHandle timer1;
	portBASE_TYPE xStatus;

    read_def_params(&my_conf);
    hp.retcode = 1;
    while (hp.retcode != 0)
    {
       log_info("---Stage 1---");
       log_info("Set def level: %f\n", my_conf.v_def);
       dac_volts(my_conf.v_def);
       log_info("Start pause: %f\n", my_conf.start_pause);
       vTaskDelay(my_conf.start_pause);
       hp = get_half_period(my_conf.start_timeout, 0);
    }

    log_info("---Stage 2---");
	timer1_callback_flag = 0;
	timer1 = xTimerCreate("timer1", (hp.period * my_conf.k1) / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) log_error("Can't create software timer 1\n");
    while (timer1_callback_flag == 0) dac_volts(get_adc_volts());
    //Delete timer
    xStatus = xTimerDelete(timer1, 10);
    if (xStatus == pdFAIL) log_error("Can't delete software timer 1\n");

    log_info("---Stage 3---");
    hp.retcode = 1;
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

	log_info("---Get half period---\n");
	//Set default retcode to 1
	hp.retcode = 1;
	//Reset callback flag
	timer1_callback_flag = 0;
	//Create software timer
	timer1 = xTimerCreate("timer1", timeout / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) log_error("Can't create software timer 1\n");
	xStatus = xTimerStart(timer1, 10);
	if (xStatus == pdFAIL) log_error("Can't start software timer 1\n");

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
	log_info("Cur mix: %20s\n", get_mix_text(cur_mix));
	log_info("Cur adc: %20f\n", cur_adc_v);
	//If checking for mix type is successful
    if (cur_mix != undefined_mix){
    	//Check if we need to check for mix change
    	if (need_period == 1){
    		log_info("---period is needed---\n");
    		//Save old mix type
            old_mix = cur_mix;
            //Reset callback flag
            timer1_callback_flag = 0;
            //Reset timer
            xStatus = xTimerReset(timer1, 10);
            if (xStatus == pdFAIL) log_error("Can't reset software timer 1\n");
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
            log_info("Cur mix: %20s\n", get_mix_text(cur_mix));
            log_info("Cur adc: %20f\n", cur_adc_v);
            //If checking for mix type is successful
            if (cur_mix != old_mix){
            	//Set retcode
                hp.retcode = 0;
                log_info("Period: %20d\n", hp.period);
            }
        }
    	//If checking for mix type is successful and not needed second check
    	else{
            //Set retcode
    		hp.retcode = 0;
    		log_info("Period: %20d\n", hp.period);
    	}
    }
    //Delete timer
    xStatus = xTimerDelete(timer1, 10);
    if (xStatus == pdFAIL) log_error("Can't delete software timer 1\n");
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
