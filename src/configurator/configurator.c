/*
 * configurator.c

 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#include "configurator.h"
#include "../flash/flash.h"
#include "../usart/usart.h"
#include "../string_lib/string_lib.h"
#include <stdlib.h>
#include <stdio.h>


void commands_init(void)
{
    xQueueUsartCommands = xQueueCreate(COMMANDS_QUEUE_SIZE, sizeof(parameter));
    if (xQueueUsartCommands == NULL) {
    	ulog("Can't create Usart commands queue\n", ERROR_LEVEL);
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

uint8_t is_version_inside(uint8_t *version){
	sw_version ver;

	get_version(&ver);
	if(memcmp(ver.name, version, SW_VERSION_SIZE)){
		return 0;
	}
	return 1;
}

void get_version(sw_version *to_store)
{
	flash_read_struct(to_store, sizeof(sw_version), 0);
}

void store_version(uint8_t *version)
{
	flash_erase_page(params_addr);
	flash_write_struct(version, sizeof(sw_version), 0);
}

void store_def_params(void)
{
	conf my_conf;
	my_conf.v_def = 0.45;
	my_conf.v_out = 124;
	my_conf.v_outref = 0.7;
	my_conf.test = 1035;
	store_param(&my_conf);

}

void read_def_params(conf *my_conf)
{
	flash_read_struct(my_conf, sizeof(conf), sizeof(sw_version));
}

void store_param(conf *my_conf)
{
	sw_version ver;
	get_version(&ver);
	flash_erase_page(params_addr);
	flash_write_struct(&ver, sizeof(sw_version), 0);
	flash_write_struct(my_conf, sizeof(conf), sizeof(sw_version));
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
							ulog("Can't put Commands queue", ERROR_LEVEL);
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
	uint8_t symb[5];
	float n=0;

	while(1){
        	xStatus = xQueueReceive(xQueueUsartCommands, &param, 10);
        	if (xStatus == pdPASS){
        		ulog_raw("\n\r", DEBUG_LEVEL);
        		ulog_raw(param.name, DEBUG_LEVEL);
        		ulog_raw("=", DEBUG_LEVEL);
                float_to_string(param.val, symb);
                ulog(symb, DEBUG_LEVEL);

                if (!memcmp(param.name, "v_def", sizeof("v_def"))){
                	my_conf.v_def = param.val;
                	ulog("OK", DEBUG_LEVEL);
                }
                if (!memcmp(param.name, "v_out", sizeof("v_out"))){
                	my_conf.v_out = param.val;
                	ulog("OK", DEBUG_LEVEL);
                }
                if (!memcmp(param.name, "save", sizeof("save"))){
                	store_param(&my_conf);
                	ulog("OK", DEBUG_LEVEL);
                }
			}
	}
}

void ShowDefaultPatams(void)
{
	uint8_t symb[sizeof(float)];
    ulog("Good day Dmitriy Sergeevich!", INFO_LEVEL);
                    cln_scr();
                    to_video_mem(0, 0, "Setting vars...");
    read_def_params(&my_conf);
    ulog("Current parameters:", INFO_LEVEL);
    ulog_raw("v_def ", INFO_LEVEL);
    float_to_string(my_conf.v_def, symb);
    ulog(symb, INFO_LEVEL);
    ulog_raw("v_out ", INFO_LEVEL);
    float_to_string(my_conf.v_out, symb);
    ulog(symb, INFO_LEVEL);
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

