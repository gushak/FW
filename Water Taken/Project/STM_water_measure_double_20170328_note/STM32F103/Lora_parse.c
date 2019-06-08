#include "hw_control.h"

volatile uint8_t recv_line_buf[256];
volatile uint8_t recv_line_cnt=0;
volatile uint8_t kbuf;
volatile uint16_t ContentLength = 0;
volatile uint8_t Recv_parse_num = 0;
volatile uint8_t Recv_data_ok = 0;
volatile uint8_t Recv_data_order = 0;


#define CHARISNUM(x)        ((x) >= '0' && (x) <= '9')
#define CHARISHEXNUM(x)     (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define CHAR2NUM(x)         ((x) - '0')

int32_t ParseNumber_W5500(char* ptr, uint8_t* cnt) {
	uint8_t minus = 0;
	int32_t sum = 0;
	uint8_t i = 0;
	
	/* Check for minus character */
	if (*ptr == '-') {
		minus = 1;
		ptr++;
		i++;
	}
	
	/* Parse number */
	while (CHARISNUM(*ptr)) {
		sum = 10 * sum + CHAR2NUM(*ptr);
		ptr++;
		i++;
	}
	
	/* Save number of characters used for number */
	if (cnt != NULL) {
		*cnt = i;
	}
	
	/* Minus detected */
	if (minus) {
		return 0 - sum;
	}
	
	/* Return number */
	return sum;
}

extern tm_struct Now;

volatile tm_struct Base_time;
volatile char tbuf[15];
volatile u32 t1=0,t2=0;
void lora_parse(void)
{
	uint8_t data = 0;
	char* ptr;
	
	ContentLength = 0;
	if(!uart1_is_empty())
	{
		data = uart1_dequeue();
		
		if(data == 0x0a)
		{
			recv_line_cnt = 0;
			if ((ptr = strstr((char const *)recv_line_buf, "Urdp:100:")) != NULL					//Slave -> Master
			) {	
				ptr += 9;		
				kbuf = *ptr;
				/* Parse content length */ //vinagar
				ContentLength = (kbuf - 0x30);
				
				if(ContentLength < 100)		//address 
				{
					Recv_parse_num = ContentLength;
					Recv_data_order = *(recv_line_buf + 11);
					
					if(Recv_parse_num == WATER_ADDRESS)	//기기번호가 같으면 발송 합니다. 
					{
						//요기는 알아서 ~!!
						if(Recv_data_order == 'A')
						{
							
							Recv_data_ok = 1;
							//명령에 따름~!!
							//지꺼 발송~!!
							//걍 짱나는데 한개씩 할까?
							// 걍 보내자 ~!!	
						}
						else if(Recv_data_order == 'B')
						{						
							//SELECT 만들어야함
							
							
							
						}		
						else if(Recv_data_order == 'C')
						{						
							//ALL ENABLE
							//다시 받기용도입니다
							Intake_page_all_enable();
							
						}	
					}
					
					memcpy((void *)&tbuf, (const void *)&recv_line_buf[15], 15);
					/*8
					tbuf[0] =  (recv_line_buf + 15);					
					tbuf[1] =  *(recv_line_buf + 16);
					tbuf[2] =  *(recv_line_buf + 17);
					tbuf[3] =  *(recv_line_buf + 18);
					tbuf[4] =  *(recv_line_buf + 19);
					tbuf[5] =  *(recv_line_buf + 20);
					tbuf[6] =  *(recv_line_buf + 21);
					tbuf[7] =  *(recv_line_buf + 22);
					tbuf[8] =  *(recv_line_buf + 23);
					tbuf[9] =  *(recv_line_buf + 24);
					tbuf[10] =  *(recv_line_buf + 25);
					tbuf[11] =  *(recv_line_buf + 26);
					tbuf[12] =  *(recv_line_buf + 27);
					tbuf[13] =  *(recv_line_buf + 28);
					tbuf[14] =  *(recv_line_buf + 29);
					*/
					Base_time.tm_year = ((int)(tbuf[0] - '0') * 1000);
					Base_time.tm_year += ((int)(tbuf[1] - '0') * 100);
					Base_time.tm_year += ((int)(tbuf[2] - '0') * 10);
					Base_time.tm_year += ((int)(tbuf[3] - '0'));
					
					Base_time.tm_mon = ((int)(tbuf[4] - '0') * 10);
					Base_time.tm_mon += ((int)(tbuf[5] - '0'));

					Base_time.tm_mday = ((int)(tbuf[6] - '0') * 10);
					Base_time.tm_mday += ((int)(tbuf[7] - '0'));
					
					Base_time.tm_hour = ((int)(tbuf[8] - '0') * 10);
					Base_time.tm_hour += ((int)(tbuf[9] - '0'));
					
					Base_time.tm_min = ((int)(tbuf[10] - '0') * 10);
					Base_time.tm_min += ((int)(tbuf[11] - '0'));
					
					Base_time.tm_sec = ((int)(tbuf[12] - '0') * 10);
					Base_time.tm_sec += ((int)(tbuf[13] - '0'));

					
					if(Base_time.tm_year >= 2017) 
						if(Base_time.tm_mon >= 1 && Base_time.tm_mon <=12)
							if(Base_time.tm_mday >= 1 && Base_time.tm_mday <= 31)
								if(Base_time.tm_hour <= 24)
									if(Base_time.tm_min <= 60)
										if(Base_time.tm_sec <= 60)
										{			
											Base_time.tm_year -= 1900;
		
											t1 = RTC_DateToBinary((tm_struct *)&Now);
											t2 = RTC_DateToBinary((tm_struct *)&Base_time);
											
											if(abs(t1-t2) > 60) {
											
												RTC_Configuration();												
												
												Time_Adjust(Base_time);
												BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);

											}		
										}
											
					
					
					//datetime
				}				
			}
			
			if ((ptr = strstr((char const *)recv_line_buf, "succesfully")) != NULL					//Slave -> Master
			) {				
				//데이타를 정상적으로 발송했을 경우만 나타남
				//데이타 이후 파싱할거 없음
				//
				Send_ok_data_save();
				LED_FL;
			}
			
			memset((void *)&recv_line_buf, 0, sizeof(recv_line_buf));
		}
		else {
			recv_line_buf[recv_line_cnt++] = data;
		}
	}
	
}

