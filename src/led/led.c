/*
 * led.c
Led blinker file
 *
 *  Created on: May 20, 2015
 *      Author: romang
 */
#include "led.h"

void init_led(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void blink_tsk( void *pvParameters )
{
    while(1){
    	GPIO_SetBits(GPIOB,GPIO_Pin_0);
        vTaskDelay(1000);
    	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
        vTaskDelay(1000);
    }
 }
