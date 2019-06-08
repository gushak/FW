#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: system initialization
*
* History
* Date			Description
* 2018/09/13	file created
*****************************************************************************/

#include "modem_def2.h"
#include "stm32f10x.h"

//MCU_RY
#define MCU_RY_GPIO_HIGH_TYPE             GPIOC
#define MCU_RY_GPIO_HIGH_PIN              GPIO_PIN_0                   
#define MCU_RY_HIGH_ON()                  GPIO_SetBits(MCU_RY_GPIO_HIGH_TYPE, MCU_RY_GPIO_HIGH_PIN)    
#define MCU_RY_HIGH_OFF()                 GPIO_ResetBits(MCU_RY_GPIO_HIGH_TYPE, MCU_RY_GPIO_HIGH_PIN)         

#define MCU_RY_GPIO_LOW_TYPE              GPIOC
#define MCU_RY_GPIO_LOW_PIN               GPIO_PIN_1                   
#define MCU_RY_LOW_ON()                   GPIO_SetBits(MCU_RY_GPIO_LOW_TYPE, MCU_RY_GPIO_LOW_PIN)    
#define MCU_RY_LOW_OFF()                  GPIO_ResetBits(MCU_RY_GPIO_LOW_TYPE, MCU_RY_GPIO_LOW_PIN)         

//Active low
#define LED_RG_GPIO_TYPE		  GPIOC
#define LED_RG_GPIO_PIN		  GPIO_PIN_2
#define LED_RG_ON()			  GPIO_ResetBits(LED_RG__GPIO_TYPE, LED_RG__GPIO_PIN)
#define LED_RG_OFF()			  GPIO_SetBits(LED_RG__GPIO_TYPE, LED_RG__GPIO_PIN)

void hw_init();

void hw_mcu_high_onoff(bool_t is_on);

void hw_mcu_ry_low_onoff(bool_t is_on);

void hw_led_rg_onoff(bool_t is_on);

void hw_reset(const char* reason);

void hw_timer_init();