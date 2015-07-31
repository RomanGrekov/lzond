/*
 * buttons.c

 *
 *  Created on: Jun 6, 2015
 *      Author: romang
 */

#include "buttons.h"


void init_buttons(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t check_button(GPIO_TypeDef *port, uint16_t pin)
{
	return GPIO_ReadInputDataBit(port, pin);
}

uint8_t check_btn1(void)
{
	return check_button(BTN1_PORT, BTN1_PIN);
}

uint8_t check_p1(void)
{
	return check_button(BTN1_PORT, P1_PIN);
}

uint8_t check_p2(void)
{
	return check_button(BTN1_PORT, P2_PIN);
}

void btn1_react(void (*f_on)(void), void (*f_off)(void))
{
	static uint8_t btn_state = 0, btn_state_old = 0, is_on = 0;
	btn_state = check_btn1();
	if (btn_state != btn_state_old){
		btn_state_old = btn_state;
		if(btn_state == 0){
			if (is_on == 0){
                is_on = 1;
                f_on();
			}
			else {
				f_off();
				is_on = 0;
			}

		}
	}

}
