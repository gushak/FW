
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "khh_lcd.h"
#include "sw_panel.h"
#include "menu.h"
#include <stdlib.h> 
#include <string.h>
#include "rf_modules.h"
#include "hc165.h"
#include "hc595.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
TIMER_10MS_TASK_STATE Timer10msTaskCount;
TIMER_1MS_TASK_STATE Timer1msTaskCount;
TIMER_IN_OUT_TASK_STATE TimerInOutTaskCount;

uint16_t wLedCount;             //s for test
uint8_t  cSwitchEnter;      
uint8_t  cSwitchUp;
uint8_t  cSwitchDown;

uint8_t cVersionCount;
uint8_t cVersionDisplayEnd;


//uint8_t  cDipSwitch;


//s relay data
key_stat SwPanelData;
Out_struct RelayControlData;

uint8_t cRelaySendState;         //s display Relay data
uint8_t cRelayUploadCount;

uint8_t cPendingRelayState;
uint8_t cPendingRelayCount;

uint8_t cSaveConfigType = 0;
uint16_t cSaveConfigReverse = 0;

//s rf data
uint8_t buffUartRxData[LORA_BUFF_SIZE];
//uint8_t buffUartTxData[LORA_BUFF_SIZE];
uint8_t buffUartRxDataSave[LORA_BUFF_SIZE];
uint8_t buffUartRxDataExcute[LORA_BUFF_SIZE];
uint8_t buffUartRxRfData[LORA_BUFF_SIZE];
uint8_t cRxdataCount;                           //s rx data count 
uint8_t cRxdataExcuteCount;                           //s rx data count 
uint8_t cRxComplteState;
RfDataStruct strRfRxData;
RfDataStruct strRfTxData;
uint8_t cRfProcessMode = 0;                           //s loar setting
uint8_t cRfSettingState = 0;                           //s rf setting
uint16_t cRfSettingDelay = 0;                           //s rf setting
uint16_t cRfRetryCount = 0;                           //s rf setting
uint16_t cRfFreqRetryCount = 0;                           //s rf setting
uint8_t cRfSetProcessFlag = RESET; 

uint8_t cRfTxDelayState;
uint8_t cRfTxDelayCount;
uint8_t cRfRXBuffClearCount;
uint8_t cRfTxEventSendCount;


//s Light 232 data
uint8_t buffLightRxData[LORA_BUFF_SIZE];
uint8_t buffLightRxDataSave[LORA_BUFF_SIZE];
uint8_t buffLightRxRfData[LORA_BUFF_SIZE];
uint8_t cLightRxdataCount;                           //s rx data count 
uint8_t cLightRxComplteState;

uint8_t cLightTxDelayState;
uint8_t cLightTxDelayCount;
uint8_t cLightRXBuffClearCount;
uint8_t cLightTxEventSendCount;

//s for rf test
uint8_t cTxdataCount;
uint8_t* cRpar = "$Rpar=\r\n";
uint8_t* cWarup = "$Warup=0\r\n";
uint8_t* cWflash = "$Wflash=\r\n";


//s lora tx data
uint8_t* TextTrup = "$Trup=";


//s Lcd Data 2*16
uint8_t cMenuControl;
char buffGLcd1Line[GLCD_BUFF_SIZE];
char buffGLcd2Line[GLCD_BUFF_SIZE];
uint8_t cReLcdDisplay;

//s buzzer control
uint8_t cBuzzerOnOff;
uint8_t cBuzzerCount;

InputsStruct StrInput;


//s hc165 Read data
uint32_t iHc165DataValueResult;
uint8_t  cHc165ProcessFlag;
uint8_t  buffSpiReadData[3] = {0,0,0};

//s hc595 Read data
uint32_t iHc595DataValueResult;
uint8_t  cHc595ProcessFlag;
uint8_t  buffSpiWriteData[2] = {0,0};

//s control light pwm part
uint16_t wLightOnTime[2];
uint16_t wLightOffTime[2]; 

uint16_t wTimer500usCont;
uint8_t  cLightTimingFlag;
uint8_t cLightOnFlag[2];
uint8_t cLightOffFlag[2];
uint32_t wLightCountSave[2];
uint32_t wLightOnCount[2];
uint32_t wLightOffCount[2];	
uint32_t wLightOnProcCount[2];
uint32_t wLightOffProcCount[2];	
uint32_t wLightOnPercent[2];

uint16_t wLightUartTime[2];
uint8_t cLightUartPercent[2];

uint8_t AutomodeSendFlag;

uint16_t AutomanReceiveErrorCount;

uint8_t RxCheckCount;

uint16_t wRelayLedValue = 0;  

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
//s data initalize
void DataInit(void)
{
  wLedCount = 0;                //s for test
  cSwitchEnter = 0;
  cSwitchUp    = 0;
  cSwitchDown = 0;
  
  cVersionCount = VER_DISPLAY_COUNT;
  cVersionDisplayEnd = RESET;  
//  cDipSwitch = 0;
  
  //cSpiData = 0;                 //s for test
  //HAL_GPIO_WritePin(OUT_RST_GPIO_Port, OUT_RST_Pin, GPIO_PIN_SET);  
  
  cRelaySendState = RELAY_DATA_NONE;            //s Relay data control state
  cRelayUploadCount = 0;
  
  Timer10msTaskCount = TIMER_10MS_TASK_ALIVE_LED;
  Timer1msTaskCount = TIMER_1MS_TASK_IN_MENU;
  
  memset(&SwPanelData, 0, sizeof(SwPanelData));
  memset(&RelayControlData, 0, sizeof(RelayControlData));

  cPendingRelayState = PENDING_STATE_STOP;
  cPendingRelayCount = 0;
  
  //s rf data
  memset(buffUartRxData, 0, LORA_BUFF_SIZE);
//  memset(buffUartTxData, 0, LORA_BUFF_SIZE);  
  memset(buffUartRxDataSave, 0, LORA_BUFF_SIZE);    
  memset(buffUartRxRfData, 0, LORA_BUFF_SIZE);    
  
  cRxdataCount = 0;                           //s rx data count 
  cRxComplteState = RF_DATA_NONE;  
  cRfTxDelayState = RF_DATA_NONE;
  cRfRXBuffClearCount = 0;
  memset(&strRfRxData, 0, sizeof(RfDataStruct));      
  memset(&strRfTxData, 0, sizeof(RfDataStruct));//hhlee
  cRfTxEventSendCount = 0;

  memset(&StrInput, 0, sizeof(StrInput));    
  
  StrInput.Input[1].Key = SET;
  StrInput.Input[3].Key = SET;
  StrInput.Input[5].Key = SET;  
  StrInput.Input[1].OldKey = SET;
  StrInput.Input[3].OldKey = SET;
  StrInput.Input[5].OldKey = SET;  
  
  
  cMenuControl = 0;
  
  //s buzzer control
  cBuzzerOnOff = RESET;
  cBuzzerCount = 0;

  //s glcd control
  cReLcdDisplay = 0;
  memset(buffGLcd1Line, 0, GLCD_BUFF_SIZE);
  memset(buffGLcd2Line, 0, GLCD_BUFF_SIZE);    

  AutomodeSendFlag = 0;
  AutomanReceiveErrorCount = 0;
  
}


