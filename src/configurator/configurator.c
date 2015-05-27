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
                NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(prvHandleCommands,(signed char*)"Commands handler",configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, NULL);

}

uint8_t is_version_inside(uint8_t *version){
	sw_version ver;
	uint8_t *ver_p = &ver.name;
	uint8_t i = 0;
	flash_read_struct(ver_p, sizeof(ver), 0);
	while(*version){
		if (*version != *ver_p) return 0;
		else{
			version++;
			ver_p++;
		}
	}
	return 1;
}

void get_version(sw_version *to_store)
{
	flash_read_struct(to_store, sizeof(sw_version), 0);
}

void store_version(uint8_t *version)
{
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
	uint8_t symb[10];
	uint8_t state=0;
	uint8_t value[10];
	parameter param;
	parameter *param_p = &param;
	uint8_t i;
	for (;;) {
		if (xQueueReceive(xQueueUsart1Rx, &a, portMAX_DELAY) == pdPASS){
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
						//param.name_len = i;
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
						xStatus = xQueueSend(xQueueUsartCommands, param_p, portMAX_DELAY);
						if (xStatus != pdPASS){
							ulog("Can't put Commands queue", ERROR_LEVEL);
						}
						cln_scr();
						to_video_mem(0, 0, param_p->name);
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
                	ulog_raw("Parameter ", DEBUG_LEVEL);
                	ulog_raw(param.name, DEBUG_LEVEL);
                	ulog_raw(" has changed to ", DEBUG_LEVEL);
                	float_to_string(param.val, symb);
                	ulog(symb, DEBUG_LEVEL);

                }
                if (!memcmp(param.name, "save", sizeof("save"))){
                	store_param(&my_conf);
                	ulog("saved!", DEBUG_LEVEL);
                }
			}
	}
}