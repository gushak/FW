#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: LED management
*
* History
* Date			Description
* 2017/09/28	file created
*****************************************************************************/

#include "smartplug_def.h"

typedef enum
{
        kLED_RG,
        kLED_DLINK,
} LED_Type_t;

#define LED_MAX 2

#define LED_BLINK_DURATION 20

void led_init();

void led_on_tick(uint32_t tick);

void led_blink(LED_Type_t led_type, bool_t start_onoff_state, uint32_t duration, bool_t repeat);

void led_onoff(LED_Type_t led_type, bool_t is_on);

void led_onoff_all(bool_t is_on);