void PandingAutoMode(void)
{
  
   if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){         //s only use pending mode
     if(cRxComplteState == RF_MAIN_PENDING_AUTO_START)
     {
       strRfTxData.Cmd = RF_SUB_PENDING_AUTO_START;
       strRfTxData.Mode = CONFIG_DEVIEC_TPYE;       

//       SwPanelRfDataChange(&RelayControlData, SW_PANDING_PANDING_PULL, RELAY_TIME_OFF);
//       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
    
       SwPanelRfDataChange(&RelayControlData, SW_PANDING_PULL, RELAY_TIME_OFF);
       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
        if(CONFIG_MOVING_COUNT >= 1){             
           SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING1_ON, RELAY_TIME_MATAIN);
           GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
        }
        if(CONFIG_MOVING_COUNT >= 2){
           SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING2_ON, RELAY_TIME_MATAIN);
           GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);    
        }
        if(CONFIG_MOVING_COUNT >= 3){    
           SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING3_ON, RELAY_TIME_MATAIN);
           GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);
        }
                
       SwPanelRelayControl(&RelayControlData);

       cPendingRelayState = PENDING_STATE_START;   
       
       cRfTxDelayState = RF_DATA_NONE;        
       cRelaySendState = RELAY_DATA_SEND;
       cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
       cRfTxDelayState = RF_MAIN_PENDING_AUTO_START;           
      cBuzzerOnOff = SET;
      cBuzzerCount = BUZZER_TIME_COUNT;         
     }          
     else if(cRxComplteState == RF_MAIN_PENDING_AUTO_STOP)
     {
       strRfTxData.Cmd = RF_SUB_PENDING_AUTO_STOP;
       strRfTxData.Mode = CONFIG_DEVIEC_TPYE;  
   
//       SwPanelRfDataChange(&RelayControlData, SW_PANDING_PULL, RELAY_TIME_OFF);
//       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);               
                     
//       SwPanelRfDataChange(&RelayControlData, SW_PANDING_PANDING_ANNEAL, RELAY_TIME_MATAIN);
//       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);               
        if(CONFIG_MOVING_COUNT >= 1){             
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING1_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
        }
        if(CONFIG_MOVING_COUNT >= 2){
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING2_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);     
        }
        if(CONFIG_MOVING_COUNT >= 3){    
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING3_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
        }

       SwPanelRelayControl(&RelayControlData);
       
       cPendingRelayState = PENDING_STATE_CMD_STOP;

       cRfTxDelayState = RF_DATA_NONE;       
       cRelaySendState = RELAY_DATA_SEND;
       cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
       cRfTxDelayState = RF_MAIN_PENDING_AUTO_STOP;           
       
      cBuzzerOnOff = SET;
      cBuzzerCount = BUZZER_TIME_COUNT;            
     }   
  }
   else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){         //s only use pending mode
     if(cRxComplteState == RF_MAIN_PENDING_AUTO_START)
     {
       strRfTxData.Cmd = RF_SUB_PENDING_AUTO_START;
       strRfTxData.Mode = DEVICE_TYPE_PENDING;   // DEVICE_TYPE_PENDING과 같이 사용됨     
    
       SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_PULL, RELAY_TIME_OFF);
       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
       
       
        if(CONFIG_MOVING_COUNT >= 1){             
           SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING1_ON, RELAY_TIME_MATAIN);
           GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
        }
        if(CONFIG_MOVING_COUNT >= 2){
           SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING2_ON, RELAY_TIME_MATAIN);
           GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);    
        }
        if(CONFIG_MOVING_COUNT >= 3){    
           SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING3_ON, RELAY_TIME_MATAIN);
           GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);
        }
                
       SwPanelRelayControl(&RelayControlData);

       cPendingRelayState = PENDING_STATE_START;   
       
       cRfTxDelayState = RF_DATA_NONE;        
       cRelaySendState = RELAY_DATA_SEND;
       cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
       cRfTxDelayState = RF_MAIN_PENDING_AUTO_START;           
      cBuzzerOnOff = SET;
      cBuzzerCount = BUZZER_TIME_COUNT;            
     }          
     else if(cRxComplteState == RF_MAIN_PENDING_AUTO_STOP)
     {
       strRfTxData.Cmd = RF_SUB_PENDING_AUTO_STOP;
       strRfTxData.Mode = DEVICE_TYPE_PENDING;  
   
       SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_PULL, RELAY_TIME_OFF);
       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);               
    
        if(CONFIG_MOVING_COUNT >= 1){             
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING1_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
        }
        if(CONFIG_MOVING_COUNT >= 2){
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING2_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);     
        }
        if(CONFIG_MOVING_COUNT >= 3){    
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING3_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
        }

       SwPanelRelayControl(&RelayControlData);
       
       cPendingRelayState = PENDING_STATE_CMD_STOP;

       cRfTxDelayState = RF_DATA_NONE;       
       cRelaySendState = RELAY_DATA_SEND;
       cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
       cRfTxDelayState = RF_MAIN_PENDING_AUTO_STOP;          
      cBuzzerOnOff = SET;
      cBuzzerCount = BUZZER_TIME_COUNT;            
     }   
  }    
  
}

////s for printf   //s 라이트 디밍 추가로 printf 사용 불가 181113
//int fputc(int ch, FILE *f) 
//{
//  uint8_t temp[1] = {ch};
//  HAL_UART_Transmit(&huart1, temp, 1, 2);
//  return (ch);
//}

void MainRecoverProcess(void)
{
  if(CONFIG_RESET_SAVE){
    CONFIG_RESET_SAVE = RESET;
    
    for(uint8_t i = 0; i < 16; i++){
      if(CONFIG_RELAY_SAVE & (1 << i)){
        SwPanelRfDataChange(&RelayControlData, i, RELAY_TIME_MATAIN);
        GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
      }
    }
    
    if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){            //s only use DEVICE_TYPE_PENDING type
      StrInput.Input[1].Time = CONFIG_PANDING_TIME1 * 50;
      StrInput.Input[2].Time = CONFIG_PANDING_TIME2 * 50;
      StrInput.Input[3].Time = CONFIG_PANDING_TIME3 * 50;
      cPendingRelayState = CONFIG_PANDING_SAVE;
      
        if(CONFIG_PANDING_TIME1)             //s when Event are input 1~2. it must be clear 
        {
          StrInput.KeyData |= 0x02;
          //StrInput.Event |= 0x02;      
          StrInput.Input[1].Key = RESET;
          StrInput.Input[1].OldKey = SET;          
        }
        if(CONFIG_PANDING_TIME2)
        {
          StrInput.KeyData |= 0x08;
          //StrInput.Event |= 0x02;  
          StrInput.Input[3].Key = RESET;
          StrInput.Input[3].OldKey = SET;             
        }
        if(CONFIG_PANDING_TIME3)
        {
          StrInput.KeyData |= 0x20;
          //StrInput.Event |= 0x20; 
          StrInput.Input[4].Key = RESET;
          StrInput.Input[5].OldKey = SET;             
        }
        StrInput.Event = 0;               
      }
      CONFIG_PANDING_TIME1 = 0;
      CONFIG_PANDING_TIME2 = 0;
      CONFIG_PANDING_TIME3 = 0;
    
    save_all_eeprom();    
    
  }    
  
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint16_t i = 0;
  uint8_t event = 0;  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  DataInit();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_IWDG_Init();
  MX_TIM2_Init();

  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  
  if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {
//    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); 
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
  else
  {    
//    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); 
  }  


  lll_eeprom_init();
  load_eeprom();

  cRelaySendState = RELAY_DATA_SEND;            //s Relay data control state
  cSaveConfigReverse = CONFIG_8CH_REVERSE;

  if(CONFIG_RF_FREQ_ADDR1 > 3 ){
    event = 1;
    CONFIG_RF_FREQ_ADDR1 = 3;
  }

  if(CONFIG_RF_FREQ_ADDR2 >= RF_AUTOMAN_FREQ_GR3_MAX){
    if(CONFIG_RF_FREQ_ADDR1 == 0){
      if(CONFIG_RF_FREQ_ADDR2 >= RF_AUTOMAN_FREQ_GR0_MAX){
        CONFIG_RF_FREQ_ADDR2 = RF_AUTOMAN_FREQ_GR0_MAX;
        event = 1;
      }
    }
    else if(CONFIG_RF_FREQ_ADDR1 == 1){
      if(CONFIG_RF_FREQ_ADDR2 >= RF_AUTOMAN_FREQ_GR1_MAX){
        CONFIG_RF_FREQ_ADDR2 = RF_AUTOMAN_FREQ_GR1_MAX;
        event = 1;
      }
    }    
    else if(CONFIG_RF_FREQ_ADDR1 == 2){
      if(CONFIG_RF_FREQ_ADDR2 >= RF_AUTOMAN_FREQ_GR2_MAX){
        CONFIG_RF_FREQ_ADDR2 = RF_AUTOMAN_FREQ_GR2_MAX;
        event = 1;
      }
    }  
    else if(CONFIG_RF_FREQ_ADDR1 == 3){
      if(CONFIG_RF_FREQ_ADDR2 >= RF_AUTOMAN_FREQ_GR3_MAX){
        CONFIG_RF_FREQ_ADDR2 = RF_AUTOMAN_FREQ_GR3_MAX;
        event = 1;
      }
    }      
  }
  
  if(event){
    save_all_eeprom();
  }  
  
  
  MX_TIM1_Init();  
  
  cSaveConfigType = CONFIG_DEVIEC_TPYE;         //s for relay initialization when this device is type change

  if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL){                //s 계사는 PWM 라이트 역방향
    HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET); 
    HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);                 
  }
  
  LCD_init();

  HAL_UART_Receive_DMA(&huart3, buffUartRxData, 1);
  HAL_UART_Receive_DMA(&huart1, buffLightRxData, 1);
  
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)                  //s timer 2 start 10ms
  {
     _Error_Handler(__FILE__, __LINE__);
  }
  
  if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)                  //s timer 3 start 1ms
  {
     _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_Base_Start_IT(&htim1) != HAL_OK)                  //s timer 3 start 1ms
  {
     _Error_Handler(__FILE__, __LINE__);
  }  