WATER_ITAKE_TABLE_STRUCT Lora_wintake;
void Send_ok_data_save(void)
{
	Lora_wintake.status = (Lora_wintake.status & 0xf0);	//갱신 초기화
	SPI_FLASH_PageWrite((uint8_t *)&Lora_wintake,Mram_intake_address[Lora_wintake.index], sizeof(Lora_wintake));
	
}

char wp_buf[256];
void Send_wintake_packet(WATER_ITAKE_TABLE_STRUCT w)
{
	/*
	
	uint16_t index;				//일련번호 
	uint8_t status;				//급여조 상태		//급이 테이블이 변경되었다면 해당 스테이터스 증가 시켜 변경내용을 전송할수 있도록 합니다. 
	uint16_t rfid_num;		//개체 식별번호 RFID
	uint16_t date;				//저장된 날짜 년월일(변환) 
	uint16_t enter_time;		//입장시간
	uint16_t visit_time;		//체류시간
	uint16_t water_cnt1;		//pcnt1//qt1;				//1사료 섭취량
	uint16_t water_cnt2;		 //pcnt2//qt2;				//2사료 섭취량  
	uint16_t water_temp;	 //weight//temp//qt3;				//3사료 섭취량
	uint16_t aqt1;					//첨가제1 섭취량
	uint16_t aqt2;					//첨가제2 섭취량
	uint16_t intake_weight;			//섭취량 무게 //개체 무게
	uint8_t feeder_num;			//섭취한 급이기 번호
	*/
	uint16_t a,b,c,d,e,f,g,h,i,j,k,l;
		
	a = WATER_ADDRESS;
	b = w.index;
	c = w.status;
	d = w.rfid_num;
	e = w.date;
	f = w.enter_time;		//입장시간
	g = w.visit_time;		//머문시간
	h = w.water_cnt1;
	i = w.water_cnt2;
	j = w.water_temp;
	k = w.intake_weight;
	l = w.aqt1;		
	
		
	memset((void *)&wp_buf, 0, sizeof(wp_buf));	//초기화
	sprintf((char *)&wp_buf, "$Trup=W:a%d:b%d:c%d:d%d:e%d:f%d:g%d:h%d:i%d:j%d:k%d:l%d:E\n\r"
					,a		//a =  물섭취 번호 
					,b	//b=일련번호
					,c	//c=급여조상태
					,d	//d=개체번호
					,e	//e=날짜
					,f	//f=입장시간
					,g	//g=퇴장시간
					,h	//h=카운트1
					,i	//i=카운트2
					,j	//j=시작저울1
					,k	//k=종료저울
					,l);	//
	
	Uart_puts(USART1, (char *)&wp_buf);						
	
}

