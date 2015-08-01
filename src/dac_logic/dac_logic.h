/*
 * dac_logic.h
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#ifndef DAC_LOGIC_H_
#define DAC_LOGIC_H_

#include "stm32f10x.h"

#define TIMER_STEP 10
void taskDacCicle( void *pvParameters );
void prvLCDshowparams(void *pvParameters);


#endif /* DAC_LOGIC_H_ */
