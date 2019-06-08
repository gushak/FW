#include "hw_control.h"
#include "water_intake.h"
#include "Lora_parse.h"
#define MAIN_LOOP_TIME       10
#define VIEW_LOOP_TIME       100 
#define CIRCLE_MAIN				10
#define CAL_LOOP_TIME 		1000

extern uint16_t VirtAddVarTab[NumbOfVar]; 
extern WATER_ITAKE_TABLE_STRUCT Lora_wintake;
extern Water_Info_t Weight;
volatile uint8_t NFC_Reader_Ok = 0;
volatile uint8_t Mode_flag = 0;
volatile uint16_t Mode_delay = 0;
volatile uint16_t FET_value[10]; 		
volatile uint8_t Stable_run_flag = 0;
uint8_t send_flag = 0;

void lcd_view_init(void)
{
	TM_HD44780_Puts(0,0,"Dawoon 20170927 ");
	TM_HD44780_Puts(0,1,"Water Intake    ");
	Delay(2000);
}
uint8_t calibration_mode = 0;
void setting(void)
{	
	lcd_view_init();
	load_eeprom();
  lora_init();
#if 0 
  uint8_t head = 1;
  SPI_FLASH_PageWrite((uint8_t*)&head, 0x06, 2);
#endif
  
  uint8_t i = 0;
  uint32_t factor_addr = 0;
  factor_addr = (uint32_t)0x0800FC00;
  while(*(uint32_t*)factor_addr != 0xFFFFFFFF)
  {
  	Weight.value[i] = *(uint32_t*)factor_addr;
	factor_addr = factor_addr+2;
	i++;
  }
  Weight.factor = atof(Weight.value);
  Weight.factor = 2.9;
}
volatile uint32_t FET_sum[2];
volatile uint16_t Input_cnt[4];
volatile uint8_t k = 0;
extern volatile uint8_t Input_value[2];
extern volatile uint16_t Setting_delay;

void timer_10ms_loop(void)
{
    SW_check();
    
    if(Setting_delay != 0) Setting_delay--;	
    

    if(INPUT1 == 0) 
    { 	
        if(	Input_cnt[0]++  > 2) 
        {
            Input_value[0] = 1;
        } 
        else 
            Input_value[0] = 0;
        
    } 
    else 
    {
        Input_cnt[0] = 0;
        Input_value[0] = 0;
    }	
    
    
    if(INPUT2 == 0) 
    {
        if(	Input_cnt[1]++  > 2) 		
        {
            Input_value[1] = 1;
        }	
        else	
            Input_value[1] = 0;	
    } 
    else 
    {
        Input_cnt[1] = 0;
        Input_value[1] = 0;
    }
	
	if(!R_IR_check())
	{
		FET_value[0] = 50;
		FET_sum[0] = FET_value[0];
		if(FET_sum[0] >=40000)
		{			
			//error_st.status = 0x01;
                        //error_st.rf_id = rfid;
			//Send_error_packet(&error_st);
		}
	}
	
	if(!R_IR_check2())
	{
		if(FET_sum[1] >= 20000)
		{			
			//error_st.status = 0x02;
                        //error_st.rf_id = rfid;
			//Send_error_packet(&error_st);
		}
	}	
    
    if(FET_value[0] != 0)
	{ 
		FET_value[0]--;   
		FET1_ON; 
	}
	else
	{   
		FET_sum[0] = 0;
		FET1_OFF; 
	}	
    if(FET_value[1] != 0) 
	{ 
		FET_value[1]--; 
	    FET2_ON; 
	}
	else 
	{   
		FET_sum[1] = 0;
		FET2_OFF; 
	}	//솔밸브
    if(FET_value[2] != 0) { FET_value[2]--;   FET3_ON; } else {   FET3_OFF; }	//온도 컨트롤러
    if(FET_value[3] != 0) { FET_value[3]--;   FET4_ON; } else {   FET4_OFF; }  //An Water supply 
    if(FET_value[4] != 0) { FET_value[4]--;   FET5_ON; } else {   FET5_OFF; }	    
}



extern uint8_t water_fill_delay;
extern volatile uint16_t Stable_water_delay;

volatile uint16_t FET_FL_cnt = 0;
volatile uint16_t Stable_water_delay = 0;
volatile uint8_t Stable_ch_flag = 0;

extern volatile uint8_t  Oper_Cnt;		// 섭취 동작중

volatile uint16_t Full_water_check_cnt = 0;
volatile uint8_t FULL_Water_Error = 0;

void timer_100ms_loop(void)
{
	if(tick > 0xFF) 
		tick = 0;
	else
		tick--;
	if(lora_tick > 0xFF)
		lora_tick = 0;
	else if(init_send)
		lora_tick--;
	
	if(Mode_delay != 0) Mode_delay--;
	if(water_fill_delay != 0) water_fill_delay--;
	if(Stable_water_delay != 0) Stable_water_delay --;
	
}

void timer_1000ms_loop(void)
{
	LED1_FL;
}


extern uint32_t pcnt , pcnt2, pweight;
char buf_cnt_sensor[16];
char buf_SW_fetvalue[16];
char buf_menu_num[16];

void main_view(void) 
{
	lcd_view_print();
}

volatile uint16_t test =0 ;

void lcd_view_print(void)
{
	sprintf((char*)&buf_cnt_sensor, " %07d", pcnt);	
	TM_HD44780_Puts(0,1, buf_cnt_sensor);	
}