//  if (HAL_TIM_Base_Start_IT(&htim5) != HAL_OK)                  //s timer 3 start 1ms
//  {
//     _Error_Handler(__FILE__, __LINE__);
//  }    
  
  sprintf(buffGLcd1Line, "Ver %01d.%02d ID %02d", VERSION_H, VERSION_L, CONFIG_RF_ADDR);    
  cReLcdDisplay = 0;
  TM_HD44780_Puts_State(0,0,buffGLcd1Line);  
  
  //printf("IOT WIRERESS Control \r\n");
  //NVIC_SystemReset();                         //s test
  

  MainRecoverProcess();                         //s 232 통신 장애로 인한 eeprom 저장 상태
  
  if((Config.Value[1] == DEVICE_TYPE_16CH) || (Config.Value[1] == DEVICE_TYPE_8CH) || (Config.Value[1] == DEVICE_TYPE_8CH_LIMIT)){
    HAL_GPIO_WritePin(OUT_RST_GPIO_Port, OUT_RST_Pin, GPIO_PIN_SET);  //s 595 사용시 필요함
    HAL_GPIO_WritePin(OUT_EN_GPIO_Port, OUT_EN_Pin, GPIO_PIN_RESET);        
  }
  

  __HAL_IWDG_START(&hiwdg);  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    __HAL_IWDG_RELOAD_COUNTER(&hiwdg);                  //s watchdog count reset

//    uint8_t rfdata[8];        
    
    if(cRxdataCount)
    {
      
      cRxdataExcuteCount = cRxdataCount;
      memcpy(buffUartRxDataExcute, buffUartRxDataSave, cRxdataExcuteCount);
      cRxdataCount = 0;
       memset(buffUartRxDataSave, 0, sizeof(buffUartRxDataSave));      
       for(i = 0; i < cRxdataExcuteCount; i++)
       {
          cRxComplteState = PorcessPacket_Rf_check(buffUartRxDataExcute[i], buffUartRxRfData);
       }      
       cRxdataExcuteCount = 0;
       AutomanReceiveErrorCount = 0;
       memset(buffUartRxDataExcute, 0, sizeof(buffUartRxDataExcute));
       cRfRXBuffClearCount = 0;
    }
    if(cRfRXBuffClearCount > SERIAL_RF_RX_CLEAR_COUNT)
    {
      PorcessPacket_Rf_Clear();
      cRfRXBuffClearCount = 0;
      cRxdataCount = 0;
      AutomanReceiveErrorCount++;

      if((huart3.ErrorCode & HAL_UART_ERROR_ORE) == HAL_UART_ERROR_ORE){
        HAL_UART_MspDeInit(&huart3);
        MX_USART3_UART_Init();
        HAL_UART_Receive_DMA(&huart3, buffUartRxData, 1);        
      }
      
      if(CONFIG_RESET_PROCESS){         //s 리셋기능 추가
        if(AutomanReceiveErrorCount > 600){  //s 30초동안 수신을 받지 못한다면
          CONFIG_RELAY_SAVE = SwPanelRelayControl(&RelayControlData);
          CONFIG_PANDING_SAVE = cPendingRelayState;
          CONFIG_RESET_SAVE = SET;
          CONFIG_PANDING_TIME1 = StrInput.Input[1].Time / 50;
          CONFIG_PANDING_TIME2 = StrInput.Input[2].Time / 50;    
          CONFIG_PANDING_TIME3 = StrInput.Input[3].Time / 50;
          save_all_eeprom();
          HAL_Delay(10);
          NVIC_SystemReset();       //s 리셋  
        }
      }
      
      memset(buffUartRxDataSave, 0, sizeof(buffUartRxDataSave));      
//      HAL_UART_Receive_IT(&huart3, buffUartRxData, 1);   

    }

    if(cLightRxdataCount)
    {
       for(i = 0; i < cLightRxdataCount; i++)
       {
          cLightRxComplteState = PorcessPacketLightCheck(buffLightRxDataSave[i], buffLightRxRfData);
       }
       cLightRxdataCount = 0;
       memset(buffLightRxDataSave, 0, sizeof(buffLightRxDataSave));
       cLightRXBuffClearCount = 0;
    }
    if(cLightRXBuffClearCount > SERIAL_RF_RX_CLEAR_COUNT)
    {
      PorcessPacketLightClear();
      cLightRXBuffClearCount = 0;
      cLightRxdataCount = 0;
      memset(buffLightRxDataSave, 0, sizeof(buffLightRxDataSave));      
      //HAL_UART_Receive_IT(&huart1, buffLightRxData, 1);   
    }    
      
    if(cMenuControl >= 1)            //s glcd display
    {
      setting_mode();
      cMenuControl =0;
    }
      
    
    if(cRxComplteState)
    {

      if(CONFIG_RF_TYPE == AUTOMAN_RF_CONTROL || CONFIG_RF_TYPE == AUTOMAN3025_RF_CONTROL)
      {  
         if(cRxComplteState == RF_MAIN_CMD_REQ)
         {
           
           if(AutomodeSendFlag == SET && (strRfTxData.Cmd == RF_SUB_PENDING_AUTO_STOP || strRfTxData.Cmd == RF_SUB_PENDING_AUTO_START)){
             AutomodeSendFlag = RESET;
           }
           else{
            strRfTxData.Cmd = RF_SUB_CMD_REQ;//lee                
           }  
            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
              strRfTxData.Mode = 20;                
            }
            else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
              strRfTxData.Mode = DEVICE_TYPE_8CH;               
            }           
            else{
              strRfTxData.Mode = CONFIG_DEVIEC_TPYE;                  
            }
        
            strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);                 
           
            cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
            cRfTxDelayState = RF_MAIN_CMD_REQ; 
            
