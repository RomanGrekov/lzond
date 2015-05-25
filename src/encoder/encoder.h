#ifndef ENCODERS_H_INCLUDED
#define ENCODERS_H_INCLUDED
#ifndef STM32F10X_H
#include "stm32f10x.h"
#endif
#ifndef STM32F10X_GPIO_H
#include "stm32f10x_gpio.h"
#endif

#define PORT GPIOB
#define INP_REG IDR

#define ENC_PIN1 6
#define ENC_PIN2 7

typedef struct{
    uint32_t value;
    uint32_t old_value;
    uint32_t step;
    uint32_t min;
    uint32_t max;
    uint32_t coef;
    uint32_t pin1;
    uint32_t pin2;
    uint8_t old_state;
    uint8_t direction;
}Encoder;

void EncoderScan(Encoder *enc_struct);

void EncoderInit(void);

#endif /* ENCODERS_H_INCLUDED */
