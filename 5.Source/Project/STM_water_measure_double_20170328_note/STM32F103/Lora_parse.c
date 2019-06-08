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
					
					if(Recv_parse_num == WATER_ADDRESS)	//����ȣ�� ������ �߼� �մϴ�. 
					{
						//���� �˾Ƽ� ~!!
						if(Recv_data_order == 'A')
						{
							
							Recv_data_ok = 1;
							//��ɿ� ����~!!
							//���� �߼�~!!
							//�� ¯���µ� �Ѱ��� �ұ�?
							// �� ������ ~!!	
						}
						else if(Recv_data_order == 'B')
						{						
							//SELECT ��������
							
							
							
						}		
						else if(Recv_data_order == 'C')
						{						
							//ALL ENABLE
							//�ٽ� �ޱ�뵵�Դϴ�
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
				//����Ÿ�� ���������� �߼����� ��츸 ��Ÿ��
				//����Ÿ ���� �Ľ��Ұ� ����
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
	Lora_wintake.status = (Lora_wintake.status & 0xf0);	//���� �ʱ�ȭ
	SPI_FLASH_PageWrite((uint8_t *)&Lora_wintake,Mram_intake_address[Lora_wintake.index], sizeof(Lora_wintake));
	
}

char wp_buf[256];
void Send_wintake_packet(WATER_ITAKE_TABLE_STRUCT w)
{
	/*
	
	uint16_t index;				//�Ϸù�ȣ 
	uint8_t status;				//�޿��� ����		//���� ���̺��� ����Ǿ��ٸ� �ش� �������ͽ� ���� ���� ���泻���� �����Ҽ� �ֵ��� �մϴ�. 
	uint16_t rfid_num;		//��ü �ĺ���ȣ RFID
	uint16_t date;				//����� ��¥ �����(��ȯ) 
	uint16_t enter_time;		//����ð�
	uint16_t visit_time;		//ü���ð�
	uint16_t water_cnt1;		//pcnt1//qt1;				//1��� ���뷮
	uint16_t water_cnt2;		 //pcnt2//qt2;				//2��� ���뷮  
	uint16_t water_temp;	 //weight//temp//qt3;				//3��� ���뷮
	uint16_t aqt1;					//÷����1 ���뷮
	uint16_t aqt2;					//÷����2 ���뷮
	uint16_t intake_weight;			//���뷮 ���� //��ü ����
	uint8_t feeder_num;			//������ ���̱� ��ȣ
	*/
	uint16_t a,b,c,d,e,f,g,h,i,j,k,l;
		
	a = WATER_ADDRESS;
	b = w.index;
	c = w.status;
	d = w.rfid_num;
	e = w.date;
	f = w.enter_time;		//����ð�
	g = w.visit_time;		//�ӹ��ð�
	h = w.water_cnt1;
	i = w.water_cnt2;
	j = w.water_temp;
	k = w.intake_weight;
	l = w.aqt1;		
	
		
	memset((void *)&wp_buf, 0, sizeof(wp_buf));	//�ʱ�ȭ
	sprintf((char *)&wp_buf, "$Trup=W:a%d:b%d:c%d:d%d:e%d:f%d:g%d:h%d:i%d:j%d:k%d:l%d:E\n\r"
					,a		//a =  ������ ��ȣ 
					,b	//b=�Ϸù�ȣ
					,c	//c=�޿�������
					,d	//d=��ü��ȣ
					,e	//e=��¥
					,f	//f=����ð�
					,g	//g=����ð�
					,h	//h=ī��Ʈ1
					,i	//i=ī��Ʈ2
					,j	//j=��������1
					,k	//k=��������
					,l);	//
	
	Uart_puts(USART1, (char *)&wp_buf);						
	
}

extern volatile uint8_t  Oper_Cnt;		// ���� ������

void Send_status_packet(WATER_ITAKE_TABLE_STRUCT w)
{
	/*	
	uint16_t index;				//�Ϸù�ȣ 
	uint8_t status;				//�޿��� ����		//���� ���̺��� ����Ǿ��ٸ� �ش� �������ͽ� ���� ���� ���泻���� �����Ҽ� �ֵ��� �մϴ�. 
	uint16_t rfid_num;		//��ü �ĺ���ȣ RFID
	uint16_t date;				//����� ��¥ �����(��ȯ) 
	uint16_t enter_time;		//����ð�
	uint16_t visit_time;		//ü���ð�
	uint16_t water_cnt1;		//pcnt1//qt1;				//1��� ���뷮
	uint16_t water_cnt2;		 //pcnt2//qt2;				//2��� ���뷮  
	uint16_t water_temp;	 //weight//temp//qt3;				//3��� ���뷮
	uint16_t aqt1;					//÷����1 ���뷮
	uint16_t aqt2;					//÷����2 ���뷮
	uint16_t intake_weight;			//���뷮 ���� //��ü ����
	uint8_t feeder_num;			//������ ���̱� ��ȣ
	*/
	uint16_t a,b,c,d,e,f,g,h,i,j,k,l;
		
	a = WATER_ADDRESS;
	b = Oper_Cnt;
	c = w.status;
	d = w.rfid_num;
	e = w.date;
	f = w.enter_time;		//����ð�
	g = w.visit_time;		//�ӹ��ð�
	h = Water_pcnt1_read();
	i =  Water_pcnt2_read();
	j = w.water_temp;
	k = Stable_weight;
	l = 0;		
	
		
	memset((void *)&wp_buf, 0, sizeof(wp_buf));	//�ʱ�ȭ
	sprintf((char *)&wp_buf, "$Trup=S:a%d:b%d:c%d:d%d:e%d:f%d:g%d:h%d:i%d:j%d:k%d:l%d:E\n\r"
					,a	//a =  ADDRESS
					,b	//b= opcnt
					,c	//c=�޿�������
					,d	//d=��ü��ȣ
					,e	//e=��¥
					,f	//f=����ð�
					,g	//g=����ð�
					,h	//h=ī��Ʈ1
					,i	//i=ī��Ʈ2
					,j	//j=��������1
					,k	//k=��������
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
		//�ϴ� ���� ����Ÿ�� �����ϴ��� Ȯ���Ѵ�!!
		for(uint16_t i=0;i<MRAM_INTAKE_MAX_ADDRESS;i++)
		{		
			SPI_FLASH_BufferRead((uint8_t*)&Lora_wintake, Mram_intake_address[i], sizeof(WATER_ITAKE_TABLE_STRUCT));
			//Intake_data_load((uint8_t*)&Lora_wintake, Mram_intake_address[i]);
			if((Lora_wintake.status & 0x01) == 0x01) {
				//�ش� ������ ������ �����Ŷ� �������Ѵ� ~!!
				search_address = i;	//���° ����Ÿ�� ���žȵȰǰ� Ȯ��
				break;
			}
		}
		
		if(search_address != 0)
		{
			//������ �ִ°Ŵ�~!! 		
			Send_wintake_packet(Lora_wintake);
			
		}
		else
		{
			//������??
			//���� ������ �����°Ŵ�~!!
			Send_status_packet(W_Intake);
		}
	}	
}

void Sub_lora_process(void)
{
	lora_parse();
	
	//�߼��� ���⼭
	
}