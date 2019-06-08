#include "SM_NMS.h"
#include <stdlib.h>
#include <assert.h>

#define ENABLE_PRINT 0

#define GET_PACKET_FRAME_SIZE(packed) ((packed.head.len) + 7) // 7 = SOF,VER(2),CTL,EOF,CRC(2)

void sm_nms_init(NMS_StateMachine_t* sm)
{               
        cirq_init(&sm->que, sm->que_buf, sizeof(sm->que_buf));
        sm_nms_reset(sm);
        sm_nms_reset_result(&sm->result_frame);
}

void sm_nms_reset(NMS_StateMachine_t* sm)
{
        sm->payload_len = 0;
        sm->cnt_in_stat = 0;
        sm->next_stat = kNMS_SOF;
        
#if ENABLE_PRINT
        if (cirq_elem_count(&sm->que) > 0){
		printf("NMS2: RESET %d"CRLF_STR, cirq_elem_count(&sm->que));
                while(!cirq_is_empty(&sm->que))
                {
                        unBuff[index] = cirq_direct_dequeue(&sm->que);
                        printf("gabage] %02x"CRLF_STR, unBuff[index]);
                        index++;
                }
	}
#endif
        cirq_init(&sm->que, sm->que_buf, sizeof(sm->que_buf));
}

void sm_nms_input(NMS_StateMachine_t* sm, uint8_t input)
{      
        
        switch (sm->next_stat)
        {
        case kNMS_SOF:
             if (input != NMS_SOF_CHAR)
             {
#if ENABLE_PRINT
              printf("NMS: SOF %02x"CRLF_STR, input);
#endif                         
              return;
             }   
             sm->next_stat = kNMS_VER;
             break;
        case kNMS_VER:
              if (input == NMS_SOF_CHAR)
               {
                    //* IMPORTANT *
                    // Sometimes QCA 7000 send E2(SOF) char twice,
                    // it should be ignored.
#if ENABLE_PRINT
                    printf("*QFIX*"CRLF_STR);
#endif
                cirq_init(&sm->que, sm->que_buf, sizeof(sm->que_buf));
                break;
               }
       
               if (++sm->cnt_in_stat == 2)
               {
                  sm->next_stat = kNMS_CTL;
                  sm->cnt_in_stat = 0;
                }   
              break;
        case kNMS_CTL:
                sm->next_stat = kNMS_LEN;
                sm->result_frame.packet.packed.head.ctl.fsn = (input>>4 & 0x0F);
                sm->result_frame.packet.packed.head.ctl.type = (input>>2 & 0x03);
                sm->result_frame.packet.packed.head.ctl.reserved = (input & 0x03);
                break;
        case kNMS_LEN:
               if (input < NMS_EXTRA_SIZE_IN_LEN)
               {
#if ENABLE_PRINT
                  printf("NMS: LEN %d"CRLF_STR, input);
#endif
                  sm_nms_reset(sm); 
                  return;
               }
                    
                  sm->payload_len = input - NMS_EXTRA_SIZE_IN_LEN;
                  sm->next_stat = kNMS_SN;
                if (sm->payload_len > NMS_LARGE_PAYLOAD_SIZE)
                {
#if ENABLE_PRINT
                  printf("NMS2: payload %d", input);
#endif  
                  sm_nms_reset(sm);
                  return;
                }
                break;
         case kNMS_SN:
                if (++sm->cnt_in_stat == 2)
                {                
                    sm->next_stat = kNMS_CMD;
                }
                break;
        case kNMS_CMD:
                sm->next_stat = kNMS_STA;
                break;
        case kNMS_STA:
                if (sm->payload_len == 0)
                        sm->next_stat = kNMS_CRC;
                else
                        sm->next_stat = kNMS_PAY;
                sm->cnt_in_stat = 0;
                break;
        case kNMS_PAY:
                if (++sm->cnt_in_stat == sm->payload_len)
                {
                        sm->next_stat = kNMS_CRC;
                        sm->cnt_in_stat = 0;
                }
                break;
        case kNMS_CRC:
                if (++sm->cnt_in_stat == 2)
                {
                        sm->next_stat = kNMS_EOF;
                }
                break;
        case kNMS_EOF:
                        sm->next_stat = kNMS_SOF;
        default:
                break;
        }

        if (cirq_enque(&sm->que, input) != CIRQ_SUCCESS)
        {
                sm_nms_reset(sm);
        }
        else if (sm->next_stat == kNMS_SOF)
        {
                sm_nms_reset_result(&sm->result_frame);
                while (!cirq_is_empty(&sm->que))
                {
                        sm->result_frame.packet.bits[sm->result_frame.packet_len] = cirq_direct_dequeue(&sm->que);
#if ENABLE_PRINT
                        printf("packet: payload %02x"CRLF_STR, sm->result_frame.packet.bits[sm->result_frame.packet_len]);
#endif
                        sm->result_frame.packet_len++;
                }
                

#if ENABLE_PRINT
                //chexdump("FromNMS2", sm->result_frame.packet.bits, sm->result_frame.packet_len);
#endif
        }
}

void sm_nms_reset_result(NMS_Result_t* result)
{
        result->packet_len = 0;
        result->packet.packed.head.len = 0;
}

int sm_nms_is_completed(const NMS_Result_t* result)
{
        return result->packet_len >= NMS_MIN_PACKET_SIZE &&
                result->packet_len == GET_PACKET_FRAME_SIZE(result->packet.packed);
}        

