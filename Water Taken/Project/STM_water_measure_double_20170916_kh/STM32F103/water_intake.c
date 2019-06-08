#include "hw_control.h"
#include "water_intake.h"
#include "main.h"
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
Water_Info_t Weight;

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
	} 
  else
  {
		Stable_weight = Stable_weight_ch2;
		weight = weight_ch2;
	}
#endif
}

extern volatile uint8_t FULL_Water_Error;
extern volatile uint16_t Full_water_check_cnt ;

#define OPER_INIT								0
#define OPER_WATER_WAIT	        1
#define OPER_COW_WAIT			      2
#define OPER_INCOW						  3
#define OPER_WATER						  4
#define OPER_NEXT_WAIT		      5
#define OPER_COMPLTE			      6
volatile uint16_t Run_wait = 0;
volatile uint32_t time = 0;
uint8_t buf[17];
uint32_t tick;
uint16_t rfid = RFID_UNKNOWN;	
void intake_Run(uint32_t current_time)
{
	static uint8_t complete_intake = 0;
	static uint16_t Intake_head = 0;
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
	static uint8_t Incow_re_enter = 0;
	if(Run_wait != 0) {	Run_wait--;	return; };
  
  ch_check_init();
  
	switch(Oper_Cnt)     
	{
		
		case OPER_INIT:
        
		uint8_t cnt = 0;
        oper_water_wait_cnt = 0;
      
        Water_pcnt1_clear();	
		Water_pcnt2_clear();
          
        In_rfid = 0;
        start_weight = 0;
        
        GATE = 0;		//문닫기
        
        R_rfid_clear();
        
        Oper_Cnt = OPER_WATER_WAIT;
		
        break;
		
 		case OPER_WATER_WAIT :
		
		if(!R_IR_check2())
		{
			FET_value[1] = 80;
			FET_value[2] = 80;
			FET_sum[1] += FET_value[2];
		}
        else if(R_IR_check2())
		{
			Oper_Cnt = OPER_COW_WAIT;
		}        		

			break;
			
		case OPER_COW_WAIT :
        
        xxx_cnt++;
        
        sprintf(buf, "Cow Wait : %04d ", Water_pcnt1_read());	
        TM_HD44780_Puts(0 ,0, buf); 		
        memset((void *)&buf, 0, sizeof(buf));
        sprintf(buf, "R:%03d,1:%01d,2:%01d,%02d", In_rfid, R_IR_check(), R_IR_check2(), (xxx_cnt / 100));	
        TM_HD44780_Puts(0 ,1, buf);
        
        if(rfid != RFID_UNKNOWN)
        {
            GATE = 0xFFFF;    
			
            Water_pcnt1_clear();	
            Water_pcnt2_clear();
            
            In_rfid = rfid;
            Oper_Cnt = OPER_INCOW;
            
            Intake_head = Intake_Head(); //mram;  
            W_Intake.index = Intake_head; 
            W_Intake.date = Get_Date();
            W_Intake.enter_time = GetTime_min();		//입장시간				
            W_Intake.rfid_num = rfid;
            W_Intake.feeder_num = CON_ADDRESS;
            W_Intake.enter_weight = weight;
        
            W_Intake_Time = RTC_GetCounter();		//시간 계산용. 필요없당!@@ ㅡ.,ㅡ;
            start_weight = Stable_weight;
            W_Intake.enter_weight = start_weight; //저울
            
            Incow_rfid_cnt = CON_COWIN_RFID_CNT;
            Incow_ir_cnt = CON_COWIN_IR_CNT;
            
            W_Intake.rfid_num = rfid;
			
        }			
        
        break;			
			
        case OPER_INCOW:		
        
        if(Incow_rfid_cnt != 0) Incow_rfid_cnt--; 
        
        uint16_t water = Weight.factor*Water_pcnt1_read();

        sprintf(buf, "IN:%04d,%03d,%03d",In_rfid,Incow_ir_cnt, Incow_rfid_cnt);	
        TM_HD44780_Puts(0 ,0, buf); 			
        sprintf(buf, "W%04d,P%03d,P%03d ",water, Water_pcnt1_read(), Water_pcnt2_read());	
        TM_HD44780_Puts(0 ,1, buf);				
	
          if(R_IR_check2())
          {
                  tick = Config.Value[11];
          }

          if(!tick)
          {	
                  if(!R_IR_check2())
                  {
                          FET_value[1] = 80;
                          FET_value[2] = 80;
                          tick = Config.Value[11];
                          FET_sum[1] += FET_value[2];
                  }
          }
		
		
        if(rfid != RFID_UNKNOWN)
        {
		    In_rfid = rfid;				
			GATE = 0xFFFF;
			
            if(W_Intake.rfid_num != rfid) 			
            {
				if(!R_IR_check2())
				{
					while(!R_IR_check2())
					{	
						if(!tick)
						{
							water = Weight.factor*Water_pcnt1_read();
								
							sprintf(buf, "IN:%04d,%03d,%03d",In_rfid,Incow_ir_cnt, Incow_rfid_cnt);	
							TM_HD44780_Puts(0 ,0, buf); 			
							sprintf(buf, "W%04d,P%03d,P%03d ",water, Water_pcnt1_read(), Water_pcnt2_read());	
							TM_HD44780_Puts(0 ,1, buf);				
							
							FET_value[1] = 80;
							FET_value[2] = 80;
							tick = Config.Value[11];
							FET_sum[1] += FET_value[2];
							complete_intake++;
						}

						if(complete_intake > 10)
						{
							reenter_cnt = 3000;
							Oper_Cnt = OPER_COMPLTE;
							complete_intake = 0;
							
						}
					}
				}	
			}
			else if(W_Intake.rfid_num == rfid)
			{
				while(!R_IR_check2())
				{	
					if(!tick)
					{
						water = Weight.factor*Water_pcnt1_read();
							
						sprintf(buf, "IN:%04d,%03d,%03d",In_rfid,Incow_ir_cnt, Incow_rfid_cnt);	
						TM_HD44780_Puts(0 ,0, buf); 			
						sprintf(buf, "W%04d,P%03d,P%03d ",water, Water_pcnt1_read(), Water_pcnt2_read());	
						TM_HD44780_Puts(0 ,1, buf);				
						
						FET_value[1] = 80;
						FET_value[2] = 80;
						tick = Config.Value[11];
						FET_sum[1] += FET_value[2];
					}
				}
				
				if(!Incow_rfid_cnt)
				{
					Incow_rfid_cnt = CON_COWIN_RFID_CNT;	
	
				}
			}
		}
        else if(rfid == RFID_UNKNOWN)
		{
				if(R_IR_check2())
				{
					if(!Incow_rfid_cnt)
					{
						GATE = 0;   
						Oper_Cnt = OPER_NEXT_WAIT;
						reenter_cnt = CON_COWIN_RFID_CNT;
					}
				}
          } 
		
        break;
			
		case OPER_NEXT_WAIT:
	
        if(reenter_cnt != 0)
        {				
            reenter_cnt--;
			
			if(W_Intake.rfid_num != rfid) //다른개체면 정산 바로 하기 !! 
			{
				while(!R_IR_check2() && complete_intake > 10)
				{
					if(!tick)
					{
						FET_value[1] = 80;
						FET_value[2] = 80;
						FET_sum[1] += FET_value[2];
						complete_intake++;
						
						if(complete_intake > 10 && R_IR_check2())
						{
							W_Intake.rfid_num = rfid;
							Oper_Cnt = OPER_COMPLTE;
							complete_intake = 0;
							break;
						}
					}
				}
			}
			else if(rfid != RFID_UNKNOWN)
			{
				GATE = 0xFFFF;   					
				Incow_ir_cnt = CON_COWIN_IR_CNT;
				Incow_rfid_cnt = CON_COWIN_RFID_CNT;
				Oper_Cnt = OPER_INCOW;
				Incow_re_enter = 1; 
			}
		}

		if(reenter_cnt == 0)
		{
			if(rfid == RFID_UNKNOWN)
			{
				W_Intake.rfid_num = rfid;
				Oper_Cnt = OPER_COMPLTE;
			}
		}
        
       
        sprintf(buf, "RE_ENTER_WAIT   "); //다음소 대기!!! 	
        TM_HD44780_Puts(0 ,0, buf); 			
        sprintf(buf, "C:%04d, R:%04d  ",In_rfid, reenter_cnt);	
        TM_HD44780_Puts(0 ,1, buf);
        
 	   break;
			
			
			
		case OPER_COMPLTE:
        
        W_Intake.water_cnt1 = Water_pcnt1_read();			
        W_Intake.water_cnt2 = Water_pcnt2_read();				
        W_Intake.end_weight = Weight.factor*Water_pcnt1_read();		 		
        
        W_Intake.status = 0x01;
        
        W_Intake.visit_time = GetTime_min();
        
        sprintf(buf, "SAVE DATA       ");	
        TM_HD44780_Puts(0 ,0, buf);

        sprintf(buf, "%04d,%04d,%04d", W_Intake.water_cnt1, W_Intake.water_cnt2, W_Intake.end_weight);	
        TM_HD44780_Puts(0 ,1, buf);

        
        SPI_FLASH_PageWrite((uint8_t *)&W_Intake, Mram_intake_address[Intake_head], 
                        sizeof(W_Intake));
                        
        Oper_Cnt = OPER_INIT;
        
        Recv_data_ok = 1;
        
        R_rfid_clear();
        Run_wait = 200;	
        
		break;
			
	default:
		break;
		
	}
}




