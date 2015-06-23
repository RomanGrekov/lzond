/*
 * dac_logic.c
 *
 *  Created on: Jun 12, 2015
 *      Author: romang
 */

#include "dac_logic.h"
#include "../dac/dac.h"
#include "../configurator/configurator.h"

void taskDacCicle( void *pvParameters )
{
    read_def_params(&my_conf);
	dac_volts(my_conf.v_def);
	vTaskDelay(my_conf.start_pause);

    while(1)
    {

    }
}
