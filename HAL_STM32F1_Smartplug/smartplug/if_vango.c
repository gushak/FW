/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: meter port interface
*
* History
* Date			Description
* 2018/10/05	file created
*****************************************************************************/

#include "if_vango.h"
#include "comm_dbg.h"
#include "smartplug_hw.h"
#include "smartplug_led.h"

struct IF_VANGO_CONTEXT
{
        uint32_t last_tx_tck;
};

void init_vango()
{
        uint8_t packet[4] = {0,};
        packet[0] = V9871_HEAD;
        packet[1] = 0x04;
        packet[2] = 0xC0;
        packet[3] = 0x92;
        
        vango_send_packet(packet, sizeof(packet));
}

void init_vango1()
{
        uint8_t packet[4] = {0,};
        packet[0] = V9871_HEAD;
        packet[1] = 0x04;
        packet[2] = 0xC1;
        packet[3] = 0x91;
        
        vango_send_packet(packet, sizeof(packet));
}

void init_vango2()
{
        uint8_t packet[4] = {0,};
        packet[0] = V9871_HEAD;
        packet[1] = 0x04;
        packet[2] = 0xC2;
        packet[3] = 0x90;
        
        vango_send_packet(packet, sizeof(packet));
}

void init_vango3()
{
        uint8_t packet[4] = {0,};
        packet[0] = V9871_HEAD;
        packet[1] = 0x04;
        packet[2] = 0xC3;
        packet[3] = 0x89;
        
        vango_send_packet(packet, sizeof(packet));
}

void vango_send_packet(const uint8_t *packet, uint16_t packet_len)
{
        uart_write(VANGO_UART_PORT, packet, packet_len);
        uart_wait_for_flush(VANGO_UART_PORT);
#if UART4_INTERRUPT_TX
        hw_safe_delay(3);
#endif
}