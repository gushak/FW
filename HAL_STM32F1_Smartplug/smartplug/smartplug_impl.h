#pragma once

/*****************************************************************************
* Copyright(C) 2017 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: smartprug implementation
*
* History
* Date			Description
* 2018/09/14	file created
*****************************************************************************/

#include "smartplug_def.h"
#include "smartplug_stat.h"

// initialize smartplug func
void impl_init();

// run function at tick interval
void impl_on_tick();
void impl_check_rx();
void impl_reset_alive_timeout();
void impl_on_hw_tick();
