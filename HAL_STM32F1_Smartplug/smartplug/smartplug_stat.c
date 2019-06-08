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

#include "smartplug_stat.h"
#include "comm_dbg.h"
#include "smartplug_func.h"
#include <assert.h>

#define STATE_BUF_SIZE 64

SmartplugInstance_t g_smartplug;

static Stat_t smartplug_stat_ = kStat_INIT;

static uint8_t state_buf_[STATE_BUF_SIZE];

const SmartplugFuncSpec_t* cur_smartplug_func_;

const FuncParam_t null_param;

static const SmartplugFuncSpec_t* stat_spec_lookup(Stat_t stat);

void stat_init()
{
        stat_reset_smartplug_data();
        smartplug_stat_ = kStat_INIT;
        cur_smartplug_func_ = smartplug_func_init_spec();
}

void stat_reset_smartplug_data()
{
        memset(&g_smartplug, 0 ,sizeof(g_smartplug));
}

Stat_t stat_get()
{
	return smartplug_stat_;
}

const char* stat_get_name()
{
        return stat_name_lookup(smartplug_stat_);
}

void stat_change(Stat_t stat, FuncParam_t param, bool_t force_change, const char* by)
{
        if (stat == smartplug_stat_)
        {
                if (!force_change)
                {
			DOUT("change: same state - %s", stat_name_lookup(smartplug_stat_), by);
			return;
                }
        }
        
	DOUT("change: %s --> %s - %s", stat_name_lookup(smartplug_stat_), stat_name_lookup(stat), by);
        
        cur_smartplug_func_->leave_func();
        smartplug_stat_ = stat;
        cur_smartplug_func_ = stat_spec_lookup(smartplug_stat_);
        cur_smartplug_func_->enter_func(param);
}

const char* stat_name_lookup(Stat_t stat)
{
        const char* stat_names[] = {
                "INIT",
                "BOOT",
                "RDY",
                "RUN",
        };
        
        return stat_names[stat];
}

void * stat_shared_buffer(size_t required_size)
{
        assert(required_size <= STATE_BUF_SIZE);
        return state_buf_;
}

static const SmartplugFuncSpec_t* stat_spec_lookup(Stat_t stat)
{
        const SmartplugFuncSpec_t* spec;
        
        switch (stat)
        {
                case kStat_BOOT:
                        spec = smartplug_func_boot_spec();
                        break;
                        
                case kStat_RDY:
                        spec = smartplug_func_rdy_spec();
                        break;
                        
                case kStat_RUN:
                        spec = smartplug_func_run_spec();
                        break;
                default:
                        break;             
                        
	}

	return spec;
}