//            cBuzzerOnOff = SET;
//            cBuzzerCount = BUZZER_TIME_COUNT;   
         }
         else if(cRxComplteState == RF_MAIN_CMD_ALIVE)
         {
           if(AutomodeSendFlag == SET && (strRfTxData.Cmd == RF_SUB_PENDING_AUTO_STOP || strRfTxData.Cmd == RF_SUB_PENDING_AUTO_START)){
             AutomodeSendFlag = RESET;
           }
           else{
            strRfTxData.Cmd = RF_MAIN_CMD_ALIVE;                 
           }

            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
              strRfTxData.Mode = DEVICE_TYPE_PENDING;                
            }
            else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
              strRfTxData.Mode = DEVICE_TYPE_8CH;               
            }           
            else{
              strRfTxData.Mode = CONFIG_DEVIEC_TPYE;                  
            }
            strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);                 
           
            cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
            cRfTxDelayState = RF_MAIN_CMD_ALIVE; 
            
            cBuzzerOnOff = SET;
            cBuzzerCount = BUZZER_TIME_COUNT;   
         }         
         else if(cRxComplteState == RF_MAIN_CMD_CHANGE)
         {
            strRfRxData.Cmd = RF_SUB_CMD_CHANGE;
            strRfRxData.Relay = buffUartRxRfData[RF_DATA_POS_RELAY] - 1;
            strRfRxData.RelayTimer = buffUartRxRfData[RF_DATA_POS_RELAYTIME1];
            strRfRxData.RelayTimer |= (uint16_t)buffUartRxRfData[RF_DATA_POS_RELAYTIME2] << 8;
            
            strRfTxData.Cmd = RF_SUB_CMD_CHANGE;
            
            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
              strRfTxData.Mode = DEVICE_TYPE_PENDING;                
            }
            else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
              strRfTxData.Mode = DEVICE_TYPE_8CH;               
            }
            else{
              strRfTxData.Mode = CONFIG_DEVIEC_TPYE;                  
            }           
            SwPanelRfDataChange(&RelayControlData, strRfRxData.Relay, strRfRxData.RelayTimer);
            GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
            
            strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
            
            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL){
              if((strRfRxData.Relay == SW_CONTROL_LIGHT1) || (strRfRxData.Relay == SW_CONTROL_LIGHT2)){
                //s 라이팅 pwm 제어 파트
                if((strRfRxData.Relay == SW_CONTROL_LIGHT1) && (strRfRxData.RelayTimer != 0)){   //s strRfRxData.RelayTimer 이 0이 아니면 on
                
                  if(cLightOnFlag[0] != SET){
                    cLightOnFlag[0] = SET;
                    cLightOffFlag[0] = RESET;						
                    wLightOnCount[0] = 400;
                    wLightOnProcCount[0] = 0;  
                    wLightOnPercent[0] = 50;                     
                  }
                  else{
                    cLightOffFlag[0] = RESET;
                  }
                  SendPacketLight(1, SET, 1, 100);      //s 디밍기에 데이터 보냄                     
                }
                else if((strRfRxData.Relay == SW_CONTROL_LIGHT1) && (strRfRxData.RelayTimer == 0)){   //s strRfRxData.RelayTimer 이 0이 이면 off
                  if(cLightOffFlag[0] != SET){
                    cLightOffFlag[0] = SET;
                    cLightOnFlag[0] = RESET;						
                    wLightOffCount[0] = 400;
                    wLightOffProcCount[0] = 0;
                    wLightOnPercent[0] = 50;                       
                  }
                  else{
                    cLightOnFlag[0] = RESET;
                  }					
                  SendPacketLight(1, RESET, 1, 100);      //s 디밍기에 데이터 보냄            
                }			
                
                if((strRfRxData.Relay == SW_CONTROL_LIGHT2) && (strRfRxData.RelayTimer != 0)){
        
                  if(cLightOnFlag[1] != SET){
                    cLightOnFlag[1] = SET;
                    cLightOffFlag[1] = RESET;						
                    wLightOnCount[1] = 400; 
                    wLightOnProcCount[1] = 0;   
                    wLightOnPercent[0] = 50;                     
                  }
                  else{
                    cLightOffFlag[1] = RESET;
                  }
                  SendPacketLight(2, SET, 1, 100);      //s 디밍기에 데이터 보냄              
                }
                else if((strRfRxData.Relay == SW_CONTROL_LIGHT2) && (strRfRxData.RelayTimer == 0)){
                  if(cLightOffFlag[1] != SET){
                    cLightOffFlag[1] = SET;
                    cLightOnFlag[1] = RESET;						
                    wLightOffCount[1] = 400; 	
                    wLightOffProcCount[0] = 0;
                    wLightOnPercent[0] = 50;                   
                  }
                  else{
                    cLightOnFlag[1] = RESET;
                  }					
                  SendPacketLight(2, RESET, 1, 100);      //s 디밍기에 데이터 보냄                   
                }	                
                
              }
              
            }
            cRelaySendState = RELAY_DATA_SEND;
            cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT + 1;           //s 20ms timer
            cRfTxDelayState = RF_MAIN_CMD_CHANGE;
            cBuzzerOnOff = SET;
            cBuzzerCount = BUZZER_TIME_COUNT;              
         }
         else if(cRxComplteState == RF_MAIN_CMD_LCD)
         {
            strRfRxData.Cmd = RF_SUB_CMD_LCD;
            strRfRxData.Relay = buffUartRxRfData[RF_DATA_POS_RELAY] - 1;
            strRfRxData.RelayTimer = buffUartRxRfData[RF_DATA_POS_RELAYTIME1];
            strRfRxData.RelayTimer |= (uint16_t)buffUartRxRfData[RF_DATA_POS_RELAYTIME2] << 8;
            
            strRfTxData.Cmd = RF_SUB_CMD_LCD;
            
            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
              strRfTxData.Mode = DEVICE_TYPE_PENDING;                
            }
            else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
              strRfTxData.Mode = DEVICE_TYPE_8CH;               
            }
            else{
              strRfTxData.Mode = CONFIG_DEVIEC_TPYE;                  
            }           
              SwPanelRfDataChange(&RelayControlData, strRfRxData.Relay, strRfRxData.RelayTimer);
            GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
            
            strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
            
            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL){
              if((strRfRxData.Relay == SW_CONTROL_LIGHT1) || (strRfRxData.Relay == SW_CONTROL_LIGHT2)){
                //s 라이팅 pwm 제어 파트
                if((strRfRxData.Relay == SW_CONTROL_LIGHT1) && (strRfRxData.RelayTimer != 0)){   //s strRfRxData.RelayTimer 이 0이 아니면 on
                
                  if(cLightOnFlag[0] != SET){
                    cLightOnFlag[0] = SET;
                    cLightOffFlag[0] = RESET;						
                    wLightOnCount[0] = 400;
                    wLightOnProcCount[0] = 0;  
                    wLightOnPercent[0] = 50;                     
                  }
                  else{
                    cLightOffFlag[0] = RESET;
                  }
                  SendPacketLight(1, SET, 1, 100);      //s 디밍기에 데이터 보냄                     
                }
                else if((strRfRxData.Relay == SW_CONTROL_LIGHT1) && (strRfRxData.RelayTimer == 0)){   //s strRfRxData.RelayTimer 이 0이 이면 off
                  if(cLightOffFlag[0] != SET){
                    cLightOffFlag[0] = SET;
                    cLightOnFlag[0] = RESET;						
                    wLightOffCount[0] = 400;
                    wLightOffProcCount[0] = 0;
                    wLightOnPercent[0] = 50;                       
                  }
                  else{
                    cLightOnFlag[0] = RESET;
                  }					
                  SendPacketLight(1, RESET, 1, 100);      //s 디밍기에 데이터 보냄            
                }			
                
                if((strRfRxData.Relay == SW_CONTROL_LIGHT2) && (strRfRxData.RelayTimer != 0)){
        
                  if(cLightOnFlag[1] != SET){
                    cLightOnFlag[1] = SET;
                    cLightOffFlag[1] = RESET;						
                    wLightOnCount[1] = 400; 
                    wLightOnProcCount[1] = 0;   
                    wLightOnPercent[0] = 50;                     
                  }
                  else{
                    cLightOffFlag[1] = RESET;
                  }
                  SendPacketLight(2, SET, 1, 100);      //s 디밍기에 데이터 보냄              
                }
                else if((strRfRxData.Relay == SW_CONTROL_LIGHT2) && (strRfRxData.RelayTimer == 0)){
                  if(cLightOffFlag[1] != SET){
                    cLightOffFlag[1] = SET;
                    cLightOnFlag[1] = RESET;						
                    wLightOffCount[1] = 400; 	
                    wLightOffProcCount[0] = 0;
                    wLightOnPercent[0] = 50;                   
                  }
                  else{
                    cLightOnFlag[1] = RESET;
                  }					
                  SendPacketLight(2, RESET, 1, 100);      //s 디밍기에 데이터 보냄                   
                }	                
                
              }
              
            }
            cRelaySendState = RELAY_DATA_SEND;
            cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
            cRfTxDelayState = RF_MAIN_CMD_LCD;
            cBuzzerOnOff = SET;
            cBuzzerCount = BUZZER_TIME_COUNT;              
         }
         else if(cRxComplteState == RF_MAIN_CMD_CONTROL)
         {
            strRfRxData.Cmd = buffUartRxRfData[RF_DATA_POS_CMD];
            strRfRxData.Relay = buffUartRxRfData[RF_DATA_POS_RELAY] - 1;
            strRfRxData.RelayTimer = buffUartRxRfData[RF_DATA_POS_RELAYTIME1];
            strRfRxData.RelayTimer |= (uint16_t)buffUartRxRfData[RF_DATA_POS_RELAYTIME2] << 8;
            
            strRfRxData.InTimer1 = buffUartRxRfData[RF_DATA_POS_INTIMER1L];
            strRfRxData.InTimer1 |= (uint16_t)buffUartRxRfData[RF_DATA_POS_INTIMER1H] << 8;
            strRfRxData.InTimer2 = buffUartRxRfData[RF_DATA_POS_INTIMER2L];
            strRfRxData.InTimer2 |= (uint16_t)buffUartRxRfData[RF_DATA_POS_INTIMER2H] << 8;
            
            if(strRfRxData.InTimer2 > 100){             //s persent
              strRfRxData.InTimer2 = 100;
            }
            else if(strRfRxData.InTimer2 < 1){
              strRfRxData.InTimer2 = 1;
            }
            if(strRfRxData.Relay == SW_CONTROL_LIGHT1){
              wLightUartTime[0] = strRfRxData.InTimer1; 
              cLightUartPercent[0] = strRfRxData.InTimer2;
              wLightOnPercent[0] = strRfRxData.InTimer2 / 2;      
            }
            else if(strRfRxData.Relay == SW_CONTROL_LIGHT2){
              wLightUartTime[1] = strRfRxData.InTimer1; 
              cLightUartPercent[1] = strRfRxData.InTimer2;              
              wLightOnPercent[1] = strRfRxData.InTimer2 / 2;                
            }
            
            strRfTxData.Cmd = RF_SUB_CMD_CONTROL;
            strRfTxData.Mode = CONFIG_DEVIEC_TPYE;            
            SwPanelRfDataChange(&RelayControlData, strRfRxData.Relay, strRfRxData.RelayTimer);
            GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
            
            strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
            cRelaySendState = RELAY_DATA_SEND;
            cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
            cRfTxDelayState = RF_MAIN_CMD_CONTROL;
			
			
            //s 라이팅 pwm 제어 파트
            if((strRfRxData.Relay == SW_CONTROL_LIGHT1) && (strRfRxData.RelayTimer != 0)){   //s strRfRxData.RelayTimer 이 0이 아니면 on
    
              if(cLightOnFlag[0] != SET){
                cLightOnFlag[0] = SET;
                cLightOffFlag[0] = RESET;						
                wLightOnCount[0] = strRfRxData.InTimer1 * 400;
                wLightOnProcCount[0] = 0;   
              }
              else{
                cLightOffFlag[0] = RESET;
              }
              sprintf(buffGLcd1Line, "LIGHT:1 ON %03d", cLightUartPercent[0]);
              TM_HD44780_Puts_State(0,1,buffGLcd2Line);               
              SendPacketLight(1, SET, wLightUartTime[0], cLightUartPercent[0]);      //s 디밍기에 데이터 보냄                     
            }
            else if((strRfRxData.Relay == SW_CONTROL_LIGHT1) && (strRfRxData.RelayTimer == 0)){   //s strRfRxData.RelayTimer 이 0이 이면 off
              if(cLightOffFlag[0] != SET){
                cLightOffFlag[0] = SET;
                cLightOnFlag[0] = RESET;						
                wLightOffCount[0] = strRfRxData.InTimer1 * 400;
                wLightOffProcCount[0] = 0;
              }
              else{
                cLightOnFlag[0] = RESET;
              }		
              sprintf(buffGLcd1Line, "LIGHT:1 OFF %03d", cLightUartPercent[0]);              
              SendPacketLight(1, RESET, wLightUartTime[0], cLightUartPercent[0]);      //s 디밍기에 데이터 보냄            
            }			
            
            if((strRfRxData.Relay == SW_CONTROL_LIGHT2) && (strRfRxData.RelayTimer != 0)){
    
              if(cLightOnFlag[1] != SET){
                cLightOnFlag[1] = SET;
                cLightOffFlag[1] = RESET;						
                wLightOnCount[1] = strRfRxData.InTimer1 * 400; 
                wLightOnProcCount[1] = 0;                    
              }
              else{
                cLightOffFlag[1] = RESET;
              }
              sprintf(buffGLcd1Line, "LIGHT:2 ON %03d", cLightUartPercent[1]);                 
              SendPacketLight(2, SET, wLightUartTime[1], cLightUartPercent[1]);      //s 디밍기에 데이터 보냄              
            }
            else if((strRfRxData.Relay == SW_CONTROL_LIGHT2) && (strRfRxData.RelayTimer == 0)){
              if(cLightOffFlag[1] != SET){
                cLightOffFlag[1] = SET;
                cLightOnFlag[1] = RESET;						
                wLightOffCount[1] = strRfRxData.InTimer1 * 400; 	
                wLightOffProcCount[0] = 0;
              
              }
              else{
                cLightOnFlag[1] = RESET;
              }		
              sprintf(buffGLcd1Line, "LIGHT:2 OFF %03d", cLightUartPercent[1]);                     
              SendPacketLight(2, RESET, wLightUartTime[1], cLightUartPercent[1]);      //s 디밍기에 데이터 보냄                   
            }				
			
            cBuzzerOnOff = SET;
            cBuzzerCount = BUZZER_TIME_COUNT;              
         }    
         else if(cRxComplteState == RF_MAIN_CMD_FAN_CONTROL)
         {
            strRfRxData.Cmd = buffUartRxRfData[RF_DATA_POS_CMD];
            strRfRxData.Relay = buffUartRxRfData[RF_DATA_POS_RELAY] - 1;
            strRfRxData.RelayTimer = buffUartRxRfData[RF_DATA_POS_RELAYTIME1];
            strRfRxData.RelayTimer |= (uint16_t)buffUartRxRfData[RF_DATA_POS_RELAYTIME2] << 8;
            
            strRfTxData.Cmd = RF_SUB_CMD_FAN_CONTROL;
            strRfTxData.Mode = CONFIG_DEVIEC_TPYE;            
            SwPanelRfDataChange(&RelayControlData, strRfRxData.Relay, strRfRxData.RelayTimer);
            GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);   
            
            strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
            cRelaySendState = RELAY_DATA_SEND;
            cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
            cRfTxDelayState = RF_MAIN_CMD_FAN_CONTROL;
	
