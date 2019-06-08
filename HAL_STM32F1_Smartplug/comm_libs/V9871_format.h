#pragma once

/* NMS2(PLC Interface) protocol format:
** Modem <--> PLC Chip(QCA 7000)
*/

#include "comm_def.h"


#define CS_READ                 0xE3
#define CS_WRITE                0xD3

#define V9871_SEL               0xC2
#define V9871_DESEL             0xC0
#define V9871_PAYLOAD           0xE3

#define V9871_LEN               0xA3

#define V9871_HADDR             0x21
#define V9871_LADDR             0x00

#define V9871_CRC               0x50

#define V9871_MAX_MTU                     128 //Max size of frame: SOF ~ CRC
#define V9871_MIN_MTU                     0

#define V9871_HEAD              0xAA

#define V9871_ACK               0xAD

#define V9871_HEADER_SIZE                 2
#define V9871_CRC_SIZE                    1

#define V9871_LARGE_BODY_SIZE   (V9871_MAX_MTU - (V9871_HEADER_SIZE + V9871_CRC_SIZE))

#define V9871_NORMAL_BODY_SIZE            (72 - (V9871_HEADER_SIZE + V9871_CRC_SIZE))      

#define V9871_AUTO_BODY_SIZE		  0

#define V9871_MIN_PACKET_SIZE             3

#define V9871_MAX_PACKET_SIZE             V9871_MAX_MTU

#pragma pack(push,1)

typedef struct
{
      uint8_t fHead;
      uint8_t ucLen;
}V9871_HEAD_t;

typedef struct
{
        V9871_HEAD_t head;
        uint8_t sz_v9871[V9871_LARGE_BODY_SIZE + 1]; 
}V9871_Packet_t;

typedef union
{
        V9871_Packet_t packed;
        uint8_t bits[V9871_MAX_PACKET_SIZE + 1];
}V9871_PacketStruct_t;

typedef struct
{
        V9871_PacketStruct_t packet;
        uint16_t packet_len;
}V9871_Result_t;

#pragma pack(pop)