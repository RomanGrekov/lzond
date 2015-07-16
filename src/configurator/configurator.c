/*
 * configurator.c

 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#include "configurator.h"
#include "../cmd_handler/cmd_handler.h"
#include "../flash/flash.h"
#include "../log/log.h"
#include "../xprintf/xprintf.h"
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
	my_conf.v_out = 0.7;
	my_conf.v_outref = 0.7;
	my_conf.test = 1035;
	my_conf.start_pause = 5000;
	my_conf.start_timeout = 100000;
	my_conf.v_r = 0.52;
	my_conf.v_l = 0.37;
	my_conf.k1 = 1.5;
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
	xprintf("\n\r%s=%f", name, val);

    if (!memcmp(name, "v_def", sizeof("v_def"))){
    	my_conf.v_def = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "v_out", sizeof("v_out"))){
    	my_conf.v_out = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "start_pause", sizeof("start_pause"))){
    	my_conf.start_pause = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "start_timeout", sizeof("start_timeout"))){
    	my_conf.start_timeout = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "v_r", sizeof("v_r"))){
    	my_conf.v_r = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "v_l", sizeof("v_l"))){
    	my_conf.v_l = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "k1", sizeof("k1"))){
    	my_conf.k1 = val;
    	xprintf("OK");
    }
    if (!memcmp(name, "save", sizeof("save"))){
    	store_conf(&my_conf);
    	xprintf("OK");
    }

}


void ShowDefaultPatams(void)
{
	uint8_t symb[sizeof(float)];
    xprintf("Good day Dmitriy Sergeevich!\n");

    cln_scr();
    to_video_mem(0, 0, "Setting vars...");

    read_def_params(&my_conf);
    xprintf("Current parameters:\n");
    xprintf("v_def: %f\n", my_conf.v_def);
    xprintf("v_out: %f\n", my_conf.v_out);
    xprintf("start_pause: %f\n", my_conf.start_pause);
    xprintf("start_timeout: %f\n", my_conf.start_timeout);
    xprintf("v_r: %f\n", my_conf.v_r);
    xprintf("v_l: %f\n", my_conf.v_l);
}

