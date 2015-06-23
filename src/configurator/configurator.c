/*
 * configurator.c

 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#include "configurator.h"
#include "../cmd_handler/cmd_handler.h"
#include "../flash/flash.h"
#include "../usart/usart.h"
#include "../string_lib/string_lib.h"
#include <stdlib.h>
//#include <stdio.h>

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
	my_conf.start_pause = 5000;
	store_conf(&my_conf);

}

void read_def_params(conf *my_conf)
{
	flash_read_struct(my_conf, sizeof(conf), sizeof(sw_version));
}

void store_conf(conf *my_conf)
{
	sw_version ver;
	get_version(&ver);
	flash_erase_page(params_addr);
	flash_write_struct(&ver, sizeof(sw_version), 0);
	flash_write_struct(my_conf, sizeof(conf), sizeof(sw_version));
}

void store_param(uint8_t *name, float val)
{
	uint8_t symb[5];
	ulog_raw("\n\r", DEBUG_LEVEL);
	ulog_raw(name, DEBUG_LEVEL);
	ulog_raw("=", DEBUG_LEVEL);
    float_to_string(val, symb);
    ulog(symb, DEBUG_LEVEL);

    if (!memcmp(name, "v_def", sizeof("v_def"))){
    	my_conf.v_def = val;
    	ulog("OK", DEBUG_LEVEL);
    }
    if (!memcmp(name, "v_out", sizeof("v_out"))){
    	my_conf.v_out = val;
    	ulog("OK", DEBUG_LEVEL);
    }
    if (!memcmp(name, "start_pause", sizeof("start_pause"))){
    	my_conf.start_pause = val;
    	ulog("OK", DEBUG_LEVEL);
    }
    if (!memcmp(name, "save", sizeof("save"))){
    	store_conf(&my_conf);
    	ulog("OK", DEBUG_LEVEL);
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
    ulog_raw("start_pause ", INFO_LEVEL);
    float_to_string(my_conf.start_pause, symb);
    ulog(symb, INFO_LEVEL);
}

