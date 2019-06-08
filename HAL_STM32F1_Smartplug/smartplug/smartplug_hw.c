/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: system initialization
*
* History
* Date			Description
* 2018/09/14	file created
*****************************************************************************/

#include "smartplug_hw.h"
#include "smartplug_it.h"

static void hw_gpio_init();

void hw_init()
{ 
        hw_gpio_init();
}

void hw_gpio_init()
{
        LED_RG_OFF();
        LED_DLINK_OFF();     
        hw_mcu_high_onoff(kFalse);     
        hw_mcu_low_onoff(kTrue);  
}

void hw_led_rg_onoff(bool_t is_on)
{
        if(is_on)
        {
          LED_RG_ON();
        }
        else
        {
          LED_RG_OFF();
        }
}

void hw_led_dlink_onoff(bool_t is_on)
{
        if(is_on)
        {
          LED_DLINK_ON();
        }
        else
        {
          LED_DLINK_OFF();
        }
}

void hw_mcu_high_onoff(bool_t is_on)
{
        if(is_on)
        {
          MCU_RY_HIGH_ON();
          hw_safe_delay(60);
          MCU_RY_HIGH_OFF();
        }
        else
        {
          MCU_RY_HIGH_OFF();
        }
}

void hw_mcu_low_onoff(bool_t is_on)
{
        if(is_on)
        {
          MCU_RY_LOW_ON();
          hw_safe_delay(60);
          MCU_RY_LOW_OFF();
        }
        else
        {
          MCU_RY_LOW_OFF();
        }
}

void hw_reset(const char* reason)
{
	DSYNCOUT("reboot");
	DSYNCOUT(reason);
	DSYNCOUT("\r\n");
	NVIC_SystemReset();
}

uint8_t hw_read_ry_gpio()
{
        return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
}

void hw_relay_func(bool_t is_on)
{
        if(is_on == kTrue)
        {
              hw_mcu_high_onoff(kTrue);
              hw_mcu_low_onoff(kFalse);
              hw_led_rg_onoff(kTrue);
              g_RelayInfo.relay_stat = 1;
        }
        else
        {
              hw_mcu_high_onoff(kFalse);
              hw_mcu_low_onoff(kTrue);
              hw_led_rg_onoff(kFalse);
              g_RelayInfo.relay_stat = 0;
        }
}

void hw_safe_delay(uint32_t millis)
{
     for(; millis != 0; millis--)
     {
            smartplug_set_main_loop_run();
            delay_1ms();
     }
}