//            cBuzzerOnOff = SET;
//            cBuzzerCount = BUZZER_TIME_COUNT;              
         }          
         else if(cRxComplteState == RF_MAIN_CMD_ACK)
         {
           cRfTxDelayCount = 0;
           cRfTxDelayState = RF_DATA_NONE;        
         } 
         
         
         PandingAutoMode();                     //s 계류식일 경우 Auto mode 셋팅
      }

      
      cRxComplteState = RELAY_DATA_NONE;
      //testvalue = atoi(testtmp);

    }
    
    
    if(cLightRxComplteState){
      if(buffLightRxRfData[0] == 1){
        cBuzzerOnOff = SET;
        cBuzzerCount = BUZZER_TIME_COUNT;              
      }
      cLightRxComplteState = 0;
    }
    
    
    if(cHc165ProcessFlag){              //s 키패드 값변화 감지
      cHc165ProcessFlag = RESET;
      SwPanelTask(&SwPanelData, buffSpiReadData, TIMER_1MS_TASK_IN_24V_READ);     
      if(SwPanelData.change == SET)
      {
         SwPanelData.change = RESET;   
         SwPanelAction(&SwPanelData, &RelayControlData);
         
        strRfRxData.Cmd = buffUartRxRfData[RF_DATA_POS_CMD];
        strRfRxData.Relay = buffUartRxRfData[RF_DATA_POS_RELAY] - 1;
        strRfRxData.RelayTimer = 0XFFFF;
//
//            strRfTxData.Cmd = RF_SUB_CMD_CHANGE;
//            strRfTxData.Mode = CONFIG_DEVIEC_TPYE;            
        //SwPanelRfDataChange(&RelayControlData, strRfRxData.Relay, strRfRxData.RelayTimer);
        GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA); 
        cRelaySendState = RELAY_DATA_SEND;
      }      
      
      cBuzzerOnOff = SET;
      cBuzzerCount = BUZZER_TIME_COUNT;      
    }
    
    if(Config.Value[1] == DEVICE_TYPE_CONTROL){    
      if(cLightTimingFlag){                       //s 전등 밝기 값 계산
        cLightTimingFlag = RESET;
        LightExcuTask();
      }
    }
    
    if((cRfProcessMode == 0) && (cRfSetProcessFlag == SET)){    
      cRfSetProcessFlag = RESET;
      RfAutomanSetProcess(CONFIG_RF_TYPE);
    }    
    
    
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART3)
  {
    buffUartRxDataSave[cRxdataCount] = buffUartRxData[0];
    cRxdataCount++;
//    HAL_UART_Receive_IT(&huart3, buffUartRxData, 1);
  }
  
  if(huart->Instance == USART1)
  {
    buffLightRxDataSave[cLightRxdataCount] = buffLightRxData[0];
    cLightRxdataCount++;
//    HAL_UART_Receive_IT(&huart1, buffLightRxData, 1);
  }  
  
}

