/*
 * cmd_handler.c

 *
 *  Created on: Jun 6, 2015
 *      Author: romang
 */

#include "cmd_handler.h"
#include <stdlib.h>
#include "../usart/usart.h"
#include "../configurator/configurator.h"

static TaskHandle_t xHandleUsartRX;
static TaskHandle_t xHandleCommands;
static struct command_parameter
{
	uint8_t name[COMMAND_NAME_SIZE];
	float val;
};
typedef struct command_parameter parameter;

void commands_init(void)
{
    xQueueUsartCommands = xQueueCreate(COMMANDS_QUEUE_SIZE, sizeof(parameter));
    if (xQueueUsartCommands == NULL) {
    	log_error("Can't create Usart commands queue\n");
    }
    xTaskCreate(prvUsart_1_RX_Handler,(signed char*)"USART RX handler",configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, &xHandleUsartRX);
    xTaskCreate(prvHandleCommands,(signed char*)"Commands handler",configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, &xHandleCommands);

}

void commands_suspend(void)
{
	vTaskSuspend(xHandleCommands);
	vTaskSuspend(xHandleUsartRX);
}

void commands_resume(void)
{
	vTaskResume(xHandleCommands);
	vTaskResume(xHandleUsartRX);
}


void prvUsart_1_RX_Handler(void *pvParameters) {
	portBASE_TYPE xStatus=pdPASS;
	uint8_t a;
	uint8_t state=0;
	uint8_t value[10];
	parameter param;
	uint8_t i;
	for (;;) {
		if (xQueueReceive(xQueueUsart1Rx, &a, portMAX_DELAY) == pdPASS){
			USART1QueueSendByte(a);
			switch (state){
				case 0:
					i=0;
					if(a == 'C'){
						state = 1;
					}
				break;
				case 1: if(a == 'M')state = 2; else state = 0; break;
				case 2: if(a == 'D')state = 3; else state = 0; break;
				case 3: if(a == '_')state = 4; else state = 0; break;
				case 4:
					if (a != '='){
						if (i < sizeof(param.name)){
							param.name[i] = a;
							i++;
							if (i < sizeof(param.name))	param.name[i] = '\0';
						}
					}
					else {
						state = 5;
						i=0;
					}
				break;
				case 5:
					if (a != '\x0D'){
						value[i] = a;
						i++;
						value[i] = '\0';
					}
					else{
						param.val = atof(value);
						xStatus = xQueueSend(xQueueUsartCommands, &param, portMAX_DELAY);
						if (xStatus != pdPASS){
							log_error("Can't put Commands queue");
						}
						cln_scr();
						to_video_mem(0, 0, &param.name);
						state = 0;
					}
			}
		}
	}
}

void prvHandleCommands(void *pvParameters) {
	portBASE_TYPE xStatus=pdPASS;
	parameter param;

	while(1){
        	xStatus = xQueueReceive(xQueueUsartCommands, &param, 10);
        	if (xStatus == pdPASS){
        		store_param(param.name, param.val);
			}
	}
}
