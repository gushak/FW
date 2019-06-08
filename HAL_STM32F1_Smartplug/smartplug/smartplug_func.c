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

#include "smartplug_func.h"
#include "comm_dbg.h"
#include "smartplug_hw.h"
#include "smartplug_stat.h"
#include "smartplug_impl.h"
#include "if_nms.h"
#include "NMS_proto.h"
#include "SM_V9871.h"
#include "smartplug_led.h"
#include "smartplug_it.h"

Relay_Parameter_t* req;
bool_t g_relay_is_on = 0;

static bool_t handle_command(Requester_t req_by, uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{
      bool_t rc = kTrue;
      
      switch(cmd_stat)
      {
      case CS_DEV_INFO_REQ:
            handle_CS_DEV_INFO_REQ(req_by, msg, msg_len);
            break;
      case CS_DEV_CONF_REQ:
            handle_CS_DEV_CONF_REQ(req_by, msg, msg_len);
            break;
      case CS_CTRL_REQ:
            handle_CS_CTRL_REQ(req_by, msg, msg_len);
            break;
      case CS_RELAY_STATUS_REQ:
            handle_CS_RELAY_STATUS_REQ(req_by, msg, msg_len);
            break;
      case CS_RCTRL_REQ:
            handle_CS_RCTRL_REQ(req_by, msg, msg_len);
            break;
      case CS_RTC_ACK: 
            handle_CS_RTC_ACK(req_by, msg, msg_len);
            break;
      default:
            rc = kFalse;
            break;
      }
      
      return rc;
}

void default_handle_rx_NMS_message(uint16_t cmd_stat, uint8_t* msg, uint16_t msg_len)
{       
      if(!handle_command(kReqFromNMS, cmd_stat, (uint8_t*)msg, msg_len))
      {
        KEPCO_3032_RES_t res = { kERR_SmartplugBusy };
        
        DOUT("CS=%04x not supported on '%s' mode", cmd_stat, stat_get_name());        
        nms_send_packet(CS_ERROR, (uint8_t*)&res, sizeof(res), NMS_AUTO_PAYLOAD_SIZE);   
      }
}

void handle_CS_DEV_INFO_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
      if(msg_len != 3)
      {
          DPRT("size=%d not match", msg_len);
          return;        
      }
      
      uint8_t payload[3] = {0,};
  
      payload[0] = g_config.bc.def.sys_mode;
      payload[1] = g_config.bc.def.alive_period;
      payload[2] = g_config.bc.def.reg_period;
      
      if(req_by == kReqFromNMS)
      {
          nms_send_packet(CS_DEV_INFO_ACK, (uint8_t*)payload, sizeof(payload), NMS_NORMAL_PAYLOAD_SIZE);
      }
      
      
}

void handle_CS_DEV_CONF_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
      uint8_t payload[14] = {0,};
      
      if(msg_len != 14)
      {
          DPRT("size=%d not match", msg_len);
          return;
      }
      
      KEPCO_5438_REQ_t* req = (KEPCO_5438_REQ_t*)msg;
      
      g_config.bc.def.sys_mode = req->SYSMODE;
      
      if(req->ALIVE_P < CFG_MIN_ALIVE_PERIOD && req->ALIVE_P > CFG_MIN_ALIVE_PERIOD)
      {
          DPRT("ALIVE_P_I range: %d\r\n", val);
      }
      else
          g_config.bc.def.alive_period = req->ALIVE_P;
      
      if(req->REG_P < CFG_MIN_REG_PERIOD && req->REG_P > CFG_MIN_REG_PERIOD)
      {
          DPRT("REG_P_I range: %d\r\n", val);
      }
      else 
          g_config.bc.def.reg_period = req->REG_P;
      
      if(sizeof(req->DEV_ID) != 11)
      {
          DPRT("DEV_ID SIZE: %s\r\n", req->DEV_ID);
      }
      else
          memcpy(&g_config.bc.def.dev_id, &(req->DEV_ID), sizeof(req->DEV_ID)); 
       
      g_FlashInfo.AlivePeriod = g_config.bc.def.alive_period;
      g_FlashInfo.RegiPeriod = g_config.bc.def.reg_period;
      memcpy(&g_FlashInfo.szDevid[0], &g_config.bc.def.dev_id[0], sizeof(g_config.bc.def.dev_id));
      
      //flash_write(FLASH_USER_START_ADDR, (uint32_t*)&g_FlashInfo, sizeof(g_FlashInfo));

      memcpy(payload, req, sizeof(req));

      if(req_by == kReqFromNMS)
      {
          nms_send_packet(CS_DEV_CONF_ACK, (uint8_t*)payload, sizeof(payload), NMS_NORMAL_PAYLOAD_SIZE);
      }
}

