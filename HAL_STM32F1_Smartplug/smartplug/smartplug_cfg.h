#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: smartplug configuration
*
* History
* Date			Description
* 2018/09/28	file created
*****************************************************************************/

#include "smartplug_def.h"
#include "smartplug_flash.h"

uint16_t cfg_alive_period(const CurConfig_t* cfg);
void cfg_set_device_default(CurConfig_t* cfg, Rtc_BaseConfig_t* RtcCfg);
uint8_t load_flash_info(uint32_t addr, uint16_t size);

