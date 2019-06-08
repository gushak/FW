#pragma once

#include "comm_def.h"
#include "NMS_format.h"
#include "circularq.h"

typedef enum
{
    kNMS_Empty,
    kNMS_SinglePacket,
    kNMS_MultiPacker
}NMS_BufStat_t;

struct NMS_PROTO_BUF
{
    NMS_BufStat_t buf_stat;
    uint16_t cmd_stat;
    uint8_t remain_buf[NMS_PROTO_MAX_PAYLOAD +1];
    CirQ_t remain_que;
};

void nms_proto_init(struct NMS_PROTO_BUF* pbuf);

// return true if protocol buffer is empty
NMS_BufStat_t nms_proto_get_stat(struct NMS_PROTO_BUF* pbuf);

// set command stat without payload
void nms_proto_set_cmd_stat(struct NMS_PROTO_BUF* pbuf, uint16_t cmd_stat);

// set command stat with payload
void nms_proto_set_payload(struct NMS_PROTO_BUF* pbuf, uint16_t cmd_stat
      ,const void* payload, uint16_t paylaod_size, uint16_t payload_limit);
      
// get packet from buffer
bool_t nms_proto_get_packet(struct NMS_PROTO_BUF* ppbuf, NMS_Result_t* result, uint16_t payload_limit);

// clear previous data in the proto buffer
void nms_proto_clear(struct NMS_PROTO_BUF* pbuf);

// configure the sn to use the received sn
void nms_proto_set_sn(uint16_t sn);

void nms_proto_inc_sn();
