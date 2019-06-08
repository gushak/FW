#pragma once

/* NMS2(PLC Interface) protocol format:
** Modem <--> PLC Chip(QCA 7000)
*/

#include "comm_def.h"

#define CS_ERROR                                        0x6010

#define CS_DEV_REQ				0x1030
#define CS_DEV_ACK				0x1031
#define CS_DEV_RDY                              0x1032

/* Device information reg,req,conf macro space
*/
#define CS_DEV_INFO_REQ                         0x2030
#define CS_DEV_INFO_ACK                         0x2031
#define CS_DEV_CONF_REQ                         0x2032
#define CS_DEV_CONF_ACK                         0x2033
#define CS_DEV_LIST_REQ                        0x2034
#define CS_DEV_LIST_ACK                        0x2035

/* metering req,ack macro space
*/
#define CS_VANGO_REQ                            0x3010                            
#define CS_VANGO_ACK                            0x3011

/*control req,ack macro space
*/
#define CS_CTRL_REQ                             0x4010
#define CS_CTRL_ACK                             0x4011
#define CS_RELAY_STATUS_REQ                     0x4012
#define CS_RELAY_STATUS_ACK                     0x4013

#define CS_RTC_REQ                              0x5010
#define CS_RTC_ACK                              0x5011
#define CS_RCTRL_REQ                             0x5012
#define CS_RCTRL_ACK                             0x5013

/* SmartPlug BOOTING MACRO
*/
#define CS_BOOT_RES                   0x5010

/*Alive Check,ack macro space
*/
#define CS_ALIVE_REQ                            0xA030
#define CS_ALIVE_ACK                            0xA031

#define NMS_SOF_CHAR			0xE2
#define NMS_EOF_CHAR                    0xE4

#define NMS_VER                                 0x0001

#define NMS_MAX_MTU                     255 //Max size of frame: SOF ~ CRC
#define NMS_MIN_MTU                     1

#define NMS_HEADER_SIZE			        9// sizeof(HEAD_NMS)
#define NMS_CRC_SIZE				2

#define NMS_LARGE_PAYLOAD_SIZE (NMS_MAX_MTU - (NMS_HEADER_SIZE + NMS_CRC_SIZE))         // 242

#define NMS_NORMAL_PAYLOAD_SIZE         (72 - (NMS_HEADER_SIZE + NMS_CRC_SIZE))        

#define NMS_AUTO_PAYLOAD_SIZE		0

#define NMS_MIN_PACKET_SIZE             (NMS_HEADER_SIZE + NMS_CRC_SIZE)

#define NMS_MAX_PACKET_SIZE             NMS_MAX_MTU

#define NMS_PROTO_MAX_PAYLOAD           300

//need to add NMS_EXTRA_SIZE_IN_LEN
#define NMS_EXTRA_SIZE_IN_LEN           5 //  SN(2) + CMD(1) + STA(1)    

#pragma pack(push,1)

typedef struct 
{
        uint8_t fsn : 4;		//0000 : last or Single packet   
        uint8_t type : 2;               //01(1): Fragment Packet
        uint8_t reserved :2;            //need to add commnet
}NMS_CTL_t;

typedef struct 
{
        uint8_t sof;
        uint16_t ver;
        NMS_CTL_t ctl;
        uint8_t len;                    // SN ~ PAYLOAD ; Length of this packet        
        uint8_t SN[2];
        uint8_t cmd;
        uint8_t stat;
}NMS_HEAD_t;

typedef struct
{
        NMS_HEAD_t head;
        uint8_t sz_nms[NMS_LARGE_PAYLOAD_SIZE]; 
}NMS_Packet_t;

typedef union
{
        NMS_Packet_t packed;
        uint8_t bits[NMS_MAX_PACKET_SIZE];
}NMS_PacketStruct_t;

typedef struct
{
        NMS_PacketStruct_t packet;
        uint16_t packet_len;
}NMS_Result_t;

#pragma pack(pop)

const char* nms_CS_name(uint16_t cmd_stat);

