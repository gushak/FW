/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: BOOT state function implementation
*
* History
* Date			Description
* 2018/09/27	file created
*****************************************************************************/
#include "stm32f1xx_hal.h"
#include "smartplug_func.h"
#include "comm_dbg.h"
#include "if_nms.h"
#include "SM_V9871.h"
#include "smartplug_led.h"

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef DateToUpdate;

//extern V9871_StateMachine_t v9871_rx_sm;

struct BOOT_CONTEXT
{
    uint8_t     boot_tx_step;
    uint8_t     boot_tx_count;
    uint32_t    boot_tx_tick;      
};

static struct BOOT_CONTEXT* boot_;

static void boot_on_enter(FuncParam_t param);
static void boot_on_tick(uint32_t tick);
static void boot_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void boot_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len);
static void boot_on_leave();


static void handle_BOOTUP_ACK(KEPCO_5034_RES_t* res);
static void handle_CS_RTC_CONF_ACK(Requester_t req_by, uint8_t *msg, uint16_t msg_len);
static void handle_CS_DEV_REQ(Requester_t req_by);

const SmartplugFuncSpec_t* smartplug_func_boot_spec()
{
    static SmartplugFuncSpec_t spec = 
    {
          boot_on_enter,
          boot_on_tick,
          boot_on_NMS_message,
          boot_on_V9871_message,
          boot_on_leave
    };
    
    return &spec;
}

static void boot_on_enter(FuncParam_t param)
{
    DPRT_STAT("<boot> enter");
    boot_ = (struct BOOT_CONTEXT*)stat_shared_buffer(sizeof(*boot_));
    memset(boot_, 0, sizeof(*boot_));
    boot_->boot_tx_count = 0;
    boot_->boot_tx_tick = 0;
    boot_->boot_tx_step = 0;
    led_blink(kLED_DLINK, kTrue, 100, kTrue);
    
}

static void boot_on_tick(uint32_t tick)
{
    uint32_t diff;
    
    if(tick < BOOTUP_DELAY)
    {
          return;
    }
 
    
    diff = sys_tick_diff(tick, boot_->boot_tx_tick);
    
    if(diff >= 3000)
    {
          boot_->boot_tx_count++;
          boot_->boot_tx_tick = tick;
          DPRT("BOOT granted");
          if(boot_->boot_tx_step == 0)
          {
              handle_CS_DEV_REQ(kReqFromNMS);
          }
          else if(boot_->boot_tx_step ==  1)
          {
              nms_send_command(CS_RTC_REQ);
          }
          
    }
}

static void boot_on_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
     switch(cmd_stat)
     {
      case CS_DEV_ACK:
          nms_send_packet(CS_DEV_RDY, msg, msg_len, NULL);
          boot_->boot_tx_step++;
          break;
     case CS_RTC_ACK:
          handle_CS_RTC_CONF_ACK(kReqFromNMS, msg, msg_len);
          stat_change(kStat_RDY, null_param, kFalse, "ready confirmed");
          break;
     default:
          default_handle_rx_NMS_message(cmd_stat, msg, msg_len);
          break;
     }
}

static void boot_on_V9871_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
}

static void boot_on_leave()
{
}
static void handle_CS_RTC_CONF_ACK(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
      if(msg_len != 7)
      {
            DPRT("size=%d not match", msg_len);
            return;
      }
     
      if(CFG_DEF_MONTH >= msg[1] && CFG_DEF_DAY >= msg[3] && CFG_DEF_HOUR >= msg[4] && CFG_DEF_MIN >= msg[5] && CFG_DEF_SEC >= msg[6]) 
      {  
            UserRTC_SetTimer(msg);
            g_RtcConfig.RtcBase = *(Rtc_Parameter_t*)msg;
       }
      else
      {
           DPRT("wrong time data\r\n");        
           return;           
      }
}

static void handle_CS_DEV_REQ(Requester_t req_by)
{       
  uint8_t payload[15] = {0,};
        
        payload[0] = g_config.bc.def.dev_type;
#if ENABLE_DEBUG_NMS_TXRX
        printf("dev_type: %d\r\n", payload[0]);
#else
#endif
        memcpy(&payload[1], &g_config.bc.def.dev_id, 11);
#if ENABLE_DEBUG_NMS_TXRX
        for(int i = 1; i < 12; i++)
        {
          printf("dev_id: %c\r\n", payload[i]);
        }
#else
#endif
        
        memcpy(&payload[12], (uint8_t*)g_config.bc.def.p_ver, 2); 
        
#if ENABLE_DEBUG_NMS_TXRX
        for(int i = 12; i < 14; i++)
        {
          printf("P_ver: %d\r\n", payload[i]);
        }      
#else
#endif
        
        payload[14] = g_config.bc.def.sys_mode;
     
#if ENABLE_DEBUG_NMS_TXRX
        printf("sys_mode %d\r\n", payload[14]);
#else
#endif
        
        if(req_by == kReqFromNMS)
              nms_send_packet(CS_DEV_REQ, (uint8_t*)payload, 15, NMS_AUTO_PAYLOAD_SIZE);

}




