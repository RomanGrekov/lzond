/*
 * configurator.c

 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#include "configurator.h"
#include "../flash/flash.h"


void commands_init(void)
{
    xQueueUsartCommands = xQueueCreate(COMMANDS_QUEUE_SIZE, sizeof(parameter));
    if (xQueueUsartCommands == NULL) {
    	log("Can't create Usart commands queue\n", ERROR_LEVEL);
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

void store_version(uint8_t *version)
{
	sw_version ver;
	flash_write_struct(version, sizeof(ver), 0);
}

uint32_t store_def_params(void)
{
	uint32_t amount=0;

	store_param("v_def", 0.45);
	store_param("v_outref", 0.7);
	store_param("v_out", 0.7);
	store_param("v_out_lim_inc", 0.9);
	amount = store_param("start_timeout", 100);
	amount = store_param("test_param", 25601.05);
	return amount;
}

void read_def_params(parameter params[], uint32_t amount)
{
	parameter p;
	parameter *p_p = &p;
	sw_version ver_struct;
	for(uint32_t i=0; i<amount;i++){
        flash_read_struct(p_p, sizeof(p), sizeof(ver_struct) + sizeof(p) * i);
        params[i] = p;
	}
}

uint32_t store_param(uint8_t *name, float val)
{
	parameter my_param;
	sw_version ver;
	static number=0;

	strcpy(my_param.name, name);
	my_param.val = val;
	flash_write_struct(&my_param, sizeof(my_param), sizeof(ver) + sizeof(my_param) * number);
	number++;
	return (number-1);
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
						param.name_len = i;
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
						param.val = atoi(value);
						xStatus = xQueueSend(xQueueUsartCommands, param_p, portMAX_DELAY);
						if (xStatus != pdPASS){
							log("Can't put Commands queue", ERROR_LEVEL);
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
	parameter *param_p = &param;
	uint8_t symb[COMMAND_NAME_SIZE + 10 + 10];
	uint8_t val[10];
	uint8_t *val_p = val;
	uint8_t val_len=0;

	while(1){
        	xStatus = xQueueReceive(xQueueUsartCommands, param_p, 10);
        	if (xStatus == pdPASS){
        		strcpy(symb, "\n\r");
        		strcpy(symb+2, param_p->name);
        		strcpy(symb+param_p->name_len+2, " = ");
        		for(uint8_t i=0; i<10; i++)val[i] = '\0';
        		itoa(param_p->val, val, 10);
        		val_len = 0;
        		while(*val_p != '\0'){val_len++; val_p++;}
        		strcpy(symb+param_p->name_len+3+2, val);
        		strcpy(symb+param_p->name_len+3+2+val_len+1, "\n\r");
				log(symb, DEBUG_LEVEL);
			}
	}
}
