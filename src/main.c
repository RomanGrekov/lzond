#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "clock/clock.h"
#include "hd44780/hd44780.h"
#include "encoder/encoder.h"
#include "configurator/configurator.h"
#include "led/led.h"
#include "usart/usart.h"
#include "flash/flash.h"
#include "string_lib/string_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include "globs.h"


static void prvStart( void *pvParameters );
static void prvGetTemp( void *pvParameters );
static void prvCheckBtn1( void *pvParameters );
void vApplicationTickHook( void );

void init_gpio(void);
void adc_init(void);
void init_btn1(void);
uint16_t get_adc_value(void);

uint8_t state = 0;

int main(void)
{
	InitRCC();
	init_gpio();
	adc_init();
	init_btn1();
    lcd_init();
    USART1Init(9600, configCPU_CLOCK_HZ);
    USART1InterrInit();


	xLcdMutex = xSemaphoreCreateMutex();
	xTaskCreate(prvStart,(signed char*)"Start tasks",configMINIMAL_STACK_SIZE,
	            NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();
    while(1);

    return 0;
}

void prvStart(void *pvParameters)
{
	sw_version ver;

	while(1){
		switch(state){
			case 0:
				lcd_clrscr();
				to_video_mem(0, 0, "FreeRTOS!-v8.0.1\nLambda zond v1.0");

				xTaskCreate(blink_tsk,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
						NULL, tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(prvGetTemp,(signed char*)"Get_temp",configMINIMAL_STACK_SIZE,
	//            NULL, tskIDLE_PRIORITY + 1, NULL);
				xTaskCreate(prvCheckBtn1,(signed char*)"Check_btn1",configMINIMAL_STACK_SIZE,
						NULL, tskIDLE_PRIORITY + 1, NULL);

                xQueueUsart1Tx = xQueueCreate(USART1_TX_QUEUE_SIZE, sizeof(unsigned char));
                if (xQueueUsart1Tx != NULL){
                }

                xUsart1TxMutex = xSemaphoreCreateMutex();
                    if (xUsart1TxMutex != NULL) {
                    }

                xTaskCreate(prvUsart1Transmitter,(signed char*)"USART1_transmitter",configMINIMAL_STACK_SIZE,
                            NULL, tskIDLE_PRIORITY + 1, NULL);

                xQueueUsart1Rx = xQueueCreate(USART1_RX_QUEUE_SIZE, sizeof(unsigned char));
                if (xQueueUsart1Rx == NULL) {
                	ulog("Can't create Usart RX queue\n", ERROR_LEVEL);
                }

                ulog_raw("Lambds zond v", INFO_LEVEL);
                get_version(&ver);
                ulog(ver.name, INFO_LEVEL);

                commands_init();
                commands_suspend();
                state = 1;

				break;
			case 1:
				break;
			case 2:
				break;
		}

	}
}


void prvGetTemp(void *pvParameters)
{
    uint16_t old_value = 0;
    uint32_t value = 0;
    uint32_t values=0;
    uint8_t iter = 0;
    uint8_t th = 5;
	char buffer[9];
	strcat(buffer, "ADC ");
	while(1)
	{
		if (iter == 64){
			value = values/64;
			if ((value > (old_value + th)) || ((value + th) < old_value)){
				old_value = value;
				itoa(value, buffer+4, 10);
				to_video_mem(0, 0, buffer);
			}
			iter = 0;
			values = 0;
		}
		values = values + get_adc_value();
		iter++;
	}

}


static void prvCheckBtn1( void *pvParameters )
{
	uint8_t btn_state = 0, btn_state_old = 0;
	uint8_t is_on = 0;
	uint8_t symb[5];
	while(1){
		btn_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
		if (btn_state != btn_state_old){
			btn_state_old = btn_state;
			if(btn_state == 0){
				if (is_on == 0){
					commands_resume();
                        ulog("Good day Dmitriy Sergeevich!", INFO_LEVEL);
                                        cln_scr();
                                        to_video_mem(0, 0, "Setting vars...");
                        if (is_version_inside(&SW_VERSION) == 0){
                                store_version(SW_VERSION);
                                store_def_params();
                                ulog("First time", DEBUG_LEVEL);
                        }
                        read_def_params(&my_conf);
                        ulog("Current parameters:", INFO_LEVEL);
                        ulog_raw("v_def ", INFO_LEVEL);
                        float_to_string(my_conf.v_def, symb);
                        ulog(symb, INFO_LEVEL);
                        ulog_raw("v_out ", INFO_LEVEL);
                        float_to_string(my_conf.v_out, symb);
                        ulog(symb, INFO_LEVEL);

                    is_on = 1;
				}
				else {
					cln_scr();
					to_video_mem(0, 0, "Default state");
					commands_suspend();
					is_on = 0;
				}

			}
		}
	}

}


void HardFault_Handler(void)
{// пустой обработчик вместо бесконечного цикла
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}

void vApplicationTickHook( void )
{
    unsigned char a=0;
}

void init_gpio(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void init_btn1(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void adc_init()
{
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

 // настройки ADC
 ADC_InitTypeDef ADC_InitStructure;
 ADC_StructInit(&ADC_InitStructure);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // режим работы - одиночный, независимый
 ADC_InitStructure.ADC_ScanConvMode = DISABLE; // не сканировать каналы, просто измерить один канал
 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // однократное измерение
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // без внешнего триггера
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //выравнивание битов результат - прижать вправо
 ADC_InitStructure.ADC_NbrOfChannel = 1; //количество каналов - одна штука
 ADC_Init(ADC1, &ADC_InitStructure);
 ADC_Cmd(ADC1, ENABLE);

 // настройка канала
 ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);

 // калибровка АЦП
 ADC_ResetCalibration(ADC1);
 while (ADC_GetResetCalibrationStatus(ADC1));
 ADC_StartCalibration(ADC1);
 while (ADC_GetCalibrationStatus(ADC1));
}

uint16_t get_adc_value()
{
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);
 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
 return ADC_GetConversionValue(ADC1);
}

//void arr_cp(uint8_t *dst, const uint8_t *src)
//{
//	for (; *src != '\0'; src++, dst++) *dst = *src;
//}