void LightExcuTask(void)
{
      double fLightPwm;		//s on off 계산용
    for(uint8_t k = 0; k < 2; k++){ 
      if(cLightOnFlag[k] == SET){
        if(wLightOnCount[k] == wLightOnProcCount[k]){
          wLightOnTime[k] = wLightOnPercent[k];	//s 계속 켜짐
        }
        else{
          wLightOnProcCount[k]++;				
          fLightPwm = (double)wLightOnProcCount[k] / wLightOnCount[k];
          wLightOnTime[k] = (uint16_t)(fLightPwm * wLightOnPercent[k]);
        }
      }
      else if(cLightOffFlag[k] == SET){
        if(wLightOffCount[k] == wLightOffProcCount[k]){
          wLightOffTime[k] = 50;	//s 계속 꺼짐
        }
        else{
          wLightOffProcCount[k]++;				
          fLightPwm = (double)wLightOffProcCount[k] / wLightOffCount[k];
          wLightOffTime[k] = (uint16_t)(fLightPwm * 50);
          if(wLightOffTime[k] <= (50 - wLightOnPercent[k])){
            wLightOffTime[k] = (50 - wLightOnPercent[k]);
          }
        }		
      }
    }
}


void PendingTask(uint8_t state)
{
  
  switch(state){
  case PENDING_STATE_STOP:
   break; 
    
  case PENDING_STATE_START:		//s 1~3개 변경하게 수정됨 180807
	  if(CONFIG_MOVING_COUNT == 1){
		if(strRfTxData.Input & (GPIO_INPUT_EVENT2))
			  cPendingRelayState = PENDING_STATE_START_IN_CHECK;
	  }
	  else if(CONFIG_MOVING_COUNT == 2){
		if(strRfTxData.Input & (GPIO_INPUT_EVENT2))
		  if(strRfTxData.Input & (GPIO_INPUT_EVENT4))
			  cPendingRelayState = PENDING_STATE_START_IN_CHECK;  
	  }
	  else if(CONFIG_MOVING_COUNT == 3){
		if(strRfTxData.Input & (GPIO_INPUT_EVENT2))
		  if(strRfTxData.Input & (GPIO_INPUT_EVENT4))
			if(strRfTxData.Input & (GPIO_INPUT_EVENT6))         
			  cPendingRelayState = PENDING_STATE_START_IN_CHECK;  
	  }      	  
   break; 
  
  case PENDING_STATE_START_IN_CHECK:		//s 1~3개 변경하게 수정됨 180807  이송라인 수
	  if(CONFIG_MOVING_COUNT == 1){
    	if((strRfTxData.Input & (GPIO_INPUT_EVENT2)) == 0)	
			cPendingRelayState = PENDING_STATE_MOVING_STOP;
	  }
	  else if(CONFIG_MOVING_COUNT == 2){
		if((strRfTxData.Input & (GPIO_INPUT_EVENT2)) == 0)	
      		if((strRfTxData.Input & (GPIO_INPUT_EVENT4)) == 0)   
          		cPendingRelayState = PENDING_STATE_MOVING_STOP;		  
	  }
	  else if(CONFIG_MOVING_COUNT == 3){
		if((strRfTxData.Input & (GPIO_INPUT_EVENT2)) == 0)	
      		if((strRfTxData.Input & (GPIO_INPUT_EVENT4)) == 0)
        		if((strRfTxData.Input & (GPIO_INPUT_EVENT6)) == 0)      
          			cPendingRelayState = PENDING_STATE_MOVING_STOP;		  
	  }      
   break; 
   
  case PENDING_STATE_MOVING_STOP:
    if(CONFIG_MOVING_COUNT >= 1){             
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){ 
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING1_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA); 
      }
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){ 
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING1_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA); 
      }      
    }
    if(CONFIG_MOVING_COUNT >= 2){
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){       
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING2_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);     
      }
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){ 
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING2_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA); 
      }      
    }
    if(CONFIG_MOVING_COUNT >= 3){    
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){            
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_MOVEING3_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);
      }
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){ 
         SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_MOVEING3_OFF, RELAY_TIME_MATAIN);
         GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA); 
      }      
    }               
            
   strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
   cPendingRelayCount = 5;                     //s 다 찬후 5초 이후에 당김
   cPendingRelayState = PENDING_STATE_MOVING_STOP_WAIT;
//   RelayControlData.time[SW_PANDING_PANDING_PULL] = 10;
   cRelaySendState = RELAY_DATA_SEND;

  
    break; 

  case PENDING_STATE_MOVING_STOP_WAIT:
   if(cPendingRelayCount == 0)
   {
    cPendingRelayState = PENDING_STATE_PEED_START;
   }
   break;     

  case PENDING_STATE_PEED_START:
   strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
   if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){ 
     AutomodeSendFlag = SET;  //s 오토 샌드 플레그     
     strRfTxData.Cmd = RF_SUB_PENDING_AUTO_STOP;
     strRfTxData.Mode = DEVICE_TYPE_PENDING;       
     
//     SwPanelRfDataChange(&RelayControlData, SW_PANDING_PANDING_PULL, RELAY_TIME_MATAIN);
//     GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
//             
     SwPanelRfDataChange(&RelayControlData, SW_PANDING_PULL, RELAY_TIME_MATAIN);
     GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  

     
     
     strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
     
     cPendingRelayState = PENDING_STATE_STOP;     
     
     cRfTxDelayState = RF_DATA_NONE;       
     cRelaySendState = RELAY_DATA_SEND;
     cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
     cRfTxDelayState = RF_MAIN_PENDING_AUTO_STOP;   
   }
   else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
     AutomodeSendFlag = SET;  //s 오토 샌드 플레그     
     strRfTxData.Cmd = RF_SUB_PENDING_AUTO_STOP;
     strRfTxData.Mode = DEVICE_TYPE_PENDING;  
     
     SwPanelRfDataChange(&RelayControlData, SW_PANDING_8CH_PULL, RELAY_TIME_MATAIN);
     GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  

     strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
     
     cPendingRelayState = PENDING_STATE_STOP;     
     
     cRfTxDelayState = RF_DATA_NONE;       
     cRelaySendState = RELAY_DATA_SEND;
     cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
     cRfTxDelayState = RF_MAIN_PENDING_AUTO_STOP;      
   }
   break;   
   
