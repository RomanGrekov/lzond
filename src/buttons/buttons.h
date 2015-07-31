/*
 * buttons.h
 *
 *  Created on: Jun 6, 2015
 *      Author: romang
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

#define BTN1_PORT GPIOB
#define BTN1_PIN GPIO_Pin_8
#define P1_PIN GPIO_Pin_4
#define P2_PIN GPIO_Pin_5

void init_buttons(void);
uint8_t check_button(GPIO_TypeDef *port, uint16_t pin);
uint8_t check_btn1(void);
uint8_t check_p1(void);
uint8_t check_p2(void);
void btn1_pressed(void (*f)(void));
void btn1_react(void (*f_on)(void), void (*f_off)(void));


#endif /* BUTTONS_H_ */
