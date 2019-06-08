#include "hw_control.h"
#include "water_intake.h"

//> struct
WATER_ITAKE_TABLE_STRUCT W_Intake = {0,};
uint32_t W_Intake_Time;
uint32_t W_Intake_Time_visit;

extern Wight_struct Parse_st;
extern volatile char Wbuf[100];

volatile uint8_t  Oper_Cnt=0;		 

extern volatile uint8_t INPUT1_check_flag; 

extern uint32_t RFID_rx_point_tail; 
uint16_t id_cnt = 0;
extern volatile uint16_t send_rfid; 
extern uint32_t pcnt, pcnt2;

volatile uint8_t  water_fill_delay = 0; 
//volatile uint16_t gate_waiting_time = 0;
//volatile uint16_t Rfid_check_timeover = 0;
volatile uint16_t water_max_wait_time = 0;
volatile uint16_t Stable_weight = 0, weight = 0;
extern volatile uint16_t Intake_page_num;
extern volatile uint16_t FET_value[10];

#define WATER_BOX_SINGLE

volatile uint8_t flag_flag_flag = 0;

void Gate_open(uint16_t m_cnt)
{
    static int isOpen;
    if (!isOpen)
    {
      FET_value[0] = 0;
      isOpen = 0;
    }
}

void Gate_close()
{
	FET_value[0] = 9999;	//60초간 닫침 
	//에어 끈다 문열려있도록 	
}

void Gate_open_Delay(uint16_t m_cnt)
{
	FET_value[0] = 0;

	
}

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

#define OPER_INIT								0
#define OPER_WATER_WAIT	1
#define OPER_COW_WAIT			2
#define OPER_INCOW						3
#define OPER_WATER						4
#define OPER_NEXT_WAIT		5
#define OPER_COMPLTE			6

