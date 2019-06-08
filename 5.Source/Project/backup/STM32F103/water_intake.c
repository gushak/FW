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
	FET_value[0] = 9999;	//60�ʰ� ��ħ 
	//���� ���� �������ֵ��� 	
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
//���� �ʱ�ȭ�Ҷ� ���� �ݱ� �ֱ�
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
	
	//���� �������� 
	ch_check_init();
	
	switch(Oper_Cnt)     
	{
		
		case OPER_INIT:
			
			oper_water_wait_cnt = 0;
			Water_pcnt1_clear();	//��ī��Ʈ �ʱ�ȭ �ϱ� 
			Water_pcnt2_clear();
			
			
			rfid = 0;
			In_rfid = 0;
			start_weight = 0;
			
			GATE = 0;		//���ݱ�
			
			R_rfid_clear();
			Oper_Cnt = OPER_WATER_WAIT;
			
			break;
		// ������ ������ �ִ� �������� Ȯ���ϰ� ��� ��
		// �߰� ���������� Ȯ�� �ϴ� �κ� �ʿ���
		case OPER_WATER_WAIT:
						
			//10mS������ �ʹ� �۴�!!
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
						//�����ð� �̻� ��ٷȴٰ�!!
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
				GATE = 1000;    //���� 10�ʰ� ���� ���´�
				//  �ʱ� ���� �ϰ� �������� �Ѿ��!!				
				Water_pcnt1_clear();	//��ī��Ʈ �ʱ�ȭ �ϱ� 
				Water_pcnt2_clear();
				
				In_rfid = rfid;
				Oper_Cnt = OPER_INCOW;
				
				memset((void *)&W_Intake, 0, sizeof(W_Intake));
				Intake_head = Intake_Head(); //mram;  
				W_Intake.index = Intake_head; 
				W_Intake.date = Get_Date();
				W_Intake.enter_time = GetTime_min();		//����ð�				
				W_Intake.rfid_num = rfid;
				W_Intake.feeder_num = CON_ADDRESS;

				W_Intake_Time = RTC_GetCounter();		//�ð� ����. �ʿ����!@@ ��.,��;

				start_weight = Stable_weight;
				W_Intake.water_temp = start_weight; //����
				
				Incow_rfid_cnt = CON_COWIN_RFID_CNT;
				Incow_ir_cnt = CON_COWIN_IR_CNT;
			}			
			break;			
			
		case OPER_INCOW:		
			//���� ���� �Ұ� 			
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
					//�ٸ���ü�� ���� �ٷ� �ϱ� !! 
					// ����ü�� 
					Oper_Cnt = OPER_COMPLTE;
				}
				else
				{
					GATE = 1000;    //���� 60�ʰ� ���� ���´�
					//���������� ������ 
				}
			}
			
			
			//�� ����� �ֱ������� ���� ���� �ȵǸ� �ٷ� �ݰ� ������
			if((R_IR_check() == 0) && (R_IR_check2() == 0))
			{				
				
				GATE = 0;	//����Ʈ �ݱ� !! 
				//�����̶� �����ϱ� !! �Ҿ��� 
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
						//�ٸ���ü�� ���� �ٷ� �ϱ� !! 
						// ����ü�� 
						Oper_Cnt = OPER_COMPLTE;
					}
					else
					{
						if((R_IR_check() == 1) || (R_IR_check2() == 1))
						{	
							//�����Ұ� �� ������ 
							GATE = 1000;    //���� 60�ʰ� ���� ���´�								
							Incow_ir_cnt = CON_COWIN_IR_CNT;
							Incow_rfid_cnt = CON_COWIN_RFID_CNT;
							Oper_Cnt = OPER_INCOW;
						}
					}
				}
				
			}
			else
			{
				//�����ϱ� 
				Oper_Cnt = OPER_COMPLTE;
			}
			
			//������ ���!!! 
			sprintf(buf, "RE_ENTER_WAIT   ");	
			TM_HD44780_Puts(0 ,0, buf); 			
			sprintf(buf, "C:%04d, R:%04d  ",In_rfid, reenter_cnt);	
			TM_HD44780_Puts(0 ,1, buf);		
			
			break;
			
			
			
		case OPER_COMPLTE:
			//��ü ����
			
		W_Intake.water_cnt1 = Water_pcnt1_read();	//����� ����!!		
		W_Intake.water_cnt2 = Water_pcnt2_read();	//����� ����!!				
		W_Intake.intake_weight = weight;		//��������� ���� �׳� �ִ´� 		
		//2017-04-03 _�Ф� �Ʒ� ���� �ʿ� 
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
		
		case 0:	// ���� �� Ĩ��ȣ�� ����, �Ǵ� Ĩ�� ���� �Ǿ��� ��� �������� �����ϰ� �ʱ�ȭ
		
			rfid = R_rfid_check();	
			TM_HD44780_Puts(0 ,0, "RUN:RFID&IR WAIT"); 			
			sprintf(buf, "RFID>%04d  ", rfid);	
			TM_HD44780_Puts(0 ,1, buf);
                

			if((R_IR_check() == 1 && rfid != RFID_UNKNOWN) || rfid != RFID_UNKNOWN) 
			{	
				//  ���� 
				
                                FET_value[0] = 6000;    //���� 60�ʰ� ���� ���´�
                        
				
                               
                                                                                              
                                  #ifdef WEIGHT_SINGLE
                                  #else
                                  sprintf(buf, "CH>%d            ", Stable_ch_flag + 1);	
                                  TM_HD44780_Puts(0 ,1, buf);
                                  #endif
				
                                  memset((void *)&W_Intake, 0, sizeof(W_Intake));
                                  Intake_head = Intake_Head(); //mram;  
				
                                  W_Intake.index = Intake_head; 
                                  W_Intake.date = Get_Date();
                                  W_Intake.enter_time = GetTime_min();		//����ð�				
                                  W_Intake.rfid_num = rfid;
                                  W_Intake.feeder_num = CON_ADDRESS;
				
                                  W_Intake_Time = RTC_GetCounter();		//�ð� ����. �ʿ����!@@ ��.,��;
				
                                  start_weight = Stable_weight;
                                  W_Intake.water_temp = start_weight; //����
				
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
                                  //test �� ����
                                 
                                  Oper_Cnt = 1;
                                  in_flag = 0;
			}
		break;
		
	case 1: 
			
		if(R_IR_check() == 0)
		{
			//�� ����
			Oper_Cnt = 2;
			//gate_close_flag = 1;

			//Rfid_check_timeover = RFID_TIMEOVER;
			//gate_waiting_time = 0;	//����ð��� �ٷ�
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
		//���뷮 �����		
		
		break;


	case 2:  

		v_time = (int64_t)W_Intake_Time_visit - (int64_t)W_Intake_Time;			//�ӹ� �ð� ��� , �ʴ���
		
                
                if(v_time < 0) v_time= 0;
		
		W_Intake.visit_time = v_time;		// �ӹ� �ð��̰� ������ S
		
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
		// ������� 
		//�̹��������� ������ ���ü��� ���µ�??? ����ä���� �ϰŵ�!!
		
		
		//20170328 �������� ����ä��� ������� ����..���� ������
		
		
		//W_Intake.water_cnt1 = Water_pcnt1_read();
		W_Intake.water_cnt2 = weight;	//����� ����!!		
		W_Intake.intake_weight = weight;		//��������� ���� �׳� �ִ´� 
		
		//2017-04-03 _�Ф� �Ʒ� ���� �ʿ� 
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





