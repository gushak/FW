#include "hw_control.h"

volatile uint8_t recv_line_buf[256];
volatile uint8_t recv_line_cnt=0;
volatile uint8_t kbuf;
volatile uint16_t ContentLength = 0;
volatile uint8_t Recv_parse_num = 0;
volatile uint8_t Recv_data_ok = 0;
volatile uint8_t Recv_data_order = 0;

MMQT_StateMachine_t mmqt;
WATER_ITAKE_TABLE_STRUCT Lora_wintake;
Error_Struct_t error_st;
#define CHARISNUM(x)        ((x) >= '0' && (x) <= '9')
#define CHARISHEXNUM(x)     (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define CHAR2NUM(x)         ((x) - '0')

void lora_init(void)
{
  memset((void *)&Lora_wintake, 0, sizeof(Lora_wintake));
  char *pString;
  char String[16] = "AT+FREQ=1";
  char String1[10] = "AT+GID=123";
  char String2[12] = "AT+FARM=";
  sprintf((char*)&String2[8],"%d",Config.Value[12]);
  
  for(uint8_t init_index = 0; init_index < 3; init_index++)
  {
      if (init_index == 0)
      {
          pString = String;
      }
      else if (init_index == 1)
      {
          pString = String1;
      }
      else if (init_index == 2)
      {
          pString = String2;
      }
      
      Uart_puts(USART1, (char *)pString);
      Uart_puts(USART1, (char *)"\r\n");
      delay_ms(500);
  }
  
  LORA_RESET;
  delay_ms(10);
  LORA_SET;
  
}

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
	if (minus) 
  {
		return 0 - sum;
	}
	
	/* Return number */
	return sum;
}

extern tm_struct Now;

