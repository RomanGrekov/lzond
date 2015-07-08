/*
 * dac.h
 *
 *  Created on: May 29, 2015
 *      Author: romang
 */

#ifndef DAC_H_
#define DAC_H_

#include "stm32f10x.h"
#include "stm32f10x_dac.h"

#define DAC_PORT GPIOA
#define DAC_CLOCKING_REGISTER RCC_APB2Periph_GPIOA
#define DAC_MOSI GPIO_Pin_7
#define DAC_CS GPIO_Pin_6
#define DAC_CLK GPIO_Pin_5
#define DAC_CS_DELAY 10 //ms
#define DAC_V_REF 3.3

void init_dac(void);
void dac_enable(void);
void dac_disable(void);
void dac(uint8_t val);
void dac_volts(float val);

#endif /* DAC_H_ */
