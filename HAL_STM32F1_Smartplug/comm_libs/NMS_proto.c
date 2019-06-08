#include "NMS_proto.h"
#include <stdlib.h>
#include <assert.h>
#include "circularq.h"
#include "crc16.h"
#include "comm_dbg.h"

uint16_t nms_sn_ = 0;

static void make_single_packet_header(struct NMS_PROTO_BUF* pbuf, NMS_HEAD_t* head)
{
        uint16_t data_len = (uint16_t)cirq_elem_count(&pbuf->remain_que);
        
        assert(pbuf->buf_stat == kNMS_SinglePacket);
        
        head->sof = NMS_SOF_CHAR;
        
        head->ver = NMS_VER;

#if ENABLE_DEBUG_NMS_TXRX
        printf("%02x %04x \r\n", head->sof, head->ver);
#else
#endif

        head->ctl.fsn = 0;
        head->ctl.type = 0;
        head->ctl.reserved = 0;
        
        head->len = NMS_EXTRA_SIZE_IN_LEN + data_len;
#if ENABLE_DEBUG_NMS_TXRX
        printf("%02x %02x %02x %02x \r\n", head->ctl.fsn, head->ctl.type, head->ctl.reserved, head->len);
#else
#endif

        head->SN[0] = 0x00;
        head->SN[1] = 0x00;
#if ENABLE_DEBUG_NMS_TXRX
        printf("%02x %02x \r\n", head->SN[0],  head->SN[1]);
#else
#endif
        
        head->cmd = (pbuf->cmd_stat >> 8) & 0xFF;
        head->stat = pbuf->cmd_stat & 0xFF;
#if ENABLE_DEBUG_NMS_TXRX
        printf("%02x %02x \r\n", head->cmd,  head->stat);
#else
#endif
}       

void nms_proto_init(struct NMS_PROTO_BUF* pbuf)
{
        pbuf->buf_stat = kNMS_Empty;
        cirq_init(&pbuf->remain_que, pbuf->remain_buf, sizeof(pbuf->remain_buf));
}

NMS_BufStat_t nms_proto_get_stat(struct NMS_PROTO_BUF* pbuf)
{
	return pbuf->buf_stat;
}

void nms_proto_set_cmd_stat(struct NMS_PROTO_BUF* pbuf, uint16_t cmd_stat)
{
        nms_proto_set_payload(pbuf, cmd_stat, NULL, 0 , NMS_AUTO_PAYLOAD_SIZE);
}

void nms_proto_set_payload(struct NMS_PROTO_BUF* pbuf
       , uint16_t cmd_stat, const void* payload, uint16_t payload_size, uint16_t payload_limit)
{
        const uint8_t* p = (const uint8_t*)payload;

#if ENABLE_DEBUG_NMS_TXRX
        printf("payload size: %02x\r\n", payload_size);
#else
#endif        
        if (!cirq_is_empty(&pbuf->remain_que))
        {
#if ENABLE_DEBUG_NMS_TXRX
		printf("<nms_enc_set_payload> has previous data: %d"CRLF_STR, cirq_elem_count(&pbuf->remain_que));
#else
#endif
		
                
		cirq_clear(&pbuf->remain_que);
        }
        
        pbuf->cmd_stat = cmd_stat;
        while (payload_size > 0)
        {
                payload_size--;
                if (cirq_enque(&pbuf->remain_que, *p++) == CIRQ_ERROR)
                {
#if ENABLE_DEBUG_NMS_TXRX
			printf("<nms_enc_set_payload> queue error: %d"CRLF_STR, cirq_elem_count(&pbuf->remain_que));
#else
#endif                  
                }
        }
        
        pbuf->buf_stat = kNMS_SinglePacket;
}

bool_t nms_proto_get_packet(struct NMS_PROTO_BUF* pbuf, NMS_Result_t* result, uint16_t payload_limit)
{
        uint8_t idx, payload_len;
        
        assert(pbuf->buf_stat != kNMS_Empty);
        if (pbuf->buf_stat == kNMS_Empty)
                return kFalse;
                
        make_single_packet_header(pbuf, &result->packet.packed.head);
        
        payload_len = result->packet.packed.head.len - NMS_EXTRA_SIZE_IN_LEN;
#if ENABLE_DEBUG_NMS_TXRX
        printf("payload_len]%d\r\n",payload_len);
#else
#endif              
        
        for (idx = 0; idx < payload_len; ++idx)
        {
                assert(!cirq_is_empty(&pbuf->remain_que));
                result->packet.packed.sz_nms[idx] = cirq_direct_dequeue(&pbuf->remain_que);
        }
        
        result->packet_len = NMS_HEADER_SIZE + payload_len;
#if ENABLE_DEBUG_NMS_TXRX
        printf("packet_len]%d\r\n",result->packet_len);
#else
#endif            
        
        append_crc16(&result->packet.bits[0], result->packet_len);
        result->packet_len += 2;
        result->packet.bits[result->packet_len] = NMS_EOF_CHAR;
        result->packet_len += 1;
        return kTrue;
        
}

void nms_proto_clear(struct NMS_PROTO_BUF* pbuf)
{
        cirq_clear(&pbuf->remain_que);
        pbuf->buf_stat = kNMS_Empty;
}

void nms_proto_se_sn(uint16_t sn)
{
        nms_sn_ = sn;
}

void nms_proto_inc_sn()
{
        nms_sn_++;
}


