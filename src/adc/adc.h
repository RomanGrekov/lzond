/*
 * adc.h
 *
 *  Created on: May 29, 2015
 *      Author: romang
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f10x.h"
#include "stm32f10x_adc.h"

#define V_OP 3.3

void adc_init(void);
uint16_t get_adc_value(void);
float get_adc_volts(void);

#endif /* ADC_H_ */
