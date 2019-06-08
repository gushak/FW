#pragma once

/* State machin of V9871 protocol;
** Mode <--> PLC Chip(V9871)
*/

#include "comm_def.h"
#include "circularq.h"
#include "V9871_format.h"

typedef enum
{
        kV9871_HEAD = 0,
	kV9871_LEN,
	kV9871_BODY,
	kV9871_CRC 
}V9871_State_t;

typedef struct
{
        V9871_State_t next_stat;
        uint8_t payload_len;
        uint8_t cnt_in_stat;    //counter in a state
        uint8_t que_buf[V9871_MAX_PACKET_SIZE + 1];
        CirQ_t que;
        V9871_Result_t result_frame;
}V9871_StateMachine_t;

void sm_v9871_init(V9871_StateMachine_t* sm);
void sm_v9871_reset(V9871_StateMachine_t* sm);
void sm_v9871_input(V9871_StateMachine_t* sm, uint8_t input);
void sm_v9871_reset_result(V9871_Result_t* result);
int sm_v9871_is_completed(const V9871_Result_t* result);
