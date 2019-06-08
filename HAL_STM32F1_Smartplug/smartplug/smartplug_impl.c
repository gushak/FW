/*****************************************************************************
* Copyright(C) 2018 Signal Information & Communication Corp.
* All right reserved.
*
* Description	: modem implementation
*
* History
* Date			Description
* 2018/09/14	file created
*****************************************************************************/
#include "smartplug_impl.h"
#include "smartplug_func.h"
#include "smartplug_cfg.h"
#include "comm_dbg.h"
#include "smartplug_hw.h"
#include "smartplug_stat.h"
#include "smartplug_led.h"
#include "SM_NMS.h"
#include "SM_V9871.h"
#include "NMS_proto.h"
#include "crc16.h"
#include "checksum.h"

#define OLD_DATA_DISCARD_TICK 500

NMS_StateMachine_t nms_rx_sm;
V9871_StateMachine_t v9871_rx_sm;

uint32_t usart2_last_rx_tick = 0;
uint32_t usart4_last_rx_tick = 0;

#pragma pack(push,1)

struct IMPL_CONTEXT
{
      uint8_t ry_gpio;
      uint8_t unButtonState;
      uint32_t gpio_set_tick;
      uint32_t alive_timeout_sec;
};

#pragma pack(pop)

static struct IMPL_CONTEXT impl_ ;

static void check_gpio_inputs();
static void impl_clear_gabage_input();
static void check_rx_from_NMS();
static void check_rx_from_v9871();
static void impl_check_NMS_packet(Func_OnNMS_message_t nms_rx_func);
static void impl_check_v9871_packet(Func_OnV9871_message_t v9871_rx_func);

static void impl_check_NMS_packet(Func_OnNMS_message_t nms_rx_func)
{

      uint16_t* packet_crc;
      uint16_t computed_crc;
      uint16_t payload_end;
        
       //check RX packet from NMS server(PLC chip)
      if(sm_nms_is_completed(&nms_rx_sm.result_frame))
      {
          uint16_t cmd_stat;
          uint8_t* msg;
          uint16_t msg_len;
            
          cmd_stat = (nms_rx_sm.result_frame.packet.packed.head.cmd << 8) | nms_rx_sm.result_frame.packet.packed.head.stat;
          
          payload_end = nms_rx_sm.result_frame.packet_len - 3;
          packet_crc = (uint16_t*)&nms_rx_sm.result_frame.packet.bits[payload_end];
         
          uint8_t payload[] = {0,};
          memcpy(payload, &nms_rx_sm.result_frame.packet.bits[3], payload_end-3);
          computed_crc =  make_crc16(payload, payload_end-3);
                   
          uint8_t *temp = (uint8_t*)&computed_crc;
          
          if(temp[1] != (uint8_t)packet_crc[0] && temp[0] != (uint8_t)packet_crc[1])
          {
            KEPCO_3032_RES_t res = { kERR_CRC_Error};
            
            sm_nms_reset_result(&nms_rx_sm.result_frame);
            nms_send_packet(CS_ERROR, (uint8_t*)&res, sizeof(res), NMS_AUTO_PAYLOAD_SIZE);
            return;
          }
          
          if(nms_rx_sm.result_frame.packet.packed.head.ctl.type == 1)
          {
              DOUT("fragment packet not supported");
          }
          else
          {
              msg = nms_rx_sm.result_frame.packet.packed.sz_nms;
              msg_len = nms_rx_sm.result_frame.packet.packed.head.len - NMS_EXTRA_SIZE_IN_LEN;
              nms_rx_func(cmd_stat, msg, msg_len);
              sm_nms_reset(&nms_rx_sm);
          }   
          sm_nms_reset_result(&nms_rx_sm.result_frame);
          impl_reset_alive_timeout();
      }
}

