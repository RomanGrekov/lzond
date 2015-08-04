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
	my_conf.v_r = 0.52;
	my_conf.v_l = 0.37;
	my_conf.v_min = 0.05;
	my_conf.v_out_inc_step = 0.01;
	my_conf.v_out_dec_step = 0.01;
	my_conf.v_out_mid_step = 0.005;
	my_conf.v_rm1 = 0.7;
	my_conf.v_rm2 = 0.88;
	my_conf.q_ref1 = 0.4;
	my_conf.q_ref2 = 0.5;
	my_conf.q_ref3 = 0.6;
	my_conf.pause_inc = 20;
	my_conf.pause_dec = 20;
	my_conf.pause_mid = 20;
	my_conf.pause_start = 5000;
	my_conf.cat_off = 1200;
	my_conf.t_power = 3000;
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
	xprintf("\n\r%s=%f\n", name, val);

    if (!memcmp(name, "v_def", sizeof("v_def"))){
    	my_conf.v_def = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_r", sizeof("v_r"))){
    	my_conf.v_r = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_l", sizeof("v_l"))){
    	my_conf.v_l = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_min", sizeof("v_min"))){
    	my_conf.v_min = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_out_inc_step", sizeof("v_out_inc_step"))){
    	my_conf.v_out_inc_step = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_out_dec_step", sizeof("v_out_dec_step"))){
    	my_conf.v_out_dec_step = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_out_mid_step", sizeof("v_out_mid_step"))){
    	my_conf.v_out_mid_step = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_rm1", sizeof("v_rm1"))){
    	my_conf.v_rm1 = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "v_rm2", sizeof("v_rm2"))){
    	my_conf.v_rm2 = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "q_ref1", sizeof("q_ref1"))){
    	my_conf.q_ref1 = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "q_ref2", sizeof("q_ref2"))){
    	my_conf.q_ref2 = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "q_ref3", sizeof("q_ref3"))){
    	my_conf.q_ref3 = val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "pause_inc", sizeof("pause_inc"))){
    	my_conf.pause_inc = (int)val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "pause_dec", sizeof("pause_dec"))){
    	my_conf.pause_dec = (int)val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "pause_mid", sizeof("pause_mid"))){
    	my_conf.pause_mid = (int)val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "cat_off", sizeof("cat_off"))){
    	my_conf.cat_off = (int)val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "pause_start", sizeof("pause_start"))){
    	my_conf.pause_start = (int)val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "t_power", sizeof("t_power"))){
    	my_conf.t_power = (int)val;
    	xprintf("OK\n");
    }
    if (!memcmp(name, "save", sizeof("save"))){
    	store_conf(&my_conf);
    	xprintf("OK\n");
    }

}


void ShowDefaultPatams(void)
{
	uint8_t symb[sizeof(float)];
    xprintf("\nSystem configuration\n");

    //cln_scr();
    //to_video_mem(0, 0, "Setting vars...");

    read_def_params(&my_conf);
    xprintf("Current parameters:\n");
    xprintf("v_def: %f\n", my_conf.v_def);
    xprintf("v_r: %f\n", my_conf.v_r);
    xprintf("v_l: %f\n", my_conf.v_l);
    xprintf("v_min: %f\n", my_conf.v_min);
    xprintf("v_out_inc_step: %f\n", my_conf.v_out_inc_step);
    xprintf("v_out_dec_step: %f\n", my_conf.v_out_dec_step);
    xprintf("v_out_mid_step: %f\n", my_conf.v_out_mid_step);
    xprintf("v_rm1: %f\n", my_conf.v_rm1);
    xprintf("v_rm2: %f\n", my_conf.v_rm2);
    xprintf("q_ref1: %f\n", my_conf.q_ref1);
    xprintf("q_ref2: %f\n", my_conf.q_ref2);
    xprintf("q_ref3: %f\n", my_conf.q_ref3);
    xprintf("pause_inc: %u\n", my_conf.pause_inc);
    xprintf("pause_dec: %u\n", my_conf.pause_dec);
    xprintf("pause_mid: %u\n", my_conf.pause_mid);
    xprintf("pause_start: %u\n", my_conf.pause_start);
    xprintf("cat_off: %u\n", my_conf.cat_off);
    xprintf("t_power: %u\n", my_conf.t_power);
}

