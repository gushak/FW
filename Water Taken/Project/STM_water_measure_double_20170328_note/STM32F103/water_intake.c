#include "hw_control.h"
#include "water_intake.h"

//> struct
WATER_ITAKE_TABLE_STRUCT W_Intake;
uint32_t W_Intake_Time;
uint32_t W_Intake_Time_visit;

extern Wight_struct Parse_st;
extern volatile char Wbuf[100];

uint16_t start_delay_time=0;
volatile uint8_t  Oper_Cnt=0;		 

extern volatile uint8_t INPUT1_check_flag; 

extern uint32_t RFID_rx_point_tail; 
uint16_t id_cnt = 0;
extern volatile uint16_t send_rfid; 
extern uint32_t pcnt, pcnt2;

volatile uint8_t  water_fill_delay = 0; 
volatile uint16_t gate_waiting_time = 0;
volatile uint16_t Rfid_check_timeover = 0;
volatile uint16_t water_max_wait_time = 0;
volatile uint16_t Stable_weight = 0, weight = 0;
extern volatile uint16_t Intake_page_num;

#define WATER_BOX_SINGLE

volatile uint8_t flag_flag_flag = 0;


void ch_check_init()
{
#ifdef WEIGHT_SINGLE
	Stable_weight = Stable_weight_ch1;
	weight = weight_ch1;
#else
	if(flag_flag_flag == 0)
	{
		Stable_weight = Stable_weight_ch1;
		weight = weight_ch1;
	} else {
		Stable_weight = Stable_weight_ch2;
		weight = weight_ch2;
	}
#endif
}

extern volatile uint8_t FULL_Water_Error;
extern volatile uint16_t Full_water_check_cnt ;

