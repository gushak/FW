#include "hw_control.h"

#define OPEN_MOTOR	1

extern volatile uint16_t FET_value[10];

volatile uint8_t Input_value[2]={0,};
volatile uint8_t inx = 0;

#ifndef OPEN_MOTOR
void Gate_close()
{
	uint16_t err_cnt = 0;
		
	FET_value[4] = 15;	//100mS 모터
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

//에어식 게이트 사용
//평상시 OPEN(열림)
//값을 주면 닫침!!
void Gate_close()
{
	FET_value[4] = 600;	//60초간 닫침 
	//에어 끈다 문열려있도록 	
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
	//10mS마다 동작
	static uint8_t in_cnt[8];
	inx = INPUT1;
	
	if(inx != 0) {   // 입력이 안들어온 경우
		Input_value[0] = 0;	
		in_cnt[0] = 0;
		
	} else { // 입력이 들어왔을 경우	
		if(in_cnt[0] >= 10) {
			Input_value[0] = 1;			
			
		} else {
			in_cnt[0] ++;
		}
	}
}
