volatile tm_struct Base_time;
volatile char tbuf[15];
volatile u32 t1=0,t2=0;
volatile u8 Lora_Status = true;
char* ptr;
char *parse_crc ;
char *addr;
uint16_t cal_crc;
char string_crc[4];
uint16_t local_index = 0;
uint16_t crc_index = 0;
uint8_t buffer[100] = {0,};
void lora_parse(void)
{
    uint8_t data = 0;
    //uint16_t local_index = 0;
    ContentLength = 0; 
    
  
    if(!uart1_is_empty())
    {
        data = uart1_dequeue();
        
        if(data == '\n')
        {    
            if(recv_line_buf[recv_line_cnt -1] == '\r')
            {
              //char *parse_crc ;
                    //char string_crc[4];
                    //uint16_t cal_crc;
                    
                if((ptr = strstr((char const *)recv_line_buf, "WATER_ACK:")) != NULL)  
                {
                    //char *parse_crc ;
                    //char string_crc[4];
                    //uint16_t cal_crc;
                    
                    parse_crc = ptr;
                    
                    for(uint8_t i = 0; ptr[i] != '\r'; i++)
                    {
                        if(local_index == 0)
                        {
                            if(parse_crc[0] == 'W' && parse_crc[1] == 'A' && parse_crc[2] == 'T' && parse_crc[3] == 'E')
                            {
                                parse_crc += 10;
                                mmqt.dev_id = atoi(parse_crc);
                            }
                        }
                        else if(local_index == 1)
                        {
                           if(parse_crc[0] == 'R' && parse_crc[1] == 'F' && parse_crc[2] ==  'I' && parse_crc[3] == 'D')
                           {
                               parse_crc += 5;
                               mmqt.rf_id = atoi(parse_crc);
                           }
                        }
                        else if(local_index == 2)
                        {
                            if(parse_crc[0] == 'T' && parse_crc[1] == 'I' && parse_crc[2] ==  'M' && parse_crc[3] == 'E')
                            {
                                 parse_crc += 5; 
                                 mmqt.hour1 = ((int)(parse_crc[0] - '0'));
                                 mmqt.hour2 = ((int)(parse_crc[1] - '0'));
                                 mmqt.min1 = ((int)(parse_crc[2] - '0'));
                                 mmqt.min2 = ((int)(parse_crc[3] - '0'));
                                 mmqt.sec1 = ((int)(parse_crc[4] - '0'));
                                 mmqt.sec2 = ((int)(parse_crc[5] - '0'));
                            }
                        }
                        else if(local_index == 3)
                        {
                            if(parse_crc[0] == 'D' && parse_crc[1] == 'A' && parse_crc[2] ==  'T' && parse_crc[3] == 'E')
                            {
                                 parse_crc += 5;
                                 mmqt.year1 = ((int)(parse_crc[0] - '0'));
                                 mmqt.year2 = ((int)(parse_crc[1] - '0'));
                                 mmqt.mon1 = ((int)(parse_crc[2] - '0'));
                                 mmqt.mon2 = ((int)(parse_crc[3] - '0'));
                                 mmqt.day1 = ((int)(parse_crc[4] - '0'));
                                 mmqt.day2 = ((int)(parse_crc[5] - '0'));          
                            }
                        }
                        else if(local_index == 4)
                        {
                           parse_crc += 1;
                           crc_index = i + 25;
                           local_index = 0;
                           break;
                        }
                        
                        if(parse_crc[0] == ',')
                            local_index++;
                   
                        
                        parse_crc++;
                    }
                    
                    for(uint8_t i = 0; i < crc_index; i++)
                    {
                        buffer[i] = atoi(&ptr[i]);
                    }
                    cal_crc = CheckSum((uint8_t*)ptr, crc_index);
                    
                    sprintf(string_crc, "%x", cal_crc);
                    
                    if((string_crc[0] == parse_crc[0]) && (string_crc[1] == parse_crc[1]) && (string_crc[2] == parse_crc[2]))
                    {
                        if(mmqt.dev_id == CON_ADDRESS)
                            if(mmqt.rf_id == W_Intake.rfid_num)
                            {
                                if (Lora_Status == true)
                                    Send_ok_data_save(); 
                                else if(Lora_Status == false)
                                {
                                    Lora_Status = true;
                                }
                                
                                Base_time.tm_hour = mmqt.hour1 * 10;
                                Base_time.tm_hour += mmqt.hour2;
                                
                                Base_time.tm_min = mmqt.min1 * 10;
                                Base_time.tm_min += mmqt.min2;
                                
                                Base_time.tm_sec = mmqt.sec1 * 10;
                                Base_time.tm_sec += mmqt.sec2;
                                
                                Base_time.tm_year = 2000;
                                Base_time.tm_year += mmqt.year1 * 10;
                                Base_time.tm_year += mmqt.year2;
                                
                                Base_time.tm_mon = mmqt.mon1 * 10;
                                Base_time.tm_mon += mmqt.mon2;

                                Base_time.tm_mday = mmqt.day1* 10;
                                Base_time.tm_mday += mmqt.day2;
                                
                                if(Base_time.tm_year >= 2019) 
                                  if(Base_time.tm_mon >= 1 && Base_time.tm_mon <=12)
                                    if(Base_time.tm_mday >= 1 && Base_time.tm_mday <= 31)
                                      if(Base_time.tm_hour <= 24)
                                        if(Base_time.tm_min <= 60)
                                          if(Base_time.tm_sec <= 60)
                                          {			
                                            Base_time.tm_year -= 1900;
                          
                                            t1 = RTC_DateToBinary((tm_struct *)&Now);
                                            t2 = RTC_DateToBinary((tm_struct *)&Base_time);
                                            
                                            if( abs(t1-t2) > 60) 
                                            {
                                              RTC_Configuration();												
                                              
                                              Time_Adjust(Base_time);
                                              BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
                                            }		
                                          } 
                                 memset((void *)&W_Intake, 0, sizeof(W_Intake));	
                                LED_FL;
                            }
                    }
                }   
                memset((void *)&recv_line_buf, 0, sizeof(recv_line_buf));
                recv_line_cnt = 0;
            }
        }
        else
        {
            recv_line_buf[recv_line_cnt++] = data;
        }
    }
}

void Send_ok_data_save(void)
{
    Lora_wintake.status = (Lora_wintake.status & 0xf0);	
    SPI_FLASH_PageWrite((uint8_t *)&Lora_wintake,Mram_intake_address[Lora_wintake.index], sizeof(Lora_wintake));
}

