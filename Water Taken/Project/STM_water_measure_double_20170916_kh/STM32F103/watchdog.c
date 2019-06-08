#include "hw_control.h"

uint8_t Wdt_init(uint16_t timeout_sec)
{
	uint8_t result = 0;
	uint16_t reload = 0;
	
	 if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
		result = 1;
        RCC_ClearFlag();
    }

	//내부 LSI사용하는 WDT 쓰기 작동
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	//LSI 40Khz 
    IWDG_SetPrescaler(IWDG_Prescaler_256);	//156.25hz 0.0064
	float temp = 0;
	temp = (float)timeout_sec / 0.0064;
	reload = (uint16_t)temp;
	
	IWDG_SetReload(reload);
	
    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
	
	return result;
}

void Wdt_reset(void) {
	IWDG_ReloadCounter();
}
