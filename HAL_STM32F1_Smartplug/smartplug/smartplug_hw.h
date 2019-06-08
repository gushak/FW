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

#include "stm32f1xx.h"
#include "comm_def.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

//MCU_RY
#define MCU_RY_GPIO_HIGH_TYPE             GPIOC
#define MCU_RY_GPIO_HIGH_PIN              GPIO_PIN_1                   
#define MCU_RY_HIGH_ON()                  HAL_GPIO_WritePin(MCU_RY_GPIO_HIGH_TYPE, MCU_RY_GPIO_HIGH_PIN, GPIO_PIN_SET)    
#define MCU_RY_HIGH_OFF()                 HAL_GPIO_WritePin(MCU_RY_GPIO_HIGH_TYPE, MCU_RY_GPIO_HIGH_PIN, GPIO_PIN_RESET)         

#define MCU_RY_GPIO_LOW_TYPE              GPIOC
#define MCU_RY_GPIO_LOW_PIN               GPIO_PIN_0                   
#define MCU_RY_LOW_ON()                   HAL_GPIO_WritePin(MCU_RY_GPIO_LOW_TYPE, MCU_RY_GPIO_LOW_PIN, GPIO_PIN_SET)    
#define MCU_RY_LOW_OFF()                  HAL_GPIO_WritePin(MCU_RY_GPIO_LOW_TYPE, MCU_RY_GPIO_LOW_PIN, GPIO_PIN_RESET)         

//Active low
#define LED_RG_GPIO_TYPE		  GPIOC
#define LED_RG_GPIO_PIN		          GPIO_PIN_2
#define LED_RG_ON()			  HAL_GPIO_WritePin(LED_RG_GPIO_TYPE, LED_RG_GPIO_PIN, GPIO_PIN_RESET)
#define LED_RG_OFF()			  HAL_GPIO_WritePin(LED_RG_GPIO_TYPE, LED_RG_GPIO_PIN, GPIO_PIN_SET)

//Active low
#define LED_DLINK_GPIO_TYPE		  GPIOC
#define LED_DLINK_GPIO_PIN                GPIO_PIN_3
#define LED_DLINK_ON()			  HAL_GPIO_WritePin(LED_DLINK_GPIO_TYPE, LED_DLINK_GPIO_PIN, GPIO_PIN_RESET)
#define LED_DLINK_OFF()			  HAL_GPIO_WritePin(LED_DLINK_GPIO_TYPE, LED_DLINK_GPIO_PIN, GPIO_PIN_SET)

#define LED
//switch input handler
#define SW_INT_GPIO_TYPE                  GPIOB
#define SW_INT_SPIO_PIN                   GPIO_PIN_0
#define SW_INT_READ()                     HAL_GPIO_ReadPin(GPIOB, GPIO_Pin_0)

void hw_init();

void hw_gpio_init();

void hw_led_rg_onoff(bool_t is_on);

void hw_led_dlink_onoff(bool_t is_on);

void hw_mcu_high_onoff(bool_t is_on);

void hw_mcu_low_onoff(bool_t is_on);

void hw_timer_init();

void hw_reset(const char* reason);

uint8_t hw_read_ry_gpio();

void hw_reset(const char* reason);

void hw_relay_func(bool_t is_on);

void hw_safe_delay(uint32_t millis);