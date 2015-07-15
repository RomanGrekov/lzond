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
	float v_outref;
	float v_out;
	float test;
	float start_pause;
	float start_timeout;
	float v_r;
	float v_l;
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
void read_def_params(conf *my_conf);
void store_conf(conf *my_conf);
void ShowDefaultPatams(void);
void store_param(uint8_t *name, float val);


#endif /* CONFIGURATOR_H_ */