void basic_mode(void)
{
	TM_HD44780_Puts(0,0, "DW_Water_Intake");	
	TM_HD44780_Puts(0,1, "2017.09.22");	//>로라통신 
	
	char buf[16] = {0,};
	sprintf(buf, "%d", CON_ADDRESS);
	TM_HD44780_Puts(10,1, buf); //>고유번호
}


void loop_main(void)
{
	#ifdef WATCHDOG_ENABLE
	Wdt_reset();
	#endif

}

volatile uint16_t send_rfid = 0;

extern volatile uint8_t Setting_enable;    
volatile uint8_t Setting_time_out = 0;

char rfid_temp[16];
char cal_buf[10] = {0,};
uint32_t current_time=0;

extern tm_struct Now;

volatile uint16_t adc_ret;
float cal_factor = 0;
float cal_cnt = 0;
void loop(void)
{
    static volatile uint32_t time_cnt = 0;
    uint32_t loop_time=0,view_time=0,parse_time = 0;

    basic_mode();
    Stable_run_flag = 1;
	
	char buf[30] ={0,};
	uint32_t cal_time = 0;
	Water_pcnt1_clear();	
	Water_pcnt2_clear();
   
	while(1)
    {   
        current_time = millis();
       
		if(calibration_mode == 0)
		{
			if((int32_t)(current_time - loop_time) >= 0)
			{
				loop_time = current_time + MAIN_LOOP_TIME;
					   
				loop_main(); //watchdog
				
				if(Setting_enable == 0 ) 
				  intake_Run(current_time);		//take a water function
				else
				{
				  //setting mode		
				}
		
				Sub_lora_process(); //parse the data from a lora
			}

			if((int32_t)(current_time - parse_time) >= 0)
			{
				parse_time = current_time + MAIN_LOOP_TIME;
				
				if(uart3_is_empty() == 0)
			  		parse_rfid(uart3_dequeue());  //RFID 		
				else
					rfid = 9999;
			}
			
			if((int32_t)(current_time - view_time) >= 0)
			{ 
			  view_time = current_time + VIEW_LOOP_TIME;
			  R_IR_check();	  //return a IR sensor result
			  R_setting_menu(); 	
			  Now = read_RTC(); //read a rtc value
			}	
			
			parse_lora_packet(); //send a data to Lora
			
			W_Intake.water_temp = Detect_Temp_Data(ADCConvertedValue);
                        
                        if(send_flag == 1)
                        {
                          error_st.status = 0x01;
                          error_st.rf_id = rfid;
                          Send_error_packet(&error_st);
                        }
		}
       
		if(calibration_mode >= 500)
		{
			
			if(((uint32_t)current_time - cal_time) >= 0)
			{
				cal_time = current_time + CAL_LOOP_TIME;
				TM_HD44780_Puts(0,0,"Cal Mode ");
				
				cal_cnt = Water_pcnt1_read();
				cal_factor = (float)(1000/cal_cnt);
				sprintf(buf, "fact %3.2f         ",cal_factor);
				TM_HD44780_Puts(0,1,buf);
				if(SW3 == 0)
				{
					Weight.factor = cal_factor;
					sprintf(cal_buf,"%4.2f",cal_factor);
					FLASH_Unlock();
					FLASH_ErasePage((uint32_t)0x0800FC00);
					for(uint8_t i = 0; i < sizeof(cal_buf); i++)
						FLASH_ProgramHalfWord((uint32_t)0x0800FC00 + (i*2),(uint16_t)cal_buf[i]);
					
					FLASH_Lock();					
					calibration_mode = 0;
				}
			}
			
			if(!R_IR_check2())
			{	
			FET_value[1] = 80;
			FET_value[2] = 80;
			FET_sum[1] += FET_value[1];
			}
		}
	}
}

uint8_t R_IR_check()
{
	//적외선 센서 입력 (소 검출시)
	return Input_value[0];
}

uint8_t R_IR_check2()
{
	//적외선 센서 입력 (소 검출시)
	return Input_value[1];
}

uint8_t R_MaxWater_check()
{
    static int ret = 0;
    
    if (ret == 0)
    {
        if (Stable_weight_ch1 > 1000)
        {
          ret = 1;
        }
    }
    else
    {
        if (Stable_weight_ch1 < 900)
        {
          ret = 0;
        }
    }
    
    return ret;
}

uint16_t R_rfid_check(void)
{
	if(rfid_is_empty() == 0)
	{
		send_rfid = rfid_dequeue();
		return send_rfid;
	}
	else return RFID_UNKNOWN;
}

void R_rfid_clear(void)
{
	uint8_t loop = 1;
	while(loop)
	{
		if(rfid_is_empty() == 0)
		{
			send_rfid = rfid_dequeue();
		}
		else
		{
			loop = 0;	
		}
	}
}


void R_setting_menu(void)
{
	static uint16_t cnt_time = 0;
	static uint8_t view_flag = 0;

		if(cnt_time++>=30) {
			cnt_time = 0;

			if(view_flag == 0) view_flag = 1; 
			else if (view_flag== 1) view_flag = 2;
			else if (view_flag== 2) view_flag = 0; 			
		}
	
		if(SW_L[0] == 1 )  
		{
			SW_L[0] = 0;
			SW_S[0] = 0;
			
			if(Setting_time_out == 0)  
			{
				 Setting_enable = 1; 
			}
		}

		if(Setting_enable == 0 ) 
		{

		} else { 
			setting_mode();
		}		
}  

  int main(void)
{  
    setup();
    setting();

    loop();
}
















