#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "clock/clock.h"
#include "hd44780/hd44780.h"
#include "encoder/encoder.h"
#include "configurator/configurator.h"
#include "led/led.h"
#include "usart/usart.h"
#include "flash/flash.h"
#include "string_lib/string_lib.h"
#include "adc/adc.h"
#include "dac/dac.h"
#include "dac_logic/dac_logic.h"
#include "buttons/buttons.h"
#include "cmd_handler/cmd_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include "globs.h"


void taskStartup(void *pvParameters );
void Init(void);
void vApplicationTickHook( void );
void prvCheckBtn1( void *pvParameters );
void prvADC( void *pvParameters );
void commands_on(void);
void commands_off(void);


int main(void)
{
	InitRCC();
	init_led();
	adc_init();
	init_dac();
	init_buttons();
    lcd_init();
    USART1Init(9600, configCPU_CLOCK_HZ);
    USART1InterrInit();

    Init();
    xTaskCreate(taskStartup,(signed char*)"Startup",configMINIMAL_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
    while(1);

    return 0;
}

void Init(void)
{
	xLcdMutex = xSemaphoreCreateMutex();
	xTaskCreate(prvLcdShow,(signed char*)"LcdShow",configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(blink_tsk,(signed char*)"LED1",configMINIMAL_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 1, NULL);
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

    xTaskCreate(prvADC,(signed char*)"ADC-DAC",configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, NULL);

    xQueueUsart1Rx = xQueueCreate(USART1_RX_QUEUE_SIZE, sizeof(unsigned char));
    if (xQueueUsart1Rx == NULL) {
        ulog("Can't create Usart RX queue\n", ERROR_LEVEL);
    }

}

void taskStartup(void *pvParameters )
{
	sw_version ver;

    lcd_clrscr();
    to_video_mem(0, 0, "FreeRTOS!-v8.0.1\nLambda zond v1.0");

    if (is_version_inside(&SW_VERSION) == 0){
            store_version(SW_VERSION);
            store_def_params();
            ulog("First start", DEBUG_LEVEL);
    }
    ulog_raw("Lambds zond v", INFO_LEVEL);
    get_version(&ver);
    ulog(ver.name, INFO_LEVEL);

    commands_init();
    commands_suspend();

    get_half_period(5000, 1);

    vTaskDelete(NULL);

}

void prvADC(void *pvParameters )
{
	float v;
	while(1){
		v = get_adc_volts();
		dac_volts(v);
	}
}

void commands_on(void)
{
    commands_resume();
    ShowDefaultPatams();
}

void commands_off(void)
{
	cln_scr();
	to_video_mem(0, 0, "Default state");
	commands_suspend();
}

void prvCheckBtn1( void *pvParameters )
{
	while(1)btn1_react(commands_on, commands_off);
}


void HardFault_Handler(void)
{// пустой обработчик вместо бесконечного цикла
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}

void vApplicationTickHook( void )
{
    unsigned char a=0;
}
