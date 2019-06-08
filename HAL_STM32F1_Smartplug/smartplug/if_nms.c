#include "if_nms.h"
#include "comm_dbg.h"
#include "NMS_proto.h"
#include "smartplug_hw.h"
#include "comm_uart.h"
#include "comm_str.h"

struct IF_NMS_CONTEXT
{
      struct NMS_PROTO_BUF proto_buf;
      uint32_t last_tx_tick;
      uint16_t payload_limit;
      uint16_t mtu_payload_size;
};

static struct IF_NMS_CONTEXT if_nms_;

void nms_interface_init()
{
     nms_proto_init(&if_nms_.proto_buf);
     if_nms_.mtu_payload_size = NMS_NORMAL_PAYLOAD_SIZE;
}

bool_t nms_set_mtu(uint16_t mtu)
{
      if(mtu >= NMS_MIN_MTU && mtu <= NMS_MAX_MTU)
      {
            if_nms_.mtu_payload_size = mtu - (NMS_HEADER_SIZE + NMS_CRC_SIZE);
            return kTrue;
      }
      else
      {
            DOUT("invalid mtu: %d", mtu);
            return kFalse;
      }
}

void nms_send_command(uint16_t cmd_stat)
{
     nms_send_packet(cmd_stat, NULL, 0, NMS_AUTO_PAYLOAD_SIZE);
}

void nms_send_packet(uint16_t cmd_stat, const uint8_t* payload, uint16_t payload_len, uint16_t payload_limit)
{
     NMS_Result_t packet;
     
#if ENABLE_DEBUG_NMS_TXRX
     printf("payload_len: %d\r\n", payload_len);
#else
#endif     
     
     if(payload_limit == NMS_AUTO_PAYLOAD_SIZE)
     {
          payload_limit = if_nms_.mtu_payload_size;
     }
     
     nms_clear_tx_packet();     
     
     nms_proto_set_payload(&if_nms_.proto_buf, cmd_stat, payload, payload_len, payload_limit);
     nms_proto_get_packet(&if_nms_.proto_buf, &packet, payload_limit);
     uart_write(NMS_UART_PORT, packet.packet.bits, packet.packet_len);
 
     uart_wait_for_flush(NMS_UART_PORT);
     hw_safe_delay(1);
     
     if_nms_.last_tx_tick = sys_tick_count();
     if_nms_.payload_limit = payload_limit;
}

bool_t nms_has_tx_packet()
{
      return (nms_proto_get_stat(&if_nms_.proto_buf)!=kNMS_Empty)?kTrue : kFalse;
}

bool_t nms_send_tx_packet()
{
       NMS_Result_t packet;
       
       if(!nms_has_tx_packet())
       {
            DOUT("*ERR* nothing to send");
            return kFalse;
       }
       
       nms_proto_get_packet(&if_nms_.proto_buf, &packet, if_nms_.payload_limit);
       uart_write(NMS_UART_PORT, packet.packet.bits, packet.packet_len);
       uart_wait_for_flush(NMS_UART_PORT);
       if_nms_.last_tx_tick = sys_tick_count();

       return kTrue;
}

void nms_clear_tx_packet()
{
      if (nms_proto_get_stat(&if_nms_.proto_buf) != kNMS_Empty)
      {
            //DOUT("*warning*ignore payload in torocol buffer");
            nms_proto_clear(&if_nms_.proto_buf);
      }
}