//  case PENDING_STATE_PEED_WAIT:
//   if(RelayControlData.time[SW_PANDING_PANDING_PULL] == 0)
//   {
//    cPendingRelayState = PENDING_STATE_PEED_END;
//   }
//   break;       
//   
//  case PENDING_STATE_PEED_END:
//    if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){
//     if(RelayControlData.time[SW_PANDING_PANDING_PULL] == 0)
//     {
//       AutomodeSendFlag = SET;  //s 오토 샌드 플레그
//       strRfTxData.Cmd = RF_SUB_PENDING_AUTO_STOP;
//       strRfTxData.Mode = CONFIG_DEVIEC_TPYE;  
//
//       SwPanelRfDataChange(&RelayControlData, SW_PANDING_PANDING_ANNEAL, RELAY_TIME_MATAIN);
//       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
//               
//       SwPanelRfDataChange(&RelayControlData, SW_PANDING_PULL, RELAY_TIME_OFF);
//       GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_TASK_UPDATA);  
//                              
//       strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
//       
//       //cPendingRelayState = PENDING_STATE_CMD_STOP;
//       cPendingRelayState = PENDING_STATE_STOP;
//
//       cRfTxDelayState = RF_DATA_NONE;       
//       cRelaySendState = RELAY_DATA_SEND;
//       cRfTxDelayCount = AUTOMAN_TX_DELAY_COUNT;           //s 20ms timer
//       cRfTxDelayState = RF_MAIN_PENDING_AUTO_STOP;      
//       
//     }   
//    }
//   break;    
  }
  
  
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
  
