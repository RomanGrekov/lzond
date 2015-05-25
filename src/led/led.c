/*
 * led.c
Led blinker file
 *
 *  Created on: May 20, 2015
 *      Author: romang
 */
#include "led.h"

void blink_tsk( void *pvParameters )
{
    while(1){
    	GPIO_SetBits(GPIOB,GPIO_Pin_0);
        vTaskDelay(1000);
    	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
        vTaskDelay(1000);
    }
 }


