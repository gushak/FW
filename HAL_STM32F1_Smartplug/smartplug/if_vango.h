#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: vango port interface
*
* History
* Date			Description
* 2018/10/05	file created
*****************************************************************************/

#include "smartplug_def.h"

typedef struct
{
        uint32_t last_tx_tick;
} VANGOsession_t;

void vango_interface_init();

void vango_interface_init1();

void vango_interface_init2();

void vango_interface_init3();

void vango_send_packet(const uint8_t* packet, uint16_t packet_len);
