#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: system interrupt routine
*
* History
* Date			Description
* 2018/09/21	file created
*****************************************************************************/

#include "smartplug_def.h"

void smartplug_set_main_loop_run();
void rtc_init();
void UserRTC_GetTimer(void);
void UserRTC_SetTimer(uint8_t* Buf);

