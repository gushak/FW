#pragma once

/* State machin of NMS protocol;
** Mode <--> PLC Chip(QCA 7000)
*/

#include "comm_def.h"
#include "circularq.h"
#include "NMS_format.h"

typedef enum
{
        kNMS_SOF = 0,
        kNMS_VER,
        kNMS_CTL,
	kNMS_LEN,
	kNMS_SN,
	kNMS_CMD,
	kNMS_STA,
	kNMS_PAY,
	kNMS_CRC,
        kNMS_EOF 
}NMS_State_t;

typedef struct
{
        NMS_State_t next_stat;
        uint8_t fsnIndex;
        uint8_t payload_len;
        uint8_t cnt_in_stat;    //counter in a state
        uint8_t que_buf[NMS_MAX_PACKET_SIZE + 1];
        CirQ_t que;
        NMS_Result_t result_frame;
}NMS_StateMachine_t;

void sm_nms_init(NMS_StateMachine_t* sm);
void sm_nms_reset(NMS_StateMachine_t* sm);
void sm_nms_input(NMS_StateMachine_t* sm, uint8_t input);
void sm_nms_reset_result(NMS_Result_t* result);
int sm_nms_is_completed(const NMS_Result_t* result);