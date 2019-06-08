#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: modem configuration
*
* History
* Date			Description
* 2018/09/20	file created
*****************************************************************************/

#include "smartplug_def.h"
#include "smartplug_stat.h"

typedef enum
{
    kReqFromNMS,
    kReqFromV9871
}Requester_t;

const SmartplugFuncSpec_t* smartplug_func_init_spec();
const SmartplugFuncSpec_t* smartplug_func_boot_spec();
const SmartplugFuncSpec_t* smartplug_func_reg_spec();
const SmartplugFuncSpec_t* smartplug_func_rdy_spec();
const SmartplugFuncSpec_t* smartplug_func_run_spec();
const SmartplugFuncSpec_t* smartplug_func_err_spec();

void default_handle_rx_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
void handle_CS_DEV_INFO_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len);
void handle_CS_DEV_CONF_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len);
void handle_CS_VANGO_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len);
void handle_CS_CTRL_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len);
void handle_CS_RELAY_STATUS_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len);
void handle_CS_RCTRL_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len);
void handle_CS_RTC_ACK(Requester_t req_by, uint8_t* msg, uint16_t msg_len);