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

#include "if_nms.h"

typedef enum
{
        kStat_INIT,
        kStat_BOOT,
        kStat_MID,
	kStat_REG,		// Registration
	kStat_RDY,		// Ready
        kStat_RUN,
        kStat_ERR
} Stat_t;

typedef struct
{
      uint8_t pl;
      uint8_t p2;
} FuncParam_t;

typedef struct
{
        bool_t regi_confirmed;
} SmartplugInstance_t;

extern SmartplugInstance_t g_smartplug;

typedef void(*Func_OnEnter_t)(FuncParam_t param);
typedef void(*Func_OnTick_t)(uint32_t tick10hz);
typedef void(*Func_OnNMS_message_t)(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
typedef void(*Func_OnV9871_message_t)(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
typedef void(*Func_OnLeave_t)();

typedef struct
{
        Func_OnEnter_t enter_func;
        Func_OnTick_t tick_func;
        Func_OnNMS_message_t nms_rx_func;
        Func_OnV9871_message_t v9871_rx_func; 
        Func_OnLeave_t leave_func;
} SmartplugFuncSpec_t;

extern const SmartplugFuncSpec_t* cur_smartplug_func_;
extern const FuncParam_t null_param;

// init statue
void stat_init();

void stat_reset_smartplug_data();

// get current state
Stat_t stat_get();

// get current state name
const char* stat_get_name();

// set current state
void stat_change(Stat_t stat, FuncParam_t param, bool_t forc_change, const char* by);

// state name lookup
const char* stat_name_lookup(Stat_t stat);

void *stat_shared_buffer(size_t required_size);

