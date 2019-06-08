#include "hw_control.h"

#define OPEN_MOTOR	1

extern volatile uint16_t FET_value[10];

volatile uint8_t Input_value[2]={0,};
volatile uint8_t inx = 0;

#ifndef OPEN_MOTOR
void Gate_close()
{
	uint16_t err_cnt = 0;
		
	FET_value[4] = 15;	//100mS ����
	Delay(100);
	

	while(1)
	{
		FET_value[4] = 10;
		if(err_cnt ++ > 2000) 
		{
			break;
		}
		
		if(INPUTM == 0)
		{
			break;
		}
		Delay(1);
	}
	FET_value[4] = 0;
	
}

void Gate_open_Delay(uint16_t m_cnt)
{
	FET_value[4] = m_cnt;
	Delay(m_cnt*10);
	
}

void Gate_open(uint16_t m_cnt)
{
	FET_value[4] = m_cnt;	
	
}
#else

//����� ����Ʈ ���
//���� OPEN(����)
//���� �ָ� ��ħ!!
void Gate_close()
{
	FET_value[4] = 600;	//60�ʰ� ��ħ 
	//���� ���� �������ֵ��� 	
}

void Gate_open_Delay(uint16_t m_cnt)
{
	FET_value[4] = 0;

	
}

void Gate_open(uint16_t m_cnt)
{
	FET_value[4] = 0;	
	
}

#endif


void loop_input_check()
{
	//10mS���� ����
	static uint8_t in_cnt[8];
	inx = INPUT1;
	
	if(inx != 0) {   // �Է��� �ȵ��� ���
		Input_value[0] = 0;	
		in_cnt[0] = 0;
		
	} else { // �Է��� ������ ���	
		if(in_cnt[0] >= 10) {
			Input_value[0] = 1;			
			
		} else {
			in_cnt[0] ++;
		}
	}
}
















