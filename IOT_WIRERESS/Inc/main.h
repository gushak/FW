/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "sw_panel.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define RELAY1_Pin GPIO_PIN_13
#define RELAY1_GPIO_Port GPIOC
#define RELAY2_Pin GPIO_PIN_14
#define RELAY2_GPIO_Port GPIOC
#define RELAY3_Pin GPIO_PIN_15
#define RELAY3_GPIO_Port GPIOC
#define RELAY5_Pin GPIO_PIN_0
#define RELAY5_GPIO_Port GPIOC
#define RELAY6_Pin GPIO_PIN_1
#define RELAY6_GPIO_Port GPIOC
#define RELAY7_Pin GPIO_PIN_2
#define RELAY7_GPIO_Port GPIOC
#define RELAY4_Pin GPIO_PIN_3
#define RELAY4_GPIO_Port GPIOC
#define RELAY8_Pin GPIO_PIN_0
#define RELAY8_GPIO_Port GPIOA
#define RELAY9_Pin GPIO_PIN_1
#define RELAY9_GPIO_Port GPIOA
#define RELAY10_Pin GPIO_PIN_2
#define RELAY10_GPIO_Port GPIOA
#define RELAY11_Pin GPIO_PIN_3
#define RELAY11_GPIO_Port GPIOA
#define RELAY12_Pin GPIO_PIN_4
#define RELAY12_GPIO_Port GPIOA
#define RELAY13_Pin GPIO_PIN_5
#define RELAY13_GPIO_Port GPIOA
#define RELAY14_Pin GPIO_PIN_6
#define RELAY14_GPIO_Port GPIOA
#define RELAY15_Pin GPIO_PIN_7
#define RELAY15_GPIO_Port GPIOA
#define RELAY16_Pin GPIO_PIN_4
#define RELAY16_GPIO_Port GPIOC
#define IN_LOAD_Pin GPIO_PIN_5
#define IN_LOAD_GPIO_Port GPIOC
#define IN_MISO_Pin GPIO_PIN_0
#define IN_MISO_GPIO_Port GPIOB
#define IN_CLK_Pin GPIO_PIN_1
#define IN_CLK_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_12
#define IN1_GPIO_Port GPIOB
#define IN2_Pin GPIO_PIN_13
#define IN2_GPIO_Port GPIOB
#define IN3_Pin GPIO_PIN_14
#define IN3_GPIO_Port GPIOB
#define IN4_Pin GPIO_PIN_15
#define IN4_GPIO_Port GPIOB
#define IN5_Pin GPIO_PIN_6
#define IN5_GPIO_Port GPIOC
#define IN6_Pin GPIO_PIN_7
#define IN6_GPIO_Port GPIOC
#define IN7_Pin GPIO_PIN_8
#define IN7_GPIO_Port GPIOC
#define IN8_Pin GPIO_PIN_9
#define IN8_GPIO_Port GPIOC
#define OUT_MOSI_Pin GPIO_PIN_8
#define OUT_MOSI_GPIO_Port GPIOA
#define BUZ_Pin GPIO_PIN_11
#define BUZ_GPIO_Port GPIOA
#define OUT_CLK_Pin GPIO_PIN_12
#define OUT_CLK_GPIO_Port GPIOA
#define OUT_RST_Pin GPIO_PIN_15
#define OUT_RST_GPIO_Port GPIOA
#define SW1_Pin GPIO_PIN_10
#define SW1_GPIO_Port GPIOC
#define SW2_Pin GPIO_PIN_11
#define SW2_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_12
#define SW3_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_2
#define LCD_RS_GPIO_Port GPIOD
#define LCD_E_Pin GPIO_PIN_3
#define LCD_E_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_4
#define LCD_D4_GPIO_Port GPIOB
#define LCD_D5_Pin GPIO_PIN_5
#define LCD_D5_GPIO_Port GPIOB
#define LCD_D6_Pin GPIO_PIN_6
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_7
#define LCD_D7_GPIO_Port GPIOB
#define OUT_LATCH_Pin GPIO_PIN_8
#define OUT_LATCH_GPIO_Port GPIOB
#define OUT_EN_Pin GPIO_PIN_9
#define OUT_EN_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

#define VERSION_H       1
#define VERSION_L       19

//s timer 10ms 
typedef enum
{
    TIMER_10MS_TASK_ALIVE_LED = 0,   //s led alive led 
    TIMER_10MS_TASK_IN_COUNT = 10, 
    TIMER_10MS_TASK_GLCD_REDISPALY = 20,     
    TIMER_10MS_TASK_RELAY_TIMER = 30,      
    TIMER_10MS_TASK_MAX = 100
}TIMER_10MS_TASK_STATE;


//s timer 1ms
typedef enum
{
    TIMER_1MS_TASK_IN_MENU = 0,    
    TIMER_1MS_TASK_CHECK_MODE_CHANGE,
    TIMER_1MS_TASK_IN_24V_LOAD,
    TIMER_1MS_TASK_IN_24V,
    TIMER_1MS_TASK_IN_SIG,   

    TIMER_1MS_TASK_OUT_RELAY_PENDING,    
    TIMER_1MS_TASK_OUT_RELAY,    
    TIMER_1MS_TASK_OUT_RELAY_LATCH,
    TIMER_1MS_TASK_IN_24V_READ,  
    TIMER_1MS_TASK_IN_ADC_START,

    TIMER_1MS_TASK_RF_SETTING,      
    TIMER_1MS_TASK_IN_MENU2,        
    TIMER_1MS_TASK_OUT_LCD,
    TIMER_1MS_TASK_OUT_RF,
    TIMER_1MS_TASK_OUT_BUZZER,
    
    TIMER_1MS_TASK_MAX = 20
}TIMER_1MS_TASK_STATE;

    
//s timer 1ms
typedef enum
{
    TIMER_IN_OUT_TASK_END = 0,    

}TIMER_IN_OUT_TASK_STATE;


