/*
 * cmd_handler.h
 *
 *  Created on: Jun 6, 2015
 *      Author: romang
 */

#ifndef CMD_HANDLER_H_
#define CMD_HANDLER_H_

#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define COMMAND_NAME_SIZE 20
#define COMMANDS_QUEUE_SIZE 5


xQueueHandle xQueueUsartCommands;

void prvUsart_1_RX_Handler(void *pvParameters);
void commands_init(void);
void commands_suspend(void);
void commands_resume(void);
void prvHandleCommands(void *pvParameters);


#endif /* CMD_HANDLER_H_ */
