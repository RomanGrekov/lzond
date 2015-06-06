/*
 * configurator.h
 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "globs.h"

void prvUsart_1_RX_Handler(void *pvParameters);

#define COMMAND_NAME_SIZE 20

//struct command_parameter
//{
//	uint8_t name[COMMAND_NAME_SIZE];
//	uint8_t name_len;
//	float val;
//	uint8_t param_mark[5];
//};

struct command_parameter
{
	uint8_t name[COMMAND_NAME_SIZE];
	float val;
};
typedef struct command_parameter parameter;

struct configuration
{
	float v_def;
	float v_outref;
	float v_out;
	float test;
};

typedef struct configuration conf;
conf my_conf;

struct version
{
	uint8_t name[SW_VERSION_SIZE];
};
typedef struct version sw_version;

TaskHandle_t xHandleUsartRX;
TaskHandle_t xHandleCommands;

void commands_init(void);
void commands_suspend(void);
void commands_resume(void);
void prvUsart_1_RX_Handler(void *pvParameters);
void prvHandleCommands(void *pvParameters);
uint8_t is_version_inside(uint8_t *version);
void store_version(uint8_t *version);
void get_version(sw_version *to_store);
void store_def_params(void);
void read_def_params(conf *my_conf);
void store_param(conf *my_conf);
void ShowDefaultPatams(void);
void commands_on(void);
void commands_off(void);

#define COMMANDS_QUEUE_SIZE 5
xQueueHandle xQueueUsartCommands;

#endif /* CONFIGURATOR_H_ */
