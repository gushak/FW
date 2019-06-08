#include "hw_control.h"

#define MAIN_LOOP_TIME       10  
#define VIEW_LOOP_TIME       100 
#define CIRCLE_MAIN				10

extern uint16_t VirtAddVarTab[NumbOfVar]; 

volatile uint8_t NFC_Reader_Ok = 0;
volatile uint8_t Mode_flag = 0;
volatile uint16_t Mode_delay = 0;
volatile uint16_t FET_value[10]; 		
volatile uint8_t Stable_run_flag = 0;


void lcd_view_init(void)
{
	TM_HD44780_Puts(0,0,"Dawoon 20170927 ");
	TM_HD44780_Puts(0,1,"Water Intake    ");
	Delay(2000);
	
}

void setting(void)
{	
	lcd_view_init();
	load_eeprom();

}

volatile uint16_t Input_cnt[4];
extern volatile uint8_t Input_value[2];
extern volatile uint16_t Setting_delay;

void timer_10ms_loop(void)
{
	
	SW_check();
	
	if(Setting_delay != 0) Setting_delay--;	
	

	if(INPUT1 == 0) { 	
		if(	Input_cnt[0]++  > 2) {
			Input_value[0] = 1;
		} else Input_value[0] = 0;
		
	} else {
		Input_cnt[0] = 0;
		Input_value[0] = 0;
	}	
	
	
	if(INPUT2 == 0) 
	{
		if(	Input_cnt[1]++  > 2) 		{
			Input_value[1] = 1;
		}	else	Input_value[1] = 0;	
	} 
	else 
	{
		Input_cnt[1] = 0;
		Input_value[1] = 0;
	}
	
	if(FET_value[0] != 0) { FET_value[0]--;   FET1_ON; } else {   FET1_OFF; }	
	if(FET_value[1] != 0) { FET_value[1]--;   FET2_ON; } else {   FET2_OFF; }	
	if(FET_value[2] != 0) { FET_value[2]--;   FET3_ON; } else {   FET3_OFF; }	
	if(FET_value[3] != 0) { FET_value[3]--;   FET4_ON; } else {   FET4_OFF; }
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
	//100mS마다 실행되는 타이머 루틴
	
	if(Mode_delay != 0) Mode_delay--;
	if(water_fill_delay != 0) water_fill_delay--;
	if(Stable_water_delay != 0) Stable_water_delay --;
	
	
#ifdef WEIGHT_SINGLE
	
#else
	if(FET_FL_cnt ++ > 50)
	{
		FET_FL_cnt = 0;		
		if(Stable_ch_flag != 0) 
		{
			
			FET_value[0] = 700;
			Stable_ch_flag = 0; 
		}
		else 
		{
			
			FET_value[0] = 0;
			Stable_ch_flag = 1;		
		}
	}
#endif
	if(Stable_run_flag == 1)
	{
		//만수 관련동작하기 , 10liter 이상일시
		if(R_MaxWater_check())
		{	
			//만수일 경우 ~!!
			FET_value[2] = 20;		//만수일 경우 출력ON	온도컨트롤러 동작시키도록 !!
			
			Full_water_check_cnt = 0;	//만수가 되면 에러도 제거
			FULL_Water_Error = 0;			//만수가 되었으니까 정상 동작
                        
                        //if ((Oper_Cnt == 0) && (Input_value[1] == 0))
                        if ((Oper_Cnt == 0) && (Stable_weight_ch1 < 1700))
                        {
                          FET_value[1] = 20; //아직 만수가 아니면 물채움 
                        }
		}
		else
		{
			//2017-03-28 김현형 추가 
			if(FULL_Water_Error != 0)
			{
				FET_value[1] = 0;	//급수 솔벨브 OFF				
				
			} 
			else 
			{
				if(Oper_Cnt == 0)
				{	
					//만수가 아닌데 섭취중이 아니면 물 채움
					FET_value[1] = 20;
					
					if(Full_water_check_cnt ++ >= FULL_WATER_OVER_TIME)
					{
						Full_water_check_cnt = 0;
						//만수가 일정시간(기본20분)동안 차오르지 않으면 물이상으로 판단함
						FULL_Water_Error = 1;
						FET_value[1] = 0;	//급수 솔벨브 OFF
						
					}
					
				}
				else
				{
					Full_water_check_cnt = 0;
					FET_value[1] = 0;
				}
			}
			
			//FET_value[0] = 0;		//만숙가 아닐경우 출력 OFF 온도 CTRL동작 안시킴
		}
	}
}

//extern uint16_t gate_waiting_time;
//extern volatile uint16_t Rfid_check_timeover;
void timer_1000ms_loop(void)
{
	LED1_FL;
	//1초마다 1회 실행되는 타이머 루틴	
//	if(gate_waiting_time != 0) gate_waiting_time--;
//	if(Rfid_check_timeover != 0) Rfid_check_timeover--;
		
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
      //  TM_HD44780_Clear();
        
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

extern tm_struct Now;

void loop(void)
{
	static volatile uint32_t time_cnt = 0;
	uint32_t current_time=0, loop_time=0,view_time=0;

	basic_mode();
	Stable_run_flag = 1;
	
	while(1)
	{
		current_time = millis();
		if((int32_t)(current_time - loop_time) >= 0){
			loop_time = current_time + MAIN_LOOP_TIME;
			
			loop_main();
			
			if(Setting_enable == 0 ) 
			{
				intake_Run();
				
			}
			else
			{
				//셋팅모드
				
			}
			
			Sub_lora_process();
		}
		
		if((int32_t)(current_time - view_time) >= 0){ 
			view_time = current_time + VIEW_LOOP_TIME;

			R_IR_check();	
			R_setting_menu(); 		
			
			Now = read_RTC();
			
			Send_lora_packet();
			
		}
		
		if(uart3_is_empty() == 0)
		{
			parse_rfid(uart3_dequeue());
		}

		//parse_weight();
		//R_rfid_check(); 		
		
		
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
	//물 만수 입력 
	//return Input_value[1];
    // 물 10 Liter 이상일시
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

		} else { 	//>Setting_enable = 1
			setting_mode();
		}		
}

int main(void)
{
	setup();
	setting();

	loop();
}
