static void impl_check_v9871_packet(Func_OnV9871_message_t v9871_rx_func)
{
      uint8_t* packet_crc;  
      packet_crc = (uint8_t*)malloc(1);
      uint8_t computed_crc;
      uint16_t payload_end;  
//need to dump
      //if (sm_v9871_is_completed(&v9871_rx_sm.result_frame))
      if(0)
      {     
          payload_end = v9871_rx_sm.result_frame.packet_len - 4;
          packet_crc = &v9871_rx_sm.result_frame.packet.bits[payload_end];
          computed_crc = DoCheckSum(v9871_rx_sm.result_frame.packet.bits, sizeof(v9871_rx_sm.result_frame.packet.bits) - 1);     

                if (*packet_crc != computed_crc)
                {
//need to dump   
                    sm_v9871_reset_result(&v9871_rx_sm.result_frame);
                    
                    KEPCO_3032_RES_t res = { kERR_CRC_Error};
                    nms_send_packet(CS_ERROR, (uint8_t*)&res, 1, NMS_AUTO_PAYLOAD_SIZE);
                    
                    return;
                }               
                uint8_t *msg = v9871_rx_sm.result_frame.packet.packed.sz_v9871;
                uint16_t msg_len = v9871_rx_sm.result_frame.packet.packed.head.ucLen - 3;
                
                if(msg_len < 3)
                {       
                        DPRT("no v9871 data");
                }
                else
                {
                        nms_send_packet(CS_VANGO_ACK, msg, msg_len, NULL); 
                        sm_v9871_reset(&v9871_rx_sm);
                }
                sm_v9871_reset_result(&v9871_rx_sm.result_frame);
      }
      free(packet_crc);
}

static void check_rx_from_NMS()
{
    impl_check_NMS_packet(cur_smartplug_func_->nms_rx_func);
}

static void check_rx_from_v9871()
{
    impl_check_v9871_packet(cur_smartplug_func_->v9871_rx_func);
}

void impl_on_tick(uint32_t tick)
{
      DASSERT(cur_smartplug_func_ != NULL);
      cur_smartplug_func_->tick_func(tick);
      check_gpio_inputs();
      if(sys_uptime() == impl_.alive_timeout_sec)
      {
            hw_reset("ALIVE timeout");
      }
}
                     
void impl_check_rx()
{
      check_rx_from_NMS();
      check_rx_from_v9871();
}

void impl_init()
{
      stat_init();
      sm_nms_init(&nms_rx_sm);
      sm_v9871_init(&v9871_rx_sm);
      memset(&impl_, 0, sizeof(impl_));
      impl_reset_alive_timeout();
}
void impl_reset_alive_timeout()
{
     impl_.alive_timeout_sec = sys_uptime() + NMS_ALIVE_DURATION_SEC;
     DPRT("reset_alive_timeout %d", NMS2_ALIVE_DURATION_SEC);
}

void impl_on_hw_tick()
{
      uint32_t tick = sys_tick_count();
      led_on_tick(tick);
      impl_clear_gabage_input();
}

static void check_gpio_inputs()
{
      uint8_t ry_input = hw_read_ry_gpio();
      //ry_input == TRUE:pin pressed
        
      if(impl_.ry_gpio)
      {
          uint32_t diff = sys_tick_diff(sys_tick_count(),impl_.gpio_set_tick);
              
          if(ry_input)
          {
                if(diff >= RY_MCU_DURATION)
                { 
                }
                else
                { 
                }
          }
          else
          {
                 if(impl_.unButtonState == 0)
                 {
                   DPRT("ry mcu sw on");
                   hw_led_rg_onoff(kTrue);
                   hw_mcu_high_onoff(kTrue);
                   hw_mcu_low_onoff(kFalse);
                   impl_.unButtonState = 1;
                   g_RelayInfo.relay_stat = 1;
                 }
                 else
                 {
                   DPRT("pin press canceled");
                   hw_led_rg_onoff(kFalse);
                   hw_mcu_high_onoff(kFalse);
                   hw_mcu_low_onoff(kTrue);
                   impl_.unButtonState = 0;
                   g_RelayInfo.relay_stat = 0;
                 }
                 
                 impl_.ry_gpio = 0;
           }
      }
      else
      { 
          //Deteciing piun pressed from pin NOT pressed status
          
          if(ry_input)
          {
               impl_.ry_gpio = 1;
               impl_.gpio_set_tick = sys_tick_count();
          }
      }
}         

static void impl_clear_gabage_input()
{
    uint32_t cur_tick = sys_tick_count();
    if(sys_tick_diff(cur_tick, usart2_last_rx_tick) >= OLD_DATA_DISCARD_TICK){
         if(cirq_elem_count(&nms_rx_sm.que) > 0){
            DOUT("sm_nms_reset");
            sm_nms_reset(&nms_rx_sm);
         }
    }
    if(sys_tick_diff(cur_tick, usart4_last_rx_tick) >= OLD_DATA_DISCARD_TICK){
         if(cirq_elem_count(&v9871_rx_sm.que) > 0){
            DOUT("v9871_reset");
            sm_v9871_reset(&v9871_rx_sm);
         }
    }
}
                