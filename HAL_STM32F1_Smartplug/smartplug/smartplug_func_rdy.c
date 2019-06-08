/*****************************************************************************
* Copyright(C) 2018 Signal Information & Communication Corp.
* All right reserved.
*
* Description	: smartplug ready status function implementation
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

#define INITIAL_DELAY   200

extern Requester_t req_by;

struct RDY_CONTEXT
{
    uint16_t    initial_delay;
    uint8_t     rdy_tx_count;
    uint32_t    rdy_tx_tick;        
};

static struct RDY_CONTEXT* rdy_;

static void rdy_on_enter(FuncParam_t param);
static void rdy_on_tick(uint32_t tick);
static void rdy_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void rdy_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void rdy_on_leave();

const SmartplugFuncSpec_t* smartplug_func_rdy_spec()
{
    static SmartplugFuncSpec_t spec = 
    {
        rdy_on_enter,
        rdy_on_tick,
        rdy_on_NMS_message,
        rdy_on_V9871_message,
        rdy_on_leave
    };
    
    return &spec;
}

static void rdy_on_enter(FuncParam_t param)
{
      DPRT_STAT("<rdy> enter");
      rdy_ = (struct RDY_CONTEXT*)stat_shared_buffer(sizeof(*rdy_));
      memset(rdy_,0,sizeof(*rdy_));
      rdy_->initial_delay = INITIAL_DELAY;
      rdy_->rdy_tx_count = 0;
      rdy_->rdy_tx_tick = 0;
      led_blink(kLED_DLINK, kTrue, 500, kTrue);
      
}

static void rdy_on_tick(uint32_t tick)
{   
      stat_change(kStat_RUN, null_param, kFalse, "ready confirmed");
}

static void rdy_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
      switch(cmd_stat)
      {
      default:
          default_handle_rx_NMS_message(cmd_stat, msg, msg_len);
          break;
      }
}

static void rdy_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
}

static void rdy_on_leave()
{
      DPRT_STAT("<rdy> leave");
}
