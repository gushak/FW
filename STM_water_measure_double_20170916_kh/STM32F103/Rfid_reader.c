#include "hw_control.h"

#define RFID_BUFFER_SIZE 512

__IO uint16_t RFID_rx_buffer[RFID_BUFFER_SIZE];
uint32_t RFID_rx_point_head = 0, RFID_rx_point_tail = 0;

void init_rf_buffer(void)
{
	memset((uint8_t*)RFID_rx_buffer, 0x00, 512);
	RFID_rx_point_head = 0;
	RFID_rx_point_tail = 0;
}
void increase_RF_point_value(u32 *p)
{
	(*p)++;
	if(RFID_BUFFER_SIZE == (*p))
	{
		(*p) = 0; 
	}
}

uint8_t rfid_is_empty(void)
{
	if(RFID_rx_point_head == RFID_rx_point_tail) 
  {
		return 1; 
	}
	return 0;
}

u16 rfid_dequeue(void)
{
	u8 rev = RFID_rx_buffer[RFID_rx_point_tail];
	increase_RF_point_value(&RFID_rx_point_tail);
	return rev;
}


void rfid_enqueue(u16 data)
{
	RFID_rx_buffer[RFID_rx_point_head] = data;
	increase_RF_point_value(&RFID_rx_point_head);  
}

void parse_rfid_08IMF(uint8_t c)
{
	static uint8_t rf_status2 = 0;
	static uint8_t rf_buf2[8];
	static uint8_t rf_buf2_cnt = 0;
	uint32_t value = 0;
		
	switch(rf_status2)
	{
		case 0:                                                                                                    
			if(c == 0x02) 
      {	
				rf_status2 = 1;
				rf_buf2_cnt = 0;
			}
			break;
			
		case 1:
			if(c == 0x03)
      {	
				//버퍼가 여러개 일경우 
				if(rf_buf2_cnt == 8)
        {
					//데이타 확인
					
					value = (rf_buf2[3]-0x30) * 10000;	
					value += (rf_buf2[4]-0x30) * 1000;		
					value += (rf_buf2[5]-0x30) * 100;	
					value += (rf_buf2[6]-0x30) * 10;		
					value += (rf_buf2[7]-0x30) ;					
					
					if(value <= 65535) 
          {		
							rfid_enqueue(value);
					}					
				}
				
				rf_status2 = 0;			
				
			} 
      else 
      {
				if(rf_buf2_cnt < 8) 
        {							//버퍼 8자리 미만
					if(c >= 0x30 && c <= 0x39) 
          {		//아스키 숫자일경우
						rf_buf2[rf_buf2_cnt++] = c;
					}	
				}				
			}
			break;
			
		default :
			rf_status2 = 0; 
			break;			
	}		
}

static uint32_t rf_buf[40];
void parse_rfid(uint8_t c)
{
	static uint8_t rf_status = 0;
  	static uint16_t rf_buf_cnt = 0;
	uint32_t value = 0;
  
	switch(rf_status)
	{
		case 0:		
			if(c == 0x02) rf_status = 1;
			break;			
		case 1:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 2;
			else rf_status = 0;
			break;	
			
		case 2:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 3;
			else rf_status = 0;
			break;	
		case 3:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 4;
			else rf_status = 0;
			break;	
		case 4:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 5;
			else rf_status = 0;
			break;	
		case 5:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 6;
			else rf_status = 0;
			break;
		case 6:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 7;
			else rf_status = 0;
			break;	
		case 7:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 8;
			else rf_status = 0;
			break;
		case 8:		
			if(c >= 0x30  && c <= 0x39)  rf_status = 9;
			else rf_status = 0;
			break;			
		case 9:		
			if(c >= 0x30  && c <= 0x39) 
      		{
				rf_status = 10;
				rf_buf_cnt = 0;
				memset((void *)&rf_buf, 0, sizeof(rf_buf));
			}
			else 
        	rf_status = 0;
     	 	break;	
			
		case 10:	
				if(c > 0x30 && c <= 0x39)
				{
					value = c & 0x0F;
					rfid = value;		
					rf_buf_cnt = 0;
					rf_status = 0;
				}
			break;	
			
		default : 
			rf_buf_cnt = 0;
			rf_status = 0;
			break;
		}				
}