//  uint8_t SwitchBack;
//  uint8_t i;
  
  if(htim->Instance == TIM2)
  {
    if(Timer10msTaskCount == TIMER_10MS_TASK_ALIVE_LED)
    {
//      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);     
      wLedCount++;
      //printf("wLedCount [%d]\r\n", wLedCount);
        //s for test
        //Config.Value[20] = wLedCount;
	//save_all_eeprom();                    //s 변경값 저장            
    }      
    else if(Timer10msTaskCount == TIMER_10MS_TASK_IN_COUNT)
    {   
      

    }
    else if(Timer10msTaskCount == TIMER_10MS_TASK_GLCD_REDISPALY)
    {
      
      if((cVersionCount == 0) && (cVersionDisplayEnd == RESET)){             //s end version display
        cVersionDisplayEnd = SET;
        cReLcdDisplay = 0;     //s start main display 
        MeunTitleCopy();
        //cReLcdDisplay = 10;     //s start main display 
      }      
      else if(cVersionCount){           //s protect underflow 
        cVersionCount--;      
      }      
      
      cReLcdDisplay++;            
      if(cReLcdDisplay > 10){
        cReLcdDisplay = 0;
        TM_HD44780_Puts_State(0,0,buffGLcd1Line);
        TM_HD44780_Puts_State(0,1,buffGLcd2Line);        
      }
      
    }          
    else if(Timer10msTaskCount == TIMER_10MS_TASK_RELAY_TIMER)
    {
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){                //s 계류기와 계사 농장관리만 사용
         if(GpioRelayControlTask(&RelayControlData, &SwPanelData, RELAY_DATA_TIMER_1S)){
            cRelaySendState = RELAY_DATA_SEND;
         }      
      }
      
      if(cPendingRelayCount){                   //s 당김장치 딜레이로 추가됨
        cPendingRelayCount--;
      }
      
    }        

    Timer10msTaskCount++;
    
    if(Timer10msTaskCount >= TIMER_10MS_TASK_MAX)
      Timer10msTaskCount = TIMER_10MS_TASK_ALIVE_LED;
   
    
  }
  
  if(htim->Instance == TIM3)
  {
    
    switch(Timer1msTaskCount)
    {
    case TIMER_1MS_TASK_IN_MENU:
    case TIMER_1MS_TASK_IN_MENU2:
      
      SW_check();
      break;
        
    case TIMER_1MS_TASK_CHECK_MODE_CHANGE:
      if(cSaveConfigType != CONFIG_DEVIEC_TPYE && Setting_enable == 0){
        
        if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
          eeprom_pend_default_value();
          save_all_eeprom();
        }
        else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
          eeprom_8ch_limit_default_value();
          save_all_eeprom();          
        }
        
        NVIC_SystemReset();    
      }
      
      if(cSaveConfigReverse != CONFIG_8CH_REVERSE && Setting_enable == 0){
        NVIC_SystemReset();          
      }
      
      
      break;
      
    case TIMER_1MS_TASK_IN_24V_LOAD:

      break;
      
    case TIMER_1MS_TASK_IN_24V:

      break;

    case TIMER_1MS_TASK_IN_SIG:
 
      break;      
      
    case TIMER_1MS_TASK_IN_24V_READ:
      if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){            //s only use DEVICE_TYPE_PENDING type
        GpioInputTask(&StrInput);
        if(StrInput.Event)
        {
//          strRfTxData.Cmd = RF_SUB_CMD_INPUT;
          strRfTxData.Relay = SwPanelRelayControl(&RelayControlData);
          strRfTxData.Input = StrInput.KeyData;
//          if(StrInput.Event & 0x02)             //s when Event are input 1~2. it must be clear 
//          {
//            StrInput.Input[1].Time = 0;
//          }
//          if(StrInput.Event & 0x08)
//          {
//            StrInput.Input[3].Time = 0;
//          }
//          if(StrInput.Event & 0x20)
//          {
//            StrInput.Input[5].Time = 0;
//          }
//          StrInput.Event = 0;
//          SendPacket_Rf(&strRfTxData);     
//          cRfTxDelayCount = AUTOMAN_TX_ACK_DELAY_COUNT;         // every 1sec try        
//          cRfTxDelayState = RF_SUB_CMD_INPUT;
//          cRfTxEventSendCount = AUTOMAN_TX_EVENT_COUNT;         //s try to 5 times
//          cBuzzerOnOff = SET;
//          cBuzzerCount = BUZZER_TIME_COUNT;             
        }
        strRfTxData.InTimer1 = StrInput.Input[1].Time / 50;                   //s make 1sec value
        strRfTxData.InTimer2 = StrInput.Input[3].Time / 50; 
        strRfTxData.InTimer3 = StrInput.Input[5].Time / 50;           

        PendingTask(cPendingRelayState);                        //s pending type task
        
      }
     break;   

     
    case TIMER_1MS_TASK_OUT_RELAY_PENDING:      
      
      break;
     
    case TIMER_1MS_TASK_OUT_RELAY:
      if(cRelayUploadCount > 25)
      {
        cRelaySendState = RELAY_DATA_SEND;
      }
      
      if(cRelaySendState == RELAY_DATA_SEND)
      {
        cRelayUploadCount = 0;
        cRelaySendState = RELAY_DATA_END;
      }
      break;
    case TIMER_1MS_TASK_OUT_RELAY_LATCH:
      if(cRelaySendState == RELAY_DATA_END)
      {
        char buff[17];
        uint8_t i; 
        memset(buff, 0 , 17);
        cRelaySendState = RELAY_DATA_NONE;
        cReLcdDisplay = 0;              
        if(Setting_enable == 0)
        {
//         if(CONFIG_8CH_REVERSE){
//            for(i = 0; i < 16; i++){
//              if(i % 2){
//                if(RelayControlData.pin[i] == RESET){
//                  buff[i-1] = '0';              
//                }
//                else{
//                  buff[i-1] = '1';                
//                }
//              }
//              else{
//                if(RelayControlData.pin[i] == RESET){
//                  buff[i+1] = '0';              
//                }
//                else{
//                  buff[i+1] = '1';                
//                }
//              }
//            }
//         }
//         else{
            for(i = 0; i < 16; i++){
              if(RelayControlData.pin[i] == RESET){
                buff[i] = '0';              
              }
              else{
                if(RelayControlData.time[i]){
                  buff[i] = '1';             
                }
                else{
                  buff[i] = 'S';                    
                }
              }
            }
//         }
          memcpy(buffGLcd2Line, buff, GLCD_BUFF_SIZE-1);
          TM_HD44780_Puts_State(0,1,buffGLcd2Line); 
        }
      }    
      break;

    case TIMER_1MS_TASK_IN_ADC_START: 
      break;
      
    case TIMER_1MS_TASK_RF_SETTING:
      cRfSetProcessFlag = SET;
      break;
      
      
    case TIMER_1MS_TASK_OUT_LCD:
      cMenuControl++;
      break;

    case TIMER_1MS_TASK_OUT_RF:
      if(cRfTxDelayState)
      {
        if(cRfTxDelayCount-- == 0)
        {
          if(cRfTxDelayState == RF_MAIN_CMD_REQ)
          { 
            SendPacket_Rf(&strRfTxData);
            cRfTxDelayState = RF_DATA_NONE;   
            if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){            //s only use DEVICE_TYPE_PENDING type
              if(StrInput.Event)
              {         
                if(StrInput.Event & 0x02)             //s when Event are input 1~2. it must be clear 
                {
                  StrInput.Input[1].Time = 0;
                }
                if(StrInput.Event & 0x08)
                {
                  StrInput.Input[3].Time = 0;
                }
                if(StrInput.Event & 0x20)
                {
                  StrInput.Input[5].Time = 0;
                }
                StrInput.Event = 0;               
              }
            }
            
              //printf("cRfTxDelayState == RF_MAIN_CMD_REQ\r\n");
          }  
          else if(cRfTxDelayState == RF_MAIN_CMD_ALIVE)
          {
            SendPacket_Rf(&strRfTxData);
            cRfTxDelayState = RF_DATA_NONE;            
            //printf("cRfTxDelayState == RF_MAIN_CMD_ALIVE\r\n");
          }            
          else if(cRfTxDelayState == RF_MAIN_CMD_CHANGE)
          {
            SendPacket_Rf(&strRfTxData);   
            cRfTxDelayState = RF_DATA_NONE;            
              //printf("cRfTxDelayState == RF_MAIN_CMD_CHANGE\r\n");            
          }
          else if(cRfTxDelayState == RF_MAIN_CMD_CONTROL)
          {
            SendPacket_Rf(&strRfTxData);   
            cRfTxDelayState = RF_DATA_NONE;            
              //printf("cRfTxDelayState == RF_MAIN_CMD_CHANGE\r\n");            
          }  
          else if(cRfTxDelayState == RF_MAIN_CMD_FAN_CONTROL)
          {
            SendPacket_Rf(&strRfTxData);   
            cRfTxDelayState = RF_DATA_NONE;            
              //printf("cRfTxDelayState == RF_MAIN_CMD_CONTROL\r\n");            
          }            
          else if(cRfTxDelayState == RF_MAIN_PENDING_AUTO_START)
          {
            SendPacket_Rf(&strRfTxData);   
            cRfTxDelayState = RF_DATA_NONE;            
              //printf("cRfTxDelayState == RF_MAIN_PENDING_AUTO_START\r\n");            
          }
          else if(cRfTxDelayState == RF_MAIN_PENDING_AUTO_STOP)
          {
            SendPacket_Rf(&strRfTxData);   
            cRfTxDelayState = RF_DATA_NONE;            
              //printf("cRfTxDelayState == RF_MAIN_PENDING_AUTO_STOP\r\n");            
          }          
          else if(cRfTxDelayState == RF_SUB_CMD_INPUT)
          {
            cBuzzerOnOff = SET;
            cBuzzerCount = BUZZER_TIME_COUNT;               
            SendPacket_Rf(&strRfTxData);           
            if(cRfTxEventSendCount == 0){
              cRfTxDelayState = RF_DATA_NONE;               
            }
            else{  
              cRfTxDelayCount = AUTOMAN_TX_ACK_DELAY_COUNT;            
              cRfTxEventSendCount--;
            }
            //printf("cRfTxDelayState == RF_SUB_CMD_INPUT\r\n");            
          }
          else if(cRfTxDelayState == RF_MAIN_CMD_LCD)
          {
            SendPacket_Rf(&strRfTxData);   
            cRfTxDelayState = RF_DATA_NONE;
              //printf("cRfTxDelayState == RF_MAIN_CMD_CONTROL\r\n");            
          }     
        }
      }
      cRfRXBuffClearCount++;
      cLightRXBuffClearCount++;      
      break;
  
    case TIMER_1MS_TASK_OUT_BUZZER:
      
      if(cBuzzerOnOff == SET)
      {
        HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_SET);
        cBuzzerOnOff = RESET;  
      }
      else if((cBuzzerOnOff == RESET) && cBuzzerCount)
      {
        cBuzzerCount--;
        if(cBuzzerCount == 0)
        {
          HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_RESET);          
        } 
      }     
      
      break;
      
    default:
      break;
      
    }
    
    Timer1msTaskCount++;
    
    if(Timer1msTaskCount >= TIMER_1MS_TASK_MAX)
      Timer1msTaskCount = TIMER_1MS_TASK_IN_MENU;
  }  
  
  if(htim->Instance == TIM1)
  {
//    uint8_t lightvalue;
    uint16_t relayvalueback;
    
    if(Config.Value[1] == DEVICE_TYPE_16CH || Config.Value[1] == DEVICE_TYPE_8CH || Config.Value[1] == DEVICE_TYPE_8CH_LIMIT){ 
      uint8_t readevent = 0;
      Hc165Task();
      readevent = hc595OutTask(wRelayLedValue);
      
      if(readevent){
        relayvalueback = SwPanelRelayControl(&RelayControlData);              
        if(Config.Value[1] == DEVICE_TYPE_16CH){
          wRelayLedValue = SwPanelLedControl16ChReverse(relayvalueback);  
          //wRelayLedValue = relayvalueback;
        }
        else{ 
          wRelayLedValue = SwPanelLedControlReverse(relayvalueback);          
        }

      }
    }


//    if(Config.Value[1] == DEVICE_TYPE_CONTROL){                       //s 삭제
//      if(cLightOnFlag[0] == SET){
//        lightvalue = wLightOnTime[0];        
//        if(wLightOnTime[0] <= 5){
//          lightvalue = 5; //s 플리커 방지
//        }
//
//        if(lightvalue < wTimer50usCont){
//          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET); 
//        }
//        else{
//          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET); 
//        }
//      }
//      else if(cLightOffFlag[0] == SET){
//        lightvalue = wLightOffTime[0];
//        if((wLightOffTime[0] >= 45)){
//          if(wLightOffTime[0] != 50){
//            lightvalue = 45; //s 플리커 방지
//          }        
//        }
//        if(lightvalue < wTimer50usCont){
//                HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET); 
//        }
//        else{
//                HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET); 
//        }	
//      }
//                      
//      if(cLightOnFlag[1] == SET){
//        lightvalue = wLightOnTime[1];         
//        if(wLightOnTime[1] <= 5){
//          lightvalue = 5; //s 플리커 방지
//        }      
//        
//        if(lightvalue < wTimer50usCont){
//                HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET); 
//        }
//        else{
//                HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET); 
//        }
//      }
//      else if(cLightOffFlag[1] == SET){
//        lightvalue = wLightOffTime[1];        
//        if((wLightOffTime[0] >= 45)){
//          if(wLightOffTime[0] != 50){
//            lightvalue = 45; //s 플리커 방지
//          }        
//        }    
//        
//        if(wLightOffTime[1] < wTimer50usCont){
//                HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET); 
//        }
//        else{
//                HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET); 
//        }	
//      }
//      
//
//      wTimer50usCont++;	  
//
//      if(wTimer50usCont >= 50){
//        wTimer50usCont = 0;
//        cLightTimingFlag = SET;
//      }
//    }
    
    
  }  
  
//  if(htim->Instance == TIM5)		//s 100us 루틴		//s 50 단계설정 200hz stm32f103rbt6은 timer 5가 없음
//  {
//	  
//
//  }
  
  
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
//  while(1)
//  {
    //printf("error_%s[%d]\r\n", file, line);        
//    HAL_Delay(1000);
//  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
