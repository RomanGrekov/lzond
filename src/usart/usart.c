#include "usart.h"
#include "FreeRTOS.h"


void USART1Init(uint16_t boudrate, uint32_t f_cpu){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Clock port A
    //============Pin 9 (TX) setup===================//
    GPIOA->CRH      &= ~(GPIO_CRH_CNF9_0); //Set pin 9 to alternative push-pull
    GPIOA->CRH      |= GPIO_CRH_CNF9_1; //
    GPIOA->CRH      |= (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0); // 50 MHz
    //============Pin 10 (RX) setup===================//
    GPIOA->CRH      &= ~(GPIO_CRH_CNF10_1); //Set pin 10 to input
    GPIOA->CRH      |= GPIO_CRH_CNF10_0; // without lift to positive
    GPIOA->CRH      &= ~(GPIO_CRH_MODE10_1 | GPIO_CRH_MODE10_0); // input

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable clock for USART1

    USART1->BRR = f_cpu/boudrate; //Calculate value for speed. In my case it's 0x4e2
    USART1->CR1 |= USART_CR1_UE; //Enable USART
    USART1->CR1 &= ~(USART_CR1_M); // 8 data bit
    USART1->CR1 &= ~(USART_CR1_PCE); // Parity disabled
    USART1->CR1 |=(USART_CR1_TE | USART_CR1_RE); // Enable transmit and receive

    USART1->CR2 &= ~(USART_CR2_STOP_0 | USART_CR2_STOP_1); // One stop bit
}

void USART1WriteByte(uint8_t data) {
	while(!(USART1->SR & USART_SR_TXE)); //Waiting until bit TC in register SR not 1
	USART1->DR=data; //Send byte
}

void USART1InterrInit(void){
	USART1->CR1 |= USART_CR1_RXNEIE;/*!<RXNE Interrupt Enable */
	USART1->CR1 |= USART_CR1_TCIE;/*!<Transmission Complete Interrupt Enable */
	NVIC_EnableIRQ(USART1_IRQn);//Allow usart receive unterruption
	NVIC_SetPriority(USART1_IRQn, 15);
}

uint8_t USART1ReadByte(void){
	if (USART1->SR & USART_SR_RXNE){
		return 	USART1->DR;
	}
	return 0;
}

void USART1SendString(uint8_t *data){
	while (*data){
		USART1WriteByte(*data);
		data++;
	}
}

void USART1_IRQHandler(void){
	static portBASE_TYPE xHigherPriorityTaskWoken, xStatus;
	uint8_t a;
	if(USART1->SR & USART_SR_RXNE){
		xHigherPriorityTaskWoken = pdFALSE;
		a = USART1ReadByte();
		xStatus = xQueueSendFromISR(xQueueUsart1Rx, &a, xHigherPriorityTaskWoken);
		if(xStatus != pdTRUE){
			ulog("Can't write USART RX byte to queue", ERROR_LEVEL);
		}
		if( xHigherPriorityTaskWoken == pdTRUE ){
			taskYIELD();
		}
	}
	if((USART1->SR & USART_SR_TC) != 0){//!<Transmission Complete
		USART1->SR &= ~USART_SR_TC; //Clear flag --^
	}
}

void USART1QueueSendString(uint8_t *data){
	portBASE_TYPE xStatus;

	if(xSemaphoreTake(xUsart1TxMutex, portMAX_DELAY) == pdTRUE){
		while(*data){
			xStatus = xQueueSend(xQueueUsart1Tx, data, 100);
			if (xStatus == pdPASS){
				data++;
			}
		}
		xSemaphoreGive(xUsart1TxMutex);
	}
}
void USART1QueueSendByte(uint8_t data){
	if(xSemaphoreTake(xUsart1TxMutex, portMAX_DELAY) == pdTRUE){
        xQueueSend(xQueueUsart1Tx, &data, 100);
		xSemaphoreGive(xUsart1TxMutex);
	}
}

void prvUsart1Transmitter(void *pvParameters) {
	portBASE_TYPE xStatus;
	uint8_t a;
	for (;;) {
		xStatus = xQueueReceive(xQueueUsart1Tx, &a, portMAX_DELAY);
		if (xStatus == pdPASS){
			USART1WriteByte(a);
		}
	}
}

void ulog(uint8_t *data, uint8_t level){
	ulog_raw(data, level);
	ulog_raw("\n\r", level);
}

void ulog_raw(uint8_t *data, uint8_t level){
    if(level <= DEBUG_LEVEL){
        USART1QueueSendString(data);
    }
}

