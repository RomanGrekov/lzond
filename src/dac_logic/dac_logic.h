/*
 * dac_logic.h
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#ifndef DAC_LOGIC_H_
#define DAC_LOGIC_H_

void taskDacCicle( void *pvParameters );

struct HalfPeriod {
	uint8_t retcode;
	uint32_t period;
	float cur_adc;
};

#endif /* DAC_LOGIC_H_ */