//메인 초기화할때 모터 닫기 넣기
void intake_Run()
{
	static uint16_t Intake_head = 0;
	char buf[17];
	static uint16_t rfid = 0;
	static uint8_t in_flag = 0;
	static uint16_t start_weight = 0;
	static uint16_t Gap_time = 0;
	int64_t v_time = 0;  //visit_time	
	uint8_t gate_close_flag = 0;	
	static uint8_t RFID_on_IR_status_flag = 0; 

	if(start_delay_time != 0) return; 
	//if(dStable_water_delay !=0) return; 
	
	ch_check_init();
	
	switch(Oper_Cnt)
	{
	case 0:	
		//적외선 감지 & 만수
		
		//2017-01-8 추가 김현형, CON_Water_eat 설정에 따라서~!!
		if(R_MaxWater_check() == 1 || CON_Water_eat == 1)
		{
			//만수이면 문열어
			Gate_open(0);	//문을 열어 놓는다 
			
			TM_HD44780_Puts(0 ,0, "RUN:RFID&IR WAIT"); 			
			sprintf(buf, "W_Weight>%05d  ", Stable_weight);	
			TM_HD44780_Puts(0 ,1, buf);

			//이부분은 고민되는게 센서 입력들어오면 일단 ㄴ문을 닫았다가
			//리더가 인식되면 그때 다시 열어버리는 방법을 고민해보자!!
			//왜냐하면 인식도 안되었는데 물먹으면 정확한 측정이 어렵다 ??
			if(R_IR_check() == 1) {
				//적외선 센서 입력시 소입장을 간주하고 문이 닫혀있다면 문수이므로 문을 연다!!
				//열어라~~~!!! 참깨!!
				//뿅~
				//Gate_open(GATE_motion_time);		//설정값에 따름
			}
			//소입장용 ID대기
			rfid = R_rfid_check();	
	
			if(rfid != 0 )
			{	
				Gate_open(GATE_motion_time);
				
				if(R_IR_check() == 1) {
					//리더 체크시 적외선 센서 입력됨
					RFID_on_IR_status_flag = 1; 					
				}
				else RFID_on_IR_status_flag = 0; 

				sprintf(buf, "RUN->OK:%04d    ", rfid);	
				TM_HD44780_Puts(0 ,0, buf); 	

#ifdef WEIGHT_SINGLE
#else
				sprintf(buf, "CH>%d            ", Stable_ch_flag + 1);	
				TM_HD44780_Puts(0 ,1, buf);
#endif
				
				memset((void *)&W_Intake, 0, sizeof(W_Intake));
				Intake_head = Intake_Head(); //mram;  
				
				W_Intake.index = Intake_head; 
				W_Intake.date = Get_Date();
				W_Intake.enter_time = GetTime_min();		//입장시간				
				W_Intake.rfid_num = rfid;
				W_Intake.feeder_num = WATER_ADDRESS;
				
				W_Intake_Time = RTC_GetCounter();		//시간 계산용. 필요없당!@@ ㅡ.,ㅡ;
				
				//if(in_flag ==0)
				{
					start_weight = Stable_weight;
					W_Intake.water_temp = start_weight; //저울
				}
				
#ifdef WEIGHT_SINGLE
				Water_pcnt1_clear();
				Water_pcnt2_clear();
#else
				if(flag_flag_flag == 0)
				{
					Water_pcnt1_clear();	
				} else {
					Water_pcnt2_clear();	
				}
#endif
				
				//test 후 삭제
				start_delay_time = R_waiting_time;		//입장시 잠시 대기 
				Oper_Cnt = 1;
				in_flag = 0;

				Gap_time = 50;	
			}			
			
		}		
		else	{			
			//만수가 아닐경우
			if(FULL_Water_Error != 0)
			{
				TM_HD44780_Puts(0 ,0, "ERR:FILLTIMEOVER"); 	
				Gate_open(0);	//문을 열어 놓는다 
			} 
			else
			{
				sprintf(buf, "RUN:WaterX:%05d",Full_water_check_cnt );	
				TM_HD44780_Puts(0 ,0, buf); 		
				Gate_close();	//문을 닫고 물채움 !!
			}
			
			sprintf(buf, "W_Weight>%5d  ", weight);	
			TM_HD44780_Puts(0 ,1, buf);
			
			R_rfid_clear();
		}	
		break;
	
		
	case 1: 
		
		rfid = R_rfid_check();	// 감지된  id번호		
		if(rfid != 0) {
			
			if(W_Intake.rfid_num != rfid)
			{
				//다른소 감지-정산
				Oper_Cnt = 2;
				gate_close_flag = 1;

				Rfid_check_timeover = RFID_TIMEOVER;
				gate_waiting_time = 0;	//정산시간은 바로
				in_flag = 0;
			}
			else
			{
				//기존소 감지
				in_flag = 1;		
				Rfid_check_timeover = RFID_TIMEOVER;
			}		
		}
		
#ifdef WEIGHT_SINGLE
		W_Intake.water_cnt1 = Stable_weight;
		W_Intake.water_cnt2 = 0;
#else
		if(flag_flag_flag == 0)
		{
			W_Intake.water_cnt1 = Water_pcnt1_read();
		} else {
			W_Intake.water_cnt2 = Water_pcnt2_read();
		}
#endif 
		
		W_Intake_Time_visit = RTC_GetCounter();	
		
		sprintf(buf, "EAT>%03d,%03d,%03d ", Water_pcnt1_read()%999, rfid, Rfid_check_timeover);	
		TM_HD44780_Puts(0 ,0, buf);
		
		sprintf(buf, "DRINK..ing>%5d", Stable_weight);	
		TM_HD44780_Puts(0 ,1, buf);
		
		if(RFID_on_IR_status_flag !=0) {				
			if(R_IR_check() == 0)	// 적외선 센서에서 소가 나갔을 경우 
			{
				if(Gap_time != 0) 
				{
					Gap_time--;
				} else {						
					if(in_flag == 1)
					{
						in_flag = 0; 
						gate_close_flag = 1; 	//문닫는다!!!						
						gate_waiting_time = GATE_waiting_time;	//대시시간	
						//문닫음
						//RFID_on_IR_status_flag = 0; 
						Oper_Cnt = 2;
					}				
					//Gate_close();		
					//문닫는다!!
				}
//				sprintf(buf, "RUN->CLOSE  %03d ", gate_waiting_time);	
//				TM_HD44780_Puts(0 ,0, buf);
		
			}
			else
			{
				Gap_time = 50;
				//Gap_time = 
			}
		}
		
		if(Rfid_check_timeover == 0)
		{
			//리더값이 일정시간 동안 안들어왔을 경우
			if(in_flag == 1)
			{
				in_flag = 0; 
				gate_close_flag = 1;					
				gate_waiting_time = GATE_waiting_time;	//대시시간	
				//문닫음
				
				Oper_Cnt = 2;
			}		
			
			sprintf(buf, "RUN->RFID_T_OVER");	
			TM_HD44780_Puts(0 ,0, buf);
			
		}
		
		if(gate_close_flag != 0) 
		{
			Gate_close();		
			start_delay_time = R_waiting_time;		//입장시 잠시 대기 
		}
				
		
		//섭취량 모니터		
		
		
		break;


	case 2:  

		v_time = (int64_t)W_Intake_Time_visit - (int64_t)W_Intake_Time;			//머문 시간 계산 , 초단위
		if(v_time < 0) v_time= 0;
				
		W_Intake.visit_time = v_time;		// 머문 시간이고 단위는 S		
		
		water_max_wait_time = 0;
		Gap_time = 50;
		Oper_Cnt = 3;
		
		break;
		
		
	case 3:
		
		if(gate_waiting_time == 0)
		{
			
			Oper_Cnt = 4; // 정산대기 

		} else {
			
			Gate_close();		
			//소가 다시 들어올수 있음-정산대기 !!		
			rfid = R_rfid_check();	
			if(rfid != 0) {
				if(W_Intake.rfid_num != rfid)
				{
					//다른소 감지 정산
					Oper_Cnt = 4;

					sprintf(buf, "C_Weight>%5d  ", weight);	
					TM_HD44780_Puts(0 ,1, buf);
				}
				else
				{
					in_flag = 1;
					//기존소 감지
					Oper_Cnt = 1;		
					Rfid_check_timeover = RFID_TIMEOVER;

					Gate_open(GATE_motion_time);	

					sprintf(buf, "C_Weight>%5d  ", weight);	
					TM_HD44780_Puts(0 ,1, buf);
					
				}		
			}
			
			sprintf(buf, "RUN->CLOSE  %03d ", gate_waiting_time);	
			TM_HD44780_Puts(0 ,0, buf);
		}
		
		break;
		
	case 4:
		// 만수대기 
		//이번버전에서 만수가 나올수가 없는데??? 물안채워야 하거든!!
		
		
		//20170328 정산이후 만수채우고 대기하지 않음..무게 측정함
		
		
		//W_Intake.water_cnt1 = Water_pcnt1_read();
		W_Intake.water_cnt2 = weight;	//종료시 무게!!		
		W_Intake.intake_weight = weight;		//종료시점에 무게 그냥 넣는다 
		
		//2017-04-03 _ㅠㅠ 아래 수정 필요 
		W_Intake.status = 0x01;
		
		sprintf(buf, "C_Weight>%5d  ", W_Intake.intake_weight);	
		TM_HD44780_Puts(0 ,1, buf);
		
		SPI_FLASH_PageWrite((uint8_t *)&W_Intake,Mram_intake_address[Intake_head], sizeof(W_Intake));
		Intake_head_increment();

		Oper_Cnt = 0;
		
		sprintf(buf, "RUN->INTAKE_END " );
		TM_HD44780_Puts(0 ,0, buf)	;
		
		memset((void *)&W_Intake, 0, sizeof(W_Intake));
		
		start_delay_time = 500;	
		
#if 0
		if(R_MaxWater_check() == 1) 
		{
			
#ifdef WEIGHT_SINGLE
			W_Intake.water_cnt1 = Water_pcnt1_read();
			W_Intake.water_cnt2 = Water_pcnt2_read();
#else
			if(flag_flag_flag == 0)
			{
				W_Intake.water_cnt1 = Water_pcnt1_read();
			} else {
				W_Intake.water_cnt2 = Water_pcnt2_read();
			}
#endif
			
			W_Intake.status = 0x01;

			

#ifdef WEIGHT_SINGLE
			W_Intake.intake_weight = Stable_weight;		//종료시점에 무게 그냥 넣는다 
			
			sprintf(buf, "C_Weight>%5d  ", W_Intake.intake_weight);	
			TM_HD44780_Puts(0 ,1, buf);
#else
			W_Intake.water_temp = start_weight - Stable_weight;
			flag_flag_flag^= 1;
			
			sprintf(buf, "C_Weight>%5d  ", W_Intake.water_temp);	
			TM_HD44780_Puts(0 ,1, buf);
#endif			
			
			
			SPI_FLASH_PageWrite((uint8_t *)&W_Intake,Mram_intake_address[Intake_head], sizeof(W_Intake));
			Intake_head_increment();
	
			Oper_Cnt = 0;
			
			sprintf(buf, "RUN->INTAKE_END " );
			TM_HD44780_Puts(0 ,0, buf)	;
			
			memset((void *)&W_Intake, 0, sizeof(W_Intake));
			
			start_delay_time = 500;	
			
		} 
		else 
		{
			if(water_max_wait_time ++ >= WATER_MAX_wait_time)
			{
#ifdef WEIGHT_SINGLE
				W_Intake.water_cnt1 = Water_pcnt1_read();
				W_Intake.water_cnt2 = Water_pcnt2_read();
#else
				if(flag_flag_flag == 0)
				{
					W_Intake.water_cnt1 = Water_pcnt1_read();
				} else {
					W_Intake.water_cnt2 = Water_pcnt2_read();
				}
#endif
				
				W_Intake.status = DEVICE_WATER_MAX_TIMEOVER | 0x01;	
				
				
#ifdef WEIGHT_SINGLE
			W_Intake.intake_weight = Stable_weight;		//종료시점에 무게 그냥 넣는다 
			
			sprintf(buf, "C_Weight>%5d  ", W_Intake.intake_weight);	
			TM_HD44780_Puts(0 ,1, buf);
#else
			W_Intake.water_temp = start_weight - Stable_weight;
			flag_flag_flag^= 1;
			
			sprintf(buf, "C_Weight>%5d  ", W_Intake.water_temp);	
			TM_HD44780_Puts(0 ,1, buf);
#endif
			
				
				SPI_FLASH_PageWrite((uint8_t *)&W_Intake,Mram_intake_address[Intake_head], sizeof(W_Intake));
				Intake_head_increment();
				
		
				Oper_Cnt = 0;
				
				sprintf(buf, "RUN->WTIMEOVER " );
				TM_HD44780_Puts(0 ,0, buf)	;			
				
				memset((void *)&W_Intake, 0, sizeof(W_Intake));
				start_delay_time = 500;
			}			
			
			sprintf(buf, "RUN->%05d,%05d", Water_pcnt1_read(), Water_pcnt2_read());
			TM_HD44780_Puts(0 ,0, buf);
			
		}
#endif
		
		//타임오버~!!
		//작성할것
		
		break;

	default:
		break;
		
	}
}