volatile uint16_t Run_wait = 0;
//메인 초기화할때 모터 닫기 넣기
void intake_Run()
{
	static uint16_t Intake_head = 0;
	char buf[17];
	static uint16_t rfid = 0;	
	static uint16_t In_rfid = 0;
	static uint16_t Incow_ir_cnt = 0;
	static uint16_t Incow_rfid_cnt = 0;
	static uint16_t reenter_cnt = 0;
	static uint16_t start_weight = 0;	
	static uint32_t Old_Watercnt = 0;
	static uint16_t Water_stable_cnt = 0;
	uint32_t Watercnt = 0;
	int32_t diff_Watercnt = 0;
	static uint16_t xxx_cnt = 0;
	static uint16_t oper_water_wait_cnt = 0;

	if(Run_wait != 0) {	Run_wait--;	return; };
	
	//무게 가져오기 
	ch_check_init();
	
	switch(Oper_Cnt)     
	{
		
		case OPER_INIT:
			
			oper_water_wait_cnt = 0;
			Water_pcnt1_clear();	//물카운트 초기화 하기 
			Water_pcnt2_clear();
			
			
			rfid = 0;
			In_rfid = 0;
			start_weight = 0;
			
			GATE = 0;		//문닫기
			
			R_rfid_clear();
			Oper_Cnt = OPER_WATER_WAIT;
			
			break;
		// 입자을 받을수 있는 상태인지 확인하고 대기 함
		// 중간 정산중인지 확인 하는 부분 필요함
		case OPER_WATER_WAIT:
						
			//10mS단위는 너무 작다!!
			if(oper_water_wait_cnt ++ >= 10)
			{
				oper_water_wait_cnt = 0;
				
				Watercnt = Water_pcnt1_read();
				diff_Watercnt = Watercnt - Old_Watercnt;
				Old_Watercnt = Watercnt;

				
				if(abs(diff_Watercnt) < CON_WATER_WAIT_DIFF)
				{
					Water_stable_cnt++;
					if(Water_stable_cnt >= CON_WATER_WAIT_TIME) 
					{
						//일정시간 이상 기다렸다가!!
						Water_stable_cnt = 0;
						Oper_Cnt = OPER_COW_WAIT;
						xxx_cnt = 0;
					}				
				}
				else
				{
					Water_stable_cnt = 0;
				}
				
				sprintf(buf, "Water empty:%04d", Water_stable_cnt);	
				TM_HD44780_Puts(0 ,0, buf); 			
				sprintf(buf, "C1:%04d,Dif:%04d", Watercnt,diff_Watercnt);	
				TM_HD44780_Puts(0 ,1, buf);	
			}

			break;
			
		case OPER_COW_WAIT :
			
			rfid = R_rfid_check();	
			xxx_cnt++;
			
			sprintf(buf, "Cow Wait : %04d ", Water_pcnt1_read());	
			TM_HD44780_Puts(0 ,0, buf); 		
			memset((void *)&buf, 0, sizeof(buf));
			sprintf(buf, "R:%03d,1:%01d,2:%01d,%02d", In_rfid, R_IR_check(), R_IR_check2(), (xxx_cnt / 100));	
			TM_HD44780_Puts(0 ,1, buf);
			
			if(rfid != RFID_UNKNOWN && ((R_IR_check() == 1) || (R_IR_check2() == 1))) 
			{
				GATE = 1000;    //문을 10초간 열어 놓는다
				//  초기 설정 하고 다음으로 넘어가기!!				
				Water_pcnt1_clear();	//물카운트 초기화 하기 
				Water_pcnt2_clear();
				
				In_rfid = rfid;
				Oper_Cnt = OPER_INCOW;
				
				memset((void *)&W_Intake, 0, sizeof(W_Intake));
				Intake_head = Intake_Head(); //mram;  
				W_Intake.index = Intake_head; 
				W_Intake.date = Get_Date();
				W_Intake.enter_time = GetTime_min();		//입장시간				
				W_Intake.rfid_num = rfid;
				W_Intake.feeder_num = CON_ADDRESS;

				W_Intake_Time = RTC_GetCounter();		//시간 계산용. 필요없당!@@ ㅡ.,ㅡ;

				start_weight = Stable_weight;
				W_Intake.water_temp = start_weight; //저울
				
				Incow_rfid_cnt = CON_COWIN_RFID_CNT;
				Incow_ir_cnt = CON_COWIN_IR_CNT;
			}			
			break;			
			
		case OPER_INCOW:		
			//급이 도중 소가 			
			if(Incow_rfid_cnt != 0) Incow_rfid_cnt--;
			
			sprintf(buf, "IN:%04d,%03d,%03d",In_rfid,Incow_ir_cnt, Incow_rfid_cnt);	
			TM_HD44780_Puts(0 ,0, buf); 			
			sprintf(buf, "W%04d,P%03d,P%03d ",weight, Water_pcnt1_read(), Water_pcnt2_read());	
			TM_HD44780_Puts(0 ,1, buf);			
				
			rfid = R_rfid_check();
			if(rfid != RFID_UNKNOWN)
			{
				In_rfid = rfid;				
				Incow_rfid_cnt = CON_COWIN_RFID_CNT;
				if(W_Intake.rfid_num != rfid)
				{
					//다른개체면 정산 바로 하기 !! 
					// 정상개체면 
					Oper_Cnt = OPER_COMPLTE;
				}
				else
				{
					GATE = 1000;    //문을 60초간 열어 놓는다
					//정상적으로 섭취중 
				}
			}
			
			
			//소 입장시 주기적으로 센서 갑지 안되면 바로 닫고 정산대기
			if((R_IR_check() == 0) && (R_IR_check2() == 0))
			{				
				
				GATE = 0;	//게이트 닫기 !! 
				//퇴장이라 감지하기 !! 소없음 
				Oper_Cnt = OPER_NEXT_WAIT;
				reenter_cnt = COM_RE_ENTER_CNT;
						
			}
			else
			{
				GATE = 1000;
				
			}
			
			
			break;
			
		case OPER_NEXT_WAIT:
	
			
			if(reenter_cnt != 0)
			{				
				reenter_cnt--;
				
				rfid = R_rfid_check();
				if(rfid != RFID_UNKNOWN)
				{
					In_rfid = rfid;	
					if(W_Intake.rfid_num != rfid)
					{
						//다른개체면 정산 바로 하기 !! 
						// 정상개체면 
						Oper_Cnt = OPER_COMPLTE;
					}
					else
					{
						if((R_IR_check() == 1) || (R_IR_check2() == 1))
						{	
							//기존소가 또 들어오면 
							GATE = 1000;    //문을 60초간 열어 놓는다								
							Incow_ir_cnt = CON_COWIN_IR_CNT;
							Incow_rfid_cnt = CON_COWIN_RFID_CNT;
							Oper_Cnt = OPER_INCOW;
						}
					}
				}
				
			}
			else
			{
				//정산하기 
				Oper_Cnt = OPER_COMPLTE;
			}
			
			//다음소 대기!!! 
			sprintf(buf, "RE_ENTER_WAIT   ");	
			TM_HD44780_Puts(0 ,0, buf); 			
			sprintf(buf, "C:%04d, R:%04d  ",In_rfid, reenter_cnt);	
			TM_HD44780_Puts(0 ,1, buf);		
			
			break;
			
			
			
		case OPER_COMPLTE:
			//개체 정산
			
		W_Intake.water_cnt1 = Water_pcnt1_read();	//종료시 무게!!		
		W_Intake.water_cnt2 = Water_pcnt2_read();	//종료시 무게!!				
		W_Intake.intake_weight = weight;		//종료시점에 무게 그냥 넣는다 		
		//2017-04-03 _ㅠㅠ 아래 수정 필요 
		W_Intake.status = 0x01;
		
		sprintf(buf, "SAVE DATA       ");	
		TM_HD44780_Puts(0 ,0, buf);

		sprintf(buf, "%04d,%04d,%04d", W_Intake.water_cnt1, W_Intake.water_cnt2, W_Intake.intake_weight);	
		TM_HD44780_Puts(0 ,1, buf);

		
		SPI_FLASH_PageWrite(
										(uint8_t *)&W_Intake,
										Mram_intake_address[Intake_head], 
										sizeof(W_Intake));
										
		Intake_head_increment();
	

		Oper_Cnt = OPER_INIT;
		
	
		memset((void *)&W_Intake, 0, sizeof(W_Intake));
		
		R_rfid_clear();
		Run_wait = 200;	
		
			break;
			
#if 0
		
		case 0:	// 센서 와 칩번호가 감지, 또는 칩이 감지 되었을 경우 입장으로 간주하고 초기화
		
			rfid = R_rfid_check();	
			TM_HD44780_Puts(0 ,0, "RUN:RFID&IR WAIT"); 			
			sprintf(buf, "RFID>%04d  ", rfid);	
			TM_HD44780_Puts(0 ,1, buf);
                

			if((R_IR_check() == 1 && rfid != RFID_UNKNOWN) || rfid != RFID_UNKNOWN) 
			{	
				//  입장 
				
                                FET_value[0] = 6000;    //문을 60초간 열어 놓는다
                        
				
                               
                                                                                              
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
                                  W_Intake.feeder_num = CON_ADDRESS;
				
                                  W_Intake_Time = RTC_GetCounter();		//시간 계산용. 필요없당!@@ ㅡ.,ㅡ;
				
                                  start_weight = Stable_weight;
                                  W_Intake.water_temp = start_weight; //저울
				
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
                                 
                                  Oper_Cnt = 1;
                                  in_flag = 0;
			}
		break;
		
	case 1: 
			
		if(R_IR_check() == 0)
		{
			//소 퇴장
			Oper_Cnt = 2;
			//gate_close_flag = 1;

			//Rfid_check_timeover = RFID_TIMEOVER;
			//gate_waiting_time = 0;	//정산시간은 바로
                        W_Intake.rfid_num = rfid;
			in_flag = 0;
		}
		else
		{
			in_flag = 1;		
			//Rfid_check_timeover = RFID_TIMEOVER;
		
			if (rfid == RFID_UNKNOWN)
			{
				rfid = R_rfid_check();
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
			
			//sprintf(buf, "EAT>%03d,%03d,%03d ", Water_pcnt1_read()%999, Water_pcnt2_read()%999, Rfid_check_timeover);	
			sprintf(buf, "EAT>%03d,%03d", Water_pcnt1_read()%999, Water_pcnt2_read()%999);	
			TM_HD44780_Puts(0 ,0, buf);
			
		//	sprintf(buf, "DRINK..ing>%5d", Stable_weight);	
			TM_HD44780_Puts(0 ,1, buf);
		}
		//섭취량 모니터		
		
		break;


	case 2:  

		v_time = (int64_t)W_Intake_Time_visit - (int64_t)W_Intake_Time;			//머문 시간 계산 , 초단위
		
                
                if(v_time < 0) v_time= 0;
		
		W_Intake.visit_time = v_time;		// 머문 시간이고 단위는 S
		
		water_max_wait_time = 0;
		Oper_Cnt = 3;
		
		break;
		
		
	case 3:
		//Gate_close();		
		Oper_Cnt = 4;

		sprintf(buf, "C_Weight>%5d  ", weight);	
		TM_HD44780_Puts(0 ,1, buf);
				
		sprintf(buf, "RUN->CLOSE");	
		TM_HD44780_Puts(0 ,0, buf);
		
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
		
                if (W_Intake.intake_weight != W_Intake.water_temp)
                {
                
                  SPI_FLASH_PageWrite(
                                  (uint8_t *)&W_Intake,
                                  Mram_intake_address[Intake_head], 
                                  sizeof(W_Intake));
                                  
                  Intake_head_increment();
                }

		Oper_Cnt = 0;
		
		sprintf(buf, "RUN->INTAKE_END " );
		TM_HD44780_Puts(0 ,0, buf)	;
		
		memset((void *)&W_Intake, 0, sizeof(W_Intake));
		
		R_rfid_clear();
		

		break;
#endif

	default:
		break;
		
	}
}