char wp_buf[62];
void Send_wintake_packet(WATER_ITAKE_TABLE_STRUCT w)
{
    memset((void *)wp_buf, 0, sizeof(wp_buf));
  
    uint16_t a,b,c,d,f,g,h,i,j,k,l;
    uint16_t year;
    uint8_t month,day;
  
    sprintf((char *)wp_buf, "AT=");
    
    a = CON_ADDRESS;
    b = w.index;
    c = w.status;
    d = rfid;
    year = (w.date & 0xFFFF0000) >> 16;
    month = (w.date & 0x0000FF00) >> 8;
    day = w.date;
    f = w.enter_time;		//입장시간
    g = w.visit_time;		//머문시간
    h = w.water_cnt1;
    i = w.water_cnt2;
    j = w.enter_weight;
    k = w.end_weight;
    l = w.water_temp;		
        
    sprintf((char *)&wp_buf[3], "W=2,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
            ,a	//a =  ADDRESS
            ,b	//b= opcnt
            ,c	//c=급여조상태
            ,d	//d=개체번호
            ,year	//년원일 e
            ,month
            ,day
            ,f	//f=입장시간
            ,g	//g=퇴장시간
            ,h	//h=카운트1
            ,i	//i=카운트2
            ,j	//j=시작저울1
            ,k	//k=종료저울
            ,l);	//
    
    uint16_t index_comma = 0;
    
    for(uint8_t i = 0; i < sizeof(wp_buf); i++)
    {
        if(wp_buf[i] == ',')
        {
            index_comma++;
            
            if(index_comma == 14)
            {
                for(uint8_t j = 0; j < sizeof(wp_buf) - (j - 1); j++)
                {
                    wp_buf[i+4] = wp_buf[i+3];
                }
                wp_buf[i+3] = '.';
                wp_buf[i+5] = ',';
                
                if(wp_buf[i+4] == '\0')
                    wp_buf[i+4] = '0';
                
                break;
            }
        }
    }

    uint8_t index = 0;
    
    for(uint8_t i = 0; i < sizeof(wp_buf); i ++)
    {
        if(wp_buf[i] == '\0')
        {
          index = i;
          break;
        }
    }
    
    
    uint16_t ret = CheckSum((uint8_t*)&wp_buf[5], index - 5);
    
    sprintf((char *)&wp_buf[index],"%x",ret);
           
    Uart_puts(USART1, (char *)wp_buf);
    Uart_puts(USART1, "\r\n");
    
    Intake_head_increment(); 
    Recv_data_ok = 0;				
}
uint32_t rtc;
uint8_t index = 0;
void Send_error_packet(Error_Struct_t* e)
{   
    memset((void *)wp_buf, 0, sizeof(wp_buf));

    uint16_t a,b,c,d;
    uint16_t year,month;
    uint8_t day;
    sprintf((char *)wp_buf, "AT=");
    
    rtc = Get_Date();
	
    a = CON_ADDRESS;
    b = e->status;
    c = e->rf_id;
    year = (rtc >> 16) & 0xFFFF;
    month = (rtc >> 8) & 0xFF;
    day = rtc & 0xFF;
    d = GetTime_min();
        
    sprintf((char *)&wp_buf[3], "W=3,%d,%d,%d,%d,%d,%d,%d,"
    ,a	
    ,b	
    ,c	
    ,year
    ,month
    ,day
    ,d);	//
      
    for(uint8_t i = 0; i < sizeof(wp_buf); i++)
    {
          if(wp_buf[i] == '\0')
          {
                  index = i;
                  break;
          }
    }

    uint16_t ret = CheckSum((uint8_t*)&wp_buf[5], index - 5);

    sprintf((char *)&wp_buf[index],"%x",ret);

    Uart_puts(USART1, (char *)wp_buf);
    Uart_puts(USART1, "\r\n");
    memset((void*)e, 0x00, sizeof(e));
    index = 0;
}

