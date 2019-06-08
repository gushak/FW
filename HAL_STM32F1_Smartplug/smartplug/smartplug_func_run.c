/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: RUN state function implementation
*
* History
* Date			Description
* 2017/09/27	file created
*****************************************************************************/

#include "smartplug_func.h"
#include "comm_dbg.h"
#include "if_nms.h"
#include "if_vango.h"
#include "crc16.h"
#include "smartplug_led.h"

struct RUN_CONTEXT
{
    uint32_t    alive_send_sec;
    uint8_t     run_tx_count;
    uint32_t    run_tx_tick;
};

static struct RUN_CONTEXT *run_;

static void run_on_enter(FuncParam_t param);
static void run_on_tick(uint32_t tick);
static void run_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void run_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void run_on_leave();

static void set_next_alive_second();

const SmartplugFuncSpec_t* smartplug_func_run_spec()
{
      static SmartplugFuncSpec_t spec =
      {
            run_on_enter,
            run_on_tick,
            run_on_NMS_message,
            run_on_V9871_message,
            run_on_leave
      };
      
      return &spec;
}
static void run_on_enter(FuncParam_t param)
{
      DPRT_STAT("<run> enter");
      run_ = (struct RUN_CONTEXT*)stat_shared_buffer(sizeof(*run_));
      memset(run_,0,sizeof(*run_));
      set_next_alive_second();
      run_->alive_send_sec = 0;
      run_->run_tx_count = 0;
      run_->run_tx_tick = 0;
}

static void run_on_tick(uint32_t tick)
{
      uint32_t diff = sys_tick_diff(tick, run_->run_tx_tick);
      
      if(diff > g_config.bc.def.alive_period)
      {
            DPRT("Send ALIVE_REQ");
            //nms_send_command(CS_RTC_REQ);
            set_next_alive_second();
            run_->run_tx_tick = tick;
      }
      else
      {
            //stat_change(kStat_RDY, null_param, kFalse, "alive timeout");          
      }
      
}

static void run_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
	switch (cmd_stat)
	{
	default:
		default_handle_rx_NMS_message(cmd_stat, msg, msg_len);
		break;
	}
}

static void run_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
}

static void run_on_leave()
{
      DPRT_STAT("<run> leave");
}

static void set_next_alive_second()
{
      uint32_t alive_sec = cfg_alive_period(&g_config);
      
      run_->alive_send_sec = alive_sec+sys_uptime();
}