//s Relay Out SPI Control 
typedef enum
{
    RELAY_DATA_NONE = 0,
    RELAY_DATA_SEND,
    RELAY_DATA_END
}RELAY_OUT_STATE;

//s Relay Out SPI Control 
typedef enum
{
    PENDING_STATE_STOP = 0,
    PENDING_STATE_START,
    PENDING_STATE_START_IN_CHECK,  
    PENDING_STATE_MOVING_STOP,   
    PENDING_STATE_MOVING_STOP_WAIT,    
    
    PENDING_STATE_PEED_START,
    PENDING_STATE_PEED_WAIT,             //s 5   
    PENDING_STATE_PEED_END,
    PENDING_STATE_START_OFF,
    PENDING_STATE_CMD_STOP              //s 8
    
}PENDING_TYPE_STATE;


#define SPI_READ_BUFF_SIZE      3
#define SPI_WRITE_BUFF_SIZE     4
#define RF_SPI_BUFF_SIZE        8

#define LORA_BUFF_SIZE  100
#define LORA_EXTRACT_BUFF_SIZE  10

#define AUTOMAN3025_RF_CONTROL  0               //s 로라 사용 불가로 삭제하고 신규 오토만 모듈 사용
#define AUTOMAN_RF_CONTROL      1
#define SERIAL_RF_CONTROL       2
#define SERIAL_RF_RX_CLEAR_COUNT    4  //s rx buff clear count 100ms

#define AUTOMAN_TX_DELAY_COUNT  2
#define AUTOMAN_TX_ACK_DELAY_COUNT  50  //s ack wait 1sec

#define VER_DISPLAY_COUNT  5            //s 5sec

#define AUTOMAN_TX_EVENT_COUNT  3  //s try to input event send 5 times until receive ack data

#define BUZZER_TIME_COUNT  2

#define GLCD_BUFF_SIZE  17


#define PENDING_RELAY_COUNT  150       //s 3sec   

#define RELAY_TIME_OFF          0
#define RELAY_TIME_TEST         30
#define RELAY_TIME_MATAIN       0xFFFF

#define HC165_KEY1_OFF     (0x00000001 << 6)            
#define HC165_KEY1_POSI    (0x00000001 << 7) 
#define HC165_KEY1_NEGA    (0x00000001 << 5)       
#define HC165_KEY2_OFF     (0x00000001 << 3) 
#define HC165_KEY2_POSI    (0x00000001 << 4)      
#define HC165_KEY2_NEGA    (0x00000001 << 2) 
#define HC165_KEY3_OFF     (0x00000001 << 1) 
#define HC165_KEY3_POSI    (0x00000001 << 0) 
#define HC165_KEY3_NEGA    (0x00000001 << 15) 
#define HC165_KEY4_OFF     (0x00000001 << 13) 
#define HC165_KEY4_POSI    (0x00000001 << 14) 
#define HC165_KEY4_NEGA    (0x00000001 << 12) 
#define HC165_KEY5_OFF     (0x00000001 << 10) 
#define HC165_KEY5_POSI    (0x00000001 << 11) 
#define HC165_KEY5_NEGA    (0x00000001 << 9) 
#define HC165_KEY6_OFF     (0x00000001 << 8) 
#define HC165_KEY6_POSI    (0x00000001 << 23) 
#define HC165_KEY6_NEGA    (0x00000001 << 22) 
#define HC165_KEY7_OFF     (0x00000001 << 20) 
#define HC165_KEY7_POSI    (0x00000001 << 21) 
#define HC165_KEY7_NEGA    (0x00000001 << 19) 
#define HC165_KEY8_OFF     (0x00000001 << 17) 
#define HC165_KEY8_POSI    (0x00000001 << 18) 
#define HC165_KEY8_NEGA    (0x00000001 << 16) 


//#define LORA_RF_CONTROL  1
//#ifndef  LORA_RF_CONTROL
//#define AUTOMAN_RF_CONTROL 1
//#endif
//#if(LORA_RF_CONTROL == 0 && AUTOMAN_RF_CONTROL == 0)
//#define SERIAL_RF_CONTROL 1
//#endif

extern uint8_t cRfProcessMode;                           //s rf setting
extern uint8_t cRfSettingState;                           //s rf setting
extern uint16_t cRfSettingDelay;                           //s rf setting
extern uint16_t cRfRetryCount;                           //s rf setting
extern uint16_t cRfFreqRetryCount;                           //s rf setting

extern uint8_t buffUartRxData[LORA_BUFF_SIZE];
extern uint8_t buffLightRxData[LORA_BUFF_SIZE];

extern unsigned char cReLcdDisplay;
extern unsigned char cRelaySendState;         //s display Relay data
extern char buffGLcd1Line[GLCD_BUFF_SIZE];
extern char buffGLcd2Line[GLCD_BUFF_SIZE];

//s buzzer control
extern uint8_t cBuzzerOnOff;
extern uint8_t cBuzzerCount;


extern uint32_t iHc165DataValueResult;
extern uint8_t  cHc165ProcessFlag;
extern uint8_t  buffSpiReadData[3];


void DataInit(void);
void LightExcuTask(void);
void PendingTask(uint8_t state);
//void Uart3RxLoraData(uint8_t data)

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
