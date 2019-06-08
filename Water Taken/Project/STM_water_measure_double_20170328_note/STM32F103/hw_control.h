#ifndef __HW_CONTROL__
#define __HW_CONTROL__

#define MRAM_EEPROM_ENABLE
#define WEIGHT_SINGLE

#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "eeprom.h"
//----------------------------------
#include "main.h"
#include "gpio.h"
#include "khh_lcd.h"
#include "Rfid_reader.h"
#include "w_weight.h"
//----------------------------------
#include "lrk_motor.h" 
#include "uart.h"
//----------------------------------
#include "khh_eeprom.h"
#include "menu.h"
#include "water_intake.h"
#include "KHH_stm103_RTC.h"
#include "water_intake_mram_menage.h"
#include "spi_flash.h"
#include "rtc_to_sec.h"
#include "Lora_parse.h"
//----------------------------------
#define Write_eeprom(adr, data) EE_WriteVariable(VirtAddVarTab[adr], data);
#define Read_eeprom(adr, data) 	EE_ReadVariable(VirtAddVarTab[adr], (uint16_t*)&data);

//----------------------------------
extern void SysTick_Handler(void);
extern void cycleCounterInit(void);
extern uint32_t micros(void);
extern uint32_t millis(void);
extern void Delay(uint32_t ms);
extern void delay_us(uint32_t t);
extern void delay_ms(uint32_t xx);
extern void timer_check(void);
extern void NVIC_Configuration(void);
extern void GPIO_Configuration(void);
extern void setup();

//>---------------------------------
extern volatile uint8_t SW_S[3];	
extern volatile uint8_t SW_L[3];
extern volatile uint8_t SW_Cnt[3]; 

//>---------------------------------
extern volatile uint16_t Stable_weight, weight;
extern volatile uint8_t Stable_ch_flag;
extern volatile uint16_t Stable_weight_ch1, weight_ch1;
extern volatile uint16_t Stable_weight_ch2, weight_ch2;

//----------------------------------

#define WATER_ADDRESS							Config.Value[0] //주소`				
#define WATER_ADDRESS_MAX					255

#define R_waiting_time					Config.Value[1] 
#define R_waiting_time_MAX			100

#define GATE_motion_time				Config.Value[2] //모터 구동시간300mS
#define GATE_motion_time_MAX		9999

#define GATE_waiting_time				Config.Value[3] //소가 고개를 들었다.재입장 대기시간 
#define GATE_waiting_time_MAX		999

#define GATE_In_gap_time				Config.Value[4] 
#define GATE_In_gap_time_MAX		1000

#define WATER_MAX_wait_time		Config.Value[5] 	//만수대기 시간
#define WATER_MAX_wait_time_MAX	1000


#define RFID_TIMEOVER					Config.Value[6] 	//리더값이 감지 안되는 시간
#define RFID_TIMEOVER_MAX			999


#define WEIGHT_TEMP					Config.Value[7] 
#define WEIGHT_TEMP_MAX				255

#define CHANNELwaiting_time			Config.Value[8] 
#define CHANNELwaiting_time_MAX	9000

#define CON_Water_eat					Config.Value[9] 
#define CON_Water_eat_MAX			1

#define FULL_WATER_OVER_TIME	Config.Value[10] 
#define FULL_WATER_OVER_TIME_MAX			65000

#define DEVICE_STATUS_OK						0x00
#define DEVICE_WATER_MAX_TIMEOVER	0x10
#define DEVICE_MOTOR_TIMEVOER			0x20


#endif	//__HW_CONTROL__