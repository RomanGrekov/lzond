/*
 * dac.c
 *
 *  Created on: May 29, 2015
 *      Author: romang
 */

#include "dac.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

void init_dac(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(DAC_CLOCKING_REGISTER, ENABLE);

	/* Confugure SCK and MOSI pins as Alternate Function Push Pull */
	GPIO_InitStructure.GPIO_Pin = DAC_CLK | DAC_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(DAC_PORT, &GPIO_InitStructure);
	/* Confugure MISO pin as ususal output  */
	GPIO_InitStructure.GPIO_Pin = DAC_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DAC_PORT, &GPIO_InitStructure);
    GPIO_SetBits(DAC_PORT, DAC_CS);

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}

void dac_enable(void)
{
	SPI_Cmd(SPI1, ENABLE);
}

void dac_disable(void)
{
	SPI_Cmd(SPI1, DISABLE);
}

void dac(uint8_t val)
{
	uint16_t data=0b1111000100000000;
	data |= val;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    GPIO_ResetBits(DAC_PORT, DAC_CS);
    vTaskDelay(DAC_CS_DELAY);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    vTaskDelay(DAC_CS_DELAY);
    GPIO_SetBits(DAC_PORT, DAC_CS);
}

void dac_volts(float val)
{
	uint8_t r_val=0;
	if(((int)((val * 255)/DAC_V_REF)) > 255){
		r_val = 255;
	}
	else r_val = (int)((val * 255)/DAC_V_REF);
	dac(r_val);
}