extern volatile uint8_t  Oper_Cnt;		// 섭취 동작중
void Send_status_packet(WATER_ITAKE_TABLE_STRUCT w)
{ 
    memset((void *)wp_buf, 0, sizeof(wp_buf));
  
    uint16_t a,b,c,d,f,g,h,i,j,k,l;
    uint16_t year;
    uint8_t month,day;
  
    sprintf((char *)wp_buf, "AT=");
    
    a = CON_ADDRESS;
    b = w.index;
    c = w.status;
    d = w.rfid_num;
    year = (w.date & 0xFFFF0000) >> 16;
    month = (w.date & 0x0000FF00) >> 8;
    day = w.date;
    f = w.enter_time;		//입장시간
    g = w.visit_time;		//머문시간
    h = w.water_cnt1;
    i = w.water_cnt2;
    j = w.enter_weight;
    k = w.end_weight;
    l = w.water_temp;		
        
    sprintf((char *)&wp_buf[3], "W=1,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
            ,a	//a =  ADDRESS
            ,b	//b= opcnt
            ,c	//c=급여조상태
            ,d	//d=개체번호
            ,year	//년원일 e
            ,month
            ,day
            ,f	//f=입장시간
            ,g	//g=퇴장시간
            ,h	//h=카운트1
            ,i	//i=카운트2
            ,j	//j=시작저울1
            ,k	//k=종료저울
            ,l);	//
    
    uint16_t index_comma = 0;
    
    for(uint8_t i = 0; i < sizeof(wp_buf); i++)
    {
        if(wp_buf[i] == ',')
        {
            index_comma++;
            
            if(index_comma == 14)
            {
                for(uint8_t j = 0; j < sizeof(wp_buf) - (j - 1); j++)
                {
                    wp_buf[i+4] = wp_buf[i+3];
                }
                wp_buf[i+3] = '.';
                wp_buf[i+5] = ',';
                
                if(wp_buf[i+4] == '\0')
                    wp_buf[i+4] = '0';
                
                break;
            }
        }
    }

    uint8_t index = 0;
    
    for(uint8_t i = 0; i < sizeof(wp_buf); i ++)
    {
        if(wp_buf[i] == '\0')
        {
          index = i;
          break;
        }
    }
    
    
    uint16_t ret = CheckSum((uint8_t*)&wp_buf[5], index - 5);
    
    sprintf((char *)&wp_buf[index],"%x",ret);
           
    Uart_puts(USART1, (char *)wp_buf);
    Uart_puts(USART1, "\r\n");
    
    Intake_head_increment(); 
    Recv_data_ok = 0;		
}
uint16_t lora_tick = 0;
uint8_t init_send = 0;
uint16_t search_address = 0;
void parse_lora_packet(void)
{
    if(Recv_data_ok == 1)
    {
        uint8_t delay_var = 0;
        search_address = 0;
        
        if (Lora_Status == true)
        {
            if(!init_send)
            {
                lora_tick = 10*CON_ADDRESS;
                init_send = 1;
            }
            
            uint16_t memory_index = Intake_Head();
            
            SPI_FLASH_BufferRead((uint8_t*)&Lora_wintake, Mram_intake_address[memory_index], sizeof(WATER_ITAKE_TABLE_STRUCT));
            if(!memory_index)
            {
              if(!lora_tick)
              {
                Send_status_packet(Lora_wintake);
                lora_tick = 10*Config.Value[0];
              }
            }
            else
            {
              if(!lora_tick)
              {
                Send_wintake_packet(Lora_wintake);
                lora_tick = 10*Config.Value[0];
              }
            }
             
        }
        else
        {      
            for(uint16_t i=0;i<MRAM_INTAKE_MAX_ADDRESS;i++) //일단 보낼 데이타가 존재하는지 확인한다
            {		
                SPI_FLASH_BufferRead((uint8_t*)&Lora_wintake, Mram_intake_address[i], sizeof(WATER_ITAKE_TABLE_STRUCT));
                
                if((Lora_wintake.status & 0x01) == 0x01)  //해당 정보는 보내지 않은거라 보내야한다 
                {
                    if (delay_var == 0 )
                    {
                        delay_ms(10*CON_ADDRESS);
                        delay_var++;
                    }
                    Send_wintake_packet(Lora_wintake);
                    Send_ok_data_save();
                    search_address = 0;    
                    Recv_data_ok = 1;
                }
            }
        }
    }
	else
	{
		
	}
}


void Sub_lora_process(void)
{
    lora_parse();
}

uint16_t CheckSum(uint8_t *buffer, uint16_t size)
{
    uint16_t Sum = 0;
    uint16_t i = 0;
    
    while(size--)
    {
        Sum+=buffer[i++];
    }
    return Sum;
}