void handle_CS_VANGO_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
}

void handle_CS_CTRL_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
    uint8_t payload[13] = {0,};
    
    RELAY_RES_t res = { RES_SUCCESS };
    
    if(msg_len != 1)
     {
          DPRT("size=%d not match", msg_len);        
          return;
     }
     
     req = (Relay_Parameter_t*)msg;
     
     if(req->relay_stat == kTrue)
     {     
          hw_mcu_high_onoff(kTrue);
          hw_mcu_low_onoff(kFalse);
          hw_led_rg_onoff(kTrue);
          g_RelayInfo.relay_stat = 1;
     }
     else if(req->relay_stat == kFalse)
     {
          hw_mcu_high_onoff(kFalse);
          hw_mcu_low_onoff(kTrue);
          hw_led_rg_onoff(kFalse);
          g_RelayInfo.relay_stat = 0;
     }
     else
     {
          res.status  = RES_FAIL;
     }
    
     payload[0] = res.status;
     memcpy(&payload[1], &g_config.bc.def.dev_id, 11); 

     if(req_by == kReqFromNMS)
     {
          nms_send_packet(CS_CTRL_ACK, (uint8_t*)payload, 12, NMS_AUTO_PAYLOAD_SIZE);
     }
}

void handle_CS_RELAY_STATUS_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
      if(msg_len != 1)
      {
            DPRT("size=%d not match", msg_len);        
            return;
      }
 
      uint8_t payload[12] = {0,};  

      memcpy(&payload[0], &g_RelayInfo.relay_stat, sizeof(g_RelayInfo.relay_stat));
#if ENABLE_DEBUG_NMS_TXRX 
      printf("relay_stat]%02x\r\n", g_RelayInfo.relay_stat);
#endif
      memcpy(&payload[1], &g_config.bc.def.dev_id, 11);
        
#if ENABLE_DEBUG_NMS_TXRX        
      for(uint8_t i = 1; i < sizeof(g_config.bc.def.dev_id); i++)
      {
            printf("payload]%02x\r\n", payload[i]);
      }
#endif
      if(req_by == kReqFromNMS)
            nms_send_packet(CS_RELAY_STATUS_ACK, (uint8_t*)payload, sizeof(payload), NMS_AUTO_PAYLOAD_SIZE);

}

void handle_CS_RCTRL_REQ(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
{
      
      if(msg_len != 8)
      {
            DPRT("size=%d not match", msg_len);
            return;
      }
      
      if(CFG_DEF_DAY >= msg[3] && CFG_DEF_HOUR >= msg[4] && CFG_DEF_MIN >= msg[5] && CFG_DEF_SEC >= msg[6]) 
      {
            g_RtcConfig.RtcAct.year = msg[0];
            g_RtcConfig.RtcAct.month = msg[1];
            g_RtcConfig.RtcAct.weekday = msg[2];
            g_RtcConfig.RtcAct.day = msg[3];
            g_RtcConfig.RtcAct.hour = msg[4];
            g_RtcConfig.RtcAct.min = msg[5];
            g_RtcConfig.RtcAct.sec = msg[6];
            g_relay_is_on = msg[7];        
            
            if(req_by == kReqFromNMS)
            {
                  nms_send_command(CS_RCTRL_ACK); 
            }
      }
      else
      {
           DPRT("wrong time data\r\n");        
           return;       
      }      
}


void handle_CS_RTC_ACK(Requester_t req_by, uint8_t* msg, uint16_t msg_len)
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
         
