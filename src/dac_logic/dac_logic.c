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
#include "task.h"

uint8_t timer1_callback_flag;

void taskDacCicle( void *pvParameters )
{
    read_def_params(&my_conf);
	ulog_raw("Set def level: ", INFO_LEVEL); ulog_float(my_conf.v_def, INFO_LEVEL);
	dac_volts(my_conf.v_def);
	ulog_raw("Start pause: ", INFO_LEVEL); ulog_float(my_conf.start_pause, INFO_LEVEL);
	vTaskDelay(my_conf.start_pause);

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

	hp.retcode = 1;
	timer1_callback_flag = 0;
	//Create software timer
	timer1 = xTimerCreate("timer1", timeout / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) ulog("Can't create software timer 1", ERROR_LEVEL);
	xStatus = xTimerStart(timer1, 10);
	xTimeBefore = xTaskGetTickCount();
	if (xStatus == pdFAIL) ulog("Can't start software timer 1", ERROR_LEVEL);

	while (timer1_callback_flag == 0 && cur_mix == undefined_mix){
		cur_adc_v = get_adc_volts();
		cur_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
	}
	if (timer1_callback_flag == 0) xTimerStop(timer1, 10);
    if (cur_mix != undefined_mix){
    	if (need_period == 1){
    		ulog("---period is needed---", INFO_LEVEL);
            old_mix = cur_mix;

            timer1_callback_flag = 0;
            cur_mix = undefined_mix;
            xStatus = xTimerReset(timer1, 10);
            xTimeBefore = xTaskGetTickCount();
            if (xStatus == pdFAIL) ulog("Can't reset software timer 1", ERROR_LEVEL);
            while (timer1_callback_flag == 0 && (cur_mix == old_mix || cur_mix == undefined_mix)){
                cur_adc_v = get_adc_volts();
                cur_mix = get_cur_mix(cur_adc_v, my_conf.v_r, my_conf.v_l);
            }
            if (timer1_callback_flag == 0) xTimerStop(timer1, 10);
            if (cur_mix != old_mix && cur_mix != undefined_mix){
                hp.period = xTaskGetTickCount() - xTimeBefore;
                hp.retcode = 0;
            }
        }
    	else{
    		hp.period = xTaskGetTickCount() - xTimeBefore;
    		hp.retcode = 0;
    	}

    }

    hp.cur_adc = cur_adc_v;
	return hp;


}

void timer1_callback(xTimerHandle xTimer){
	timer1_callback_flag = 1;
}

uint8_t get_cur_mix(float cur_adc_v, float v_r, float v_l){
	if (cur_adc_v >= v_r){
		ulog("Current mix is rich", INFO_LEVEL);
		return rich_mix;
	}
	else{
		if (cur_adc_v >= v_l){
			ulog("Current mix is lean", INFO_LEVEL);
			return lean_mix;
		}
	}

	ulog("Current mix is undefined", INFO_LEVEL);
	return undefined_mix;

}
