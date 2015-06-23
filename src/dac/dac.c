/*
 * dac.c
 *
 *  Created on: May 29, 2015
 *      Author: romang
 */

#include "dac.h"

void init_dac(void)
{
	DAC_InitTypeDef DAC_InitStructure;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
}

void dac_enable(void)
{
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void dac_disable(void)
{
	DAC_Cmd(DAC_Channel_1, DISABLE);
}

void dac(uint8_t val)
{
	DAC_SetChannel1Data(DAC_Align_8b_R, val);
}

void dac_volts(float val)
{
	//Should be something
}
