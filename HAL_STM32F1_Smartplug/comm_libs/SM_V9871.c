
#include <stdlib.h>
#include <assert.h>
#include "SM_V9871.h"
#include "crc16.h"

#define GET_V9871_FRAME_SIZE(packed) ((packed.head.ucLen) + 1)

void sm_v9871_init(V9871_StateMachine_t* sm)
{
        cirq_init(&sm->que, sm->que_buf, sizeof(sm->que_buf));
        sm_v9871_reset(sm);
        sm_v9871_reset_result(&sm->result_frame);
}

void sm_v9871_reset(V9871_StateMachine_t* sm)
{
        sm->cnt_in_stat = 0;
        sm->next_stat = kV9871_HEAD;
        

#if ENABLE_PRINT
        if (cirq_elem_count(&sm->que) > 0) {
                printf("V9871: RST %d"CRLF_STR, cirq_elem_count(&sm->que));
        }
#endif
        
        cirq_clear(&sm->que);
}

void sm_v9871_input(V9871_StateMachine_t* sm, uint8_t input)
{
        switch (sm->next_stat)
        {
        case kV9871_HEAD:
                if (input != V9871_HEAD)
                {
#if ENABLE_PRINT
                        printf("V9871: HEAD %02x"CRLF_STR, input);
#endif                
                        return;
                }
                else if(input == V9871_ACK)
                {
#if ENABLE_PRINT
                        printf("V9871: init %02x"CRLF_STR, input);
#endif                
                        return;
                }
                sm->next_stat = kV9871_LEN;
                break;
        
          case kV9871_LEN:
                if (input < V9871_MIN_PACKET_SIZE)
                {
#if ENABLE_PRINT
                        printf("V9871: LEN %02x"CRLF_STR, input);
#endif                
                        sm_v9871_reset(sm);
                        return;                
                }              
                sm->cnt_in_stat = 0; 
                sm->next_stat = kV9871_BODY;
                sm->result_frame.packet.packed.head.ucLen = input;
                if (input > V9871_LARGE_BODY_SIZE)
                {
#if ENABLE_PRINT
                        printf("V9871: BODY %02x"CRLF_STR, input);
#endif                
                        sm_v9871_reset(sm);
                        return;
                }
                break;
            
          case kV9871_BODY:
                if(++sm->cnt_in_stat == sm->result_frame.packet.packed.head.ucLen )
                {
                sm->next_stat = kV9871_CRC;
                }
                break;
          
          case kV9871_CRC:
          default:
                sm->next_stat = kV9871_HEAD;
                break;
        }
          
          if (cirq_enque(&sm->que, input) != CIRQ_SUCCESS)
          {
                  printf("V9871: Q full"CRLF_STR);
                  sm_v9871_reset(sm);
          }
          else if (sm->next_stat == kV9871_HEAD)
          {
                  sm_v9871_reset_result(&sm->result_frame);
                  while(!cirq_is_empty(&sm->que))
                  {
                          sm->result_frame.packet.bits[sm->result_frame.packet_len] = cirq_direct_dequeue(&sm->que);
                          sm->result_frame.packet_len++;
                  }
          }
        
}

void sm_v9871_reset_result(V9871_Result_t* result)
{
          result->packet_len = 0;
          result->packet.packed.head.ucLen = 0;
}

int sm_v9871_is_completed(const V9871_Result_t* result)
{
          return (result->packet_len == GET_V9871_FRAME_SIZE(result->packet.packed));
                  //return (result->packet_len >= V9871_MIN_PACKET_SIZE); 
                  //&& (result->packet_len == GET_V9871_FRAME_SIZE(result->packet.packed)));
}
