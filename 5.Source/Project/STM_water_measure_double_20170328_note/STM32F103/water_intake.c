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

//���� �ʱ�ȭ�Ҷ� ���� �ݱ� �ֱ�
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
		//���ܼ� ���� & ����
		
		//2017-01-8 �߰� ������, CON_Water_eat ������ ����~!!
		if(R_MaxWater_check() == 1 || CON_Water_eat == 1)
		{
			//�����̸� ������
			Gate_open(0);	//���� ���� ���´� 
			
			TM_HD44780_Puts(0 ,0, "RUN:RFID&IR WAIT"); 			
			sprintf(buf, "W_Weight>%05d  ", Stable_weight);	
			TM_HD44780_Puts(0 ,1, buf);

			//�̺κ��� ��εǴ°� ���� �Էµ����� �ϴ� ������ �ݾҴٰ�
			//������ �νĵǸ� �׶� �ٽ� ��������� ����� ����غ���!!
			//�ֳ��ϸ� �νĵ� �ȵǾ��µ� �������� ��Ȯ�� ������ ��ƴ� ??
			if(R_IR_check() == 1) {
				//���ܼ� ���� �Է½� �������� �����ϰ� ���� �����ִٸ� �����̹Ƿ� ���� ����!!
				//�����~~~!!! ����!!
				//��~
				//Gate_open(GATE_motion_time);		//�������� ����
			}
			//������� ID���
			rfid = R_rfid_check();	
	
			if(rfid != 0 )
			{	
				Gate_open(GATE_motion_time);
				
				if(R_IR_check() == 1) {
					//���� üũ�� ���ܼ� ���� �Էµ�
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
				W_Intake.enter_time = GetTime_min();		//����ð�				
				W_Intake.rfid_num = rfid;
				W_Intake.feeder_num = WATER_ADDRESS;
				
				W_Intake_Time = RTC_GetCounter();		//�ð� ����. �ʿ����!@@ ��.,��;
				
				//if(in_flag ==0)
				{
					start_weight = Stable_weight;
					W_Intake.water_temp = start_weight; //����
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
				
				//test �� ����
				start_delay_time = R_waiting_time;		//����� ��� ��� 
				Oper_Cnt = 1;
				in_flag = 0;

				Gap_time = 50;	
			}			
			
		}		
		else	{			
			//������ �ƴҰ��
			if(FULL_Water_Error != 0)
			{
				TM_HD44780_Puts(0 ,0, "ERR:FILLTIMEOVER"); 	
				Gate_open(0);	//���� ���� ���´� 
			} 
			else
			{
				sprintf(buf, "RUN:WaterX:%05d",Full_water_check_cnt );	
				TM_HD44780_Puts(0 ,0, buf); 		
				Gate_close();	//���� �ݰ� ��ä�� !!
			}
			
			sprintf(buf, "W_Weight>%5d  ", weight);	
			TM_HD44780_Puts(0 ,1, buf);
			
			R_rfid_clear();
		}	
		break;
	
		
	case 1: 
		
		rfid = R_rfid_check();	// ������  id��ȣ		
		if(rfid != 0) {
			
			if(W_Intake.rfid_num != rfid)
			{
				//�ٸ��� ����-����
				Oper_Cnt = 2;
				gate_close_flag = 1;

				Rfid_check_timeover = RFID_TIMEOVER;
				gate_waiting_time = 0;	//����ð��� �ٷ�
				in_flag = 0;
			}
			else
			{
				//������ ����
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
			if(R_IR_check() == 0)	// ���ܼ� �������� �Ұ� ������ ��� 
			{
				if(Gap_time != 0) 
				{
					Gap_time--;
				} else {						
					if(in_flag == 1)
					{
						in_flag = 0; 
						gate_close_flag = 1; 	//���ݴ´�!!!						
						gate_waiting_time = GATE_waiting_time;	//��ýð�	
						//������
						//RFID_on_IR_status_flag = 0; 
						Oper_Cnt = 2;
					}				
					//Gate_close();		
					//���ݴ´�!!
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
			//�������� �����ð� ���� �ȵ����� ���
			if(in_flag == 1)
			{
				in_flag = 0; 
				gate_close_flag = 1;					
				gate_waiting_time = GATE_waiting_time;	//��ýð�	
				//������
				
				Oper_Cnt = 2;
			}		
			
			sprintf(buf, "RUN->RFID_T_OVER");	
			TM_HD44780_Puts(0 ,0, buf);
			
		}
		
		if(gate_close_flag != 0) 
		{
			Gate_close();		
			start_delay_time = R_waiting_time;		//����� ��� ��� 
		}
				
		
		//���뷮 �����		
		
		
		break;


	case 2:  

		v_time = (int64_t)W_Intake_Time_visit - (int64_t)W_Intake_Time;			//�ӹ� �ð� ��� , �ʴ���
		if(v_time < 0) v_time= 0;
				
		W_Intake.visit_time = v_time;		// �ӹ� �ð��̰� ������ S		
		
		water_max_wait_time = 0;
		Gap_time = 50;
		Oper_Cnt = 3;
		
		break;
		
		
	case 3:
		
		if(gate_waiting_time == 0)
		{
			
			Oper_Cnt = 4; // ������ 

		} else {
			
			Gate_close();		
			//�Ұ� �ٽ� ���ü� ����-������ !!		
			rfid = R_rfid_check();	
			if(rfid != 0) {
				if(W_Intake.rfid_num != rfid)
				{
					//�ٸ��� ���� ����
					Oper_Cnt = 4;

					sprintf(buf, "C_Weight>%5d  ", weight);	
					TM_HD44780_Puts(0 ,1, buf);
				}
				else
				{
					in_flag = 1;
					//������ ����
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
			W_Intake.intake_weight = Stable_weight;		//��������� ���� �׳� �ִ´� 
			
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
			W_Intake.intake_weight = Stable_weight;		//��������� ���� �׳� �ִ´� 
			
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
		
		//Ÿ�ӿ���~!!
		//�ۼ��Ұ�
		
		break;

	default:
		break;
		
	}
}





