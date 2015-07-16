#ifndef USART_H
#define USART_H

#include "stm32f10x.h"
#include "globs.h"

void USART1Init(uint16_t boudrate, uint32_t f_cpu);
void USART1WriteByte(uint8_t data);
void USART1InterrInit(void);
uint8_t USART1ReadByte(void);
void USART1SendString(uint8_t *data);

void prvUsart1Transmitter(void *pvParameters);
void USART1QueueSendString(uint8_t *data);
void USART1QueueSendByte(uint8_t data);


#define USART1_RX_QUEUE_SIZE 32
#define USART1_TX_QUEUE_SIZE 32

xQueueHandle xQueueUsart1Rx;
xQueueHandle xQueueUsart1Tx;
xSemaphoreHandle xUsart1TxMutex;


#endif
