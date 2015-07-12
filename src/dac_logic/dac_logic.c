/*
  * dac_logic.c
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#include "dac_logic.h"
#include "../dac/dac.h"
#include "../configurator/configurator.h"

uint8_t timer1_callback_flag;

void taskDacCicle( void *pvParameters )
{
    read_def_params(&my_conf);
	dac_volts(my_conf.v_def);
	vTaskDelay(my_conf.start_pause);

    while(1)
    {

    }
}

struct HalfPeriod get_half_period(uint32_t timeout, uint32_t need_period){
	uint8_t retcode = 1;
	xTimerHandle timer1;
	portBASE_TYPE xStatus;
	float cur_adc_v;

	timer1 = xTimerCreate("timer1", timeout / portTICK_RATE_MS, pdFALSE,
			(void*) 1, timer1_callback);
	if (timer1 == NULL) ulog("Can't create software timer 1", ERROR_LEVEL);
	timer1_callback_flag = 0;
	xStatus = xTimerStart(timer1, 10);
	if (xStatus ==pdFAIL) ulog("Can't start software timer 1", ERROR_LEVEL);

	while (timer1_callback_flag == 0){
		cur_adc_v = get_adc_volts();
	}


}

void timer1_callback(xTimerHandle xTimer){
	timer1_callback_flag = 1;
}
