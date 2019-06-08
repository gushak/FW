/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: smartplug function implementation
*
* History
* Date			Description
* 2018/09/21	file created
*****************************************************************************/

#include "smartplug_func.h"
#include "comm_dbg.h"
#include "smartplug_stat.h"

struct INIT_CONTEXT
{
        uint8_t init_delay_cnt;
};

static struct INIT_CONTEXT init_;

static void init_on_enter(FuncParam_t param);
static void init_on_tick(uint32_t tick);
static void init_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void init_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void init_on_leave();

const SmartplugFuncSpec_t* smartplug_func_init_spec()
{
        static SmartplugFuncSpec_t spec = 
        {
                  init_on_enter, 
                  init_on_tick,
                  init_on_NMS_message,
                  init_on_V9871_message,
                  init_on_leave
        };
        
        return &spec;
        
}

static void init_on_enter(FuncParam_t param)
{
        init_.init_delay_cnt = 0;
        DPRT_STAT("<init> enter");
}       

static void init_on_tick(uint32_t tick)
{
        if(++init_.init_delay_cnt == 200)
        {
                stat_change(kStat_BOOT, null_param, kFalse,"cold booting");
        }
}

static void init_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
}

static void init_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
}

static void init_on_leave()
{
	DPRT_STAT("<init> leave");  
}