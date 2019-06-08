#include "hw_control.h"


volatile tm_struct Now;

void RTC_Init(void)
{
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		//백업레지스터 값이 이상하다면~! 시간 설정
		/* RTC Configuration */
		RTC_Configuration();

		Now.tm_year = 117; // 2017년
		Now.tm_mon = 1;
		Now.tm_mday = 2;
		Now.tm_hour = 01;
		Now.tm_min = 01;
		Now.tm_sec = 01;
		
		Time_Adjust(Now);

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		/* Check if the Power On Reset flag is set */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			
		}
		/* Check if the Pin Reset flag is set */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			
		}
		
		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Enable the RTC Second */
		//RTC_ITConfig(RTC_IT_SEC, ENABLE);		//인터럽트
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}

	/* Clear reset flags */
	RCC_ClearFlag();
}

void RTC_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit();

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Second */
	//RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

volatile uint32_t sec = 0;
void Time_Adjust(tm_struct time)
{
	RTC_WaitForLastTask();
	sec = RTC_DateToBinary(&time);
	RTC_SetCounter(sec);
	RTC_WaitForLastTask();
}



tm_struct read_RTC(void)
{
	tm_struct t;

#if 0
	// one day rtc
	/* Reset RTC Counter when Time is 23:59:59 */
	if (RTC_GetCounter() == 0x0001517F)
	{
		RTC_SetCounter(0x0);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
	}
#endif 

	uint32_t TimeVar = RTC_GetCounter();
	
	RTC_BinaryToDate(TimeVar, &t);
	

	return t;	
}

void IntToDate(tm_struct *pDate, uint16_t Value)
{
	pDate->tm_mday = (Value&0x1F);
	pDate->tm_mon = (Value>>5)&0xF;
	pDate->tm_year = (Value>>(5+4))&0x3F;	
	
	pDate->tm_year += 100;		//since1900
}

uint16_t DateToInt(tm_struct *pDate)
{
	uint16_t value;
	
	pDate->tm_year = pDate->tm_year-100; 	//since 1900
	if(pDate->tm_year < 0) pDate->tm_year = 0;
	
	value = (pDate->tm_year<<(5+4)) | (pDate->tm_mon<<5) | pDate->tm_mday;
	return value;
}

uint16_t Get_Date(void)
{
	tm_struct t = read_RTC();
	return DateToInt(&t);
	
}

uint16_t GetTime_min(void)		//시간 * 60 + 분
{
 	
	tm_struct t = read_RTC();
	
	return (t.tm_hour * 60) + t.tm_min;		
}

uint16_t GetTime_sec(void)		//분 * 60 + 초
{
 	
	tm_struct t = read_RTC();
	
	return (t.tm_min * 60) + t.tm_sec;		
}




