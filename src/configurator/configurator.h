/*
 * configurator.h
 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_

#include "../usart/usart.h"
void prvUsart_1_RX_Handler(void *pvParameters);

#define COMMAND_NAME_SIZE 20

struct command_parameter
{
	uint8_t name[COMMAND_NAME_SIZE];
	uint8_t name_len;
	float val;
	const uint8_t *param_mark;
};
typedef struct command_parameter parameter;

void commands_init(void);
void prvUsart_1_RX_Handler(void *pvParameters);
void prvHandleCommands(void *pvParameters);
uint8_t is_version_inside(uint8_t *version);
void store_version(uint8_t *version);
uint32_t store_param(uint8_t *name, float val);
uint32_t store_def_params(void);

#define COMMANDS_QUEUE_SIZE 5
xQueueHandle xQueueUsartCommands;

#endif /* CONFIGURATOR_H_ */