extern volatile uint8_t  Oper_Cnt;		// 섭취 동작중

void Send_status_packet(WATER_ITAKE_TABLE_STRUCT w)
{
	/*	
	uint16_t index;				//일련번호 
	uint8_t status;				//급여조 상태		//급이 테이블이 변경되었다면 해당 스테이터스 증가 시켜 변경내용을 전송할수 있도록 합니다. 
	uint16_t rfid_num;		//개체 식별번호 RFID
	uint16_t date;				//저장된 날짜 년월일(변환) 
	uint16_t enter_time;		//입장시간
	uint16_t visit_time;		//체류시간
	uint16_t water_cnt1;		//pcnt1//qt1;				//1사료 섭취량
	uint16_t water_cnt2;		 //pcnt2//qt2;				//2사료 섭취량  
	uint16_t water_temp;	 //weight//temp//qt3;				//3사료 섭취량
	uint16_t aqt1;					//첨가제1 섭취량
	uint16_t aqt2;					//첨가제2 섭취량
	uint16_t intake_weight;			//섭취량 무게 //개체 무게
	uint8_t feeder_num;			//섭취한 급이기 번호
	*/
	uint16_t a,b,c,d,e,f,g,h,i,j,k,l;
		
	a = WATER_ADDRESS;
	b = Oper_Cnt;
	c = w.status;
	d = w.rfid_num;
	e = w.date;
	f = w.enter_time;		//입장시간
	g = w.visit_time;		//머문시간
	h = Water_pcnt1_read();
	i =  Water_pcnt2_read();
	j = w.water_temp;
	k = Stable_weight;
	l = 0;		
	
		
	memset((void *)&wp_buf, 0, sizeof(wp_buf));	//초기화
	sprintf((char *)&wp_buf, "$Trup=S:a%d:b%d:c%d:d%d:e%d:f%d:g%d:h%d:i%d:j%d:k%d:l%d:E\n\r"
					,a	//a =  ADDRESS
					,b	//b= opcnt
					,c	//c=급여조상태
					,d	//d=개체번호
					,e	//e=날짜
					,f	//f=입장시간
					,g	//g=퇴장시간
					,h	//h=카운트1
					,i	//i=카운트2
					,j	//j=시작저울1
					,k	//k=종료저울
					,l);	//
	
	Uart_puts(USART1, (char *)&wp_buf);						
	
}

uint16_t search_address = 0;
void Send_lora_packet(void)
{
	
	if(Recv_data_ok == 1)
	{
		Recv_data_ok = 0;
	
		search_address = 0;
		memset((void *)&Lora_wintake, 0, sizeof(Lora_wintake));
		//일단 보낼 데이타가 존재하는지 확인한다!!
		for(uint16_t i=0;i<MRAM_INTAKE_MAX_ADDRESS;i++)
		{		
			SPI_FLASH_BufferRead((uint8_t*)&Lora_wintake, Mram_intake_address[i], sizeof(WATER_ITAKE_TABLE_STRUCT));
			//Intake_data_load((uint8_t*)&Lora_wintake, Mram_intake_address[i]);
			if((Lora_wintake.status & 0x01) == 0x01) {
				//해당 정보는 보내지 않은거라 보내야한다 ~!!
				search_address = i;	//몇번째 데이타가 갱신안된건가 확인
				break;
			}
		}
		
		if(search_address != 0)
		{
			//보낼게 있는거다~!! 		
			Send_wintake_packet(Lora_wintake);
			
		}
		else
		{
			//없으면??
			//상태 정보를 보내는거다~!!
			Send_status_packet(W_Intake);
		}
	}	
}

void Sub_lora_process(void)
{
	lora_parse();
	
	//발송은 여기서
	
}