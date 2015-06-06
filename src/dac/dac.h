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

void init_dac(void);
void dac_enable(void);
void dac_disable(void);
void dac(uint8_t val);

#endif /* DAC_H_ */
