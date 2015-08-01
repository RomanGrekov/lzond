/*
 * configurator.h
 *
 *  Created on: May 22, 2015
 *      Author: romang
 */

#ifndef CONFIGURATOR_H_
#define CONFIGURATOR_H_
#include "stm32f10x.h"
#include "globs.h"

struct configuration
{
	float v_def;
	float v_r;
	float v_l;
	float v_min;
	float v_out_inc_step;
	float v_out_dec_step;
	float v_out_mid_step;
	float v_rm1;
	float v_rm2;
	float q_ref1;
	float q_ref2;
	float q_ref3;
	uint32_t pause_inc;
	uint32_t pause_dec;
	uint32_t pause_mid;
	uint32_t pause_start;
	uint32_t cat_off;
	uint32_t t_power;
}; // When add new param, do not forget to add in .c file

typedef struct configuration conf;
conf my_conf;

struct version
{
	uint8_t name[SW_VERSION_SIZE];
};
typedef struct version sw_version;

uint8_t is_version_inside(uint8_t *version);
void store_version(uint8_t *version);
void get_version(sw_version *to_store);
void store_def_params(void);
void store_conf(conf *my_conf);
void read_def_params(conf *my_conf);
void ShowDefaultPatams(void);
void store_param(uint8_t *name, float val);


#endif /* CONFIGURATOR_H_ */
