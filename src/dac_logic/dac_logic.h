/*
 * dac_logic.h
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#ifndef DAC_LOGIC_H_
#define DAC_LOGIC_H_

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "timers.h"

void taskDacCicle( void *pvParameters );
void timer1_callback(xTimerHandle xTimer);

struct HalfPeriod {
	uint8_t retcode;
	uint32_t period;
	float cur_adc;
};

enum {
	undefined_mix,
	lean_mix,
	rich_mix
};

struct HalfPeriod get_half_period(uint32_t timeout, uint8_t need_period);
uint8_t get_cur_mix(float cur_adc_v, float v_r, float v_l){

#endif /* DAC_LOGIC_H_ */