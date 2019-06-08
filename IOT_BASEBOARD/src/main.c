/**
 * @file main.c
 * @brief Main routine
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Eval.
 *
 * This software is provided in source form for a short-term evaluation only. The
 * evaluation license expires 90 days after the date you first download the software.
 *
 * If you plan to use this software in a commercial product, you are required to
 * purchase a commercial license from Oryx Embedded SARL.
 *
 * After the 90-day evaluation period, you agree to either purchase a commercial
 * license or delete all copies of this software. If you wish to extend the
 * evaluation period, you must contact sales@oryx-embedded.com.
 *
 * This evaluation software is provided "as is" without warranty of any kind.
 * Technical support is available as an option during the evaluation period.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.2
 **/

//Dependencies
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "stm32f4xx_nucleo_144.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mac/stm32f4x9_eth_driver.h"
#include "drivers/phy/lan8742_driver.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "mqtt/mqtt_client.h"
#include "rng/yarrow.h"
#include "debug.h"
#include "parson.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "rtc.h"
#include "lcd_hd44780.h"
#include "eeprom_4seri.h"
#include "menu.h"
#include "Http_Read_parser.h"
#include "rf_modules.h"
#include "Lora_Sensor.h"
#include "dma.h"
#include "lcd.h"

//Application configuration
#if 1     // 김과장님

#define APP_MAC_ADDR "00-A1-C2-E3-07-69"
#define APP_IPV4_HOST_ADDR "192.168.1.200"
//#define MY_ID_NAME "id:newface442"

#else     // 허과장

#define APP_MAC_ADDR "00-AB-CD-EF-F2-81"
#define APP_IPV4_HOST_ADDR "192.168.1.78"
#define MY_ID_NAME "id:youngss80"

#endif

//#define APP_USE_DHCP DISABLED//ENABLED
#define APP_USE_DHCP ENABLED
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.1.1"
#define APP_IPV4_PRIMARY_DNS "192.168.1.1"        //8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "168.126.63.1"     //"8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::429"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::429"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"
uint8_t lcd_view_flag = 0;
//MQTT server name
//#define APP_SERVER_NAME "iot.eclipse.org"

//#define APP_SERVER_NAME "192.168.1.128"  // 노트북

//#define APP_SERVER_NAME "192.168.1.11"  // 전주 

//#define APP_SERVER_NAME "192.168.1.36"  // 연구소본체.

//#define APP_SERVER_NAME "192.168.1.178"
#define APP_SERVER_NAME "103.60.126.23"		//s 리눅스 서버
//MQTT server port
#define APP_SERVER_PORT 1883   //MQTT over TCP
//#define APP_SERVER_PORT 8883 //MQTT over SSL/TLS
//#define APP_SERVER_PORT 80   //MQTT over WebSocket
//#define APP_SERVER_PORT 443  //MQTT over secure WebSocket

//URI (for MQTT over WebSocket only)
#define APP_SERVER_URI "/ws"

//List of trusted CA certificates
char_t trustedCaList[] =
   "-----BEGIN CERTIFICATE-----"
   "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/"
   "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT"
   "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow"
   "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD"
   "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB"
   "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O"
   "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq"
   "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b"
   "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw"
   "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD"
   "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV"
   "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG"
   "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69"
   "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr"
   "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz"
   "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5"
   "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo"
   "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ"
   "-----END CERTIFICATE-----";

//Connection states
#define APP_STATE_NOT_CONNECTED 0
#define APP_STATE_CONNECTING    1
#define APP_STATE_CONNECTED     2

//Global variables
uint_t connectionState = APP_STATE_NOT_CONNECTED;

RNG_HandleTypeDef RNG_Handle;
IWDG_HandleTypeDef hiwdg;

DhcpClientSettings dhcpClientSettings;
DhcpClientContext dhcpClientContext;
SlaacSettings slaacSettings;
SlaacContext slaacContext;
MqttClientContext mqttClientContext;
YarrowContext yarrowContext;
uint8_t seed[32];

//s http get start
uint8_t cHttpReadFlag = 0;
uint8_t cHttpTimeReadFlag = 0;
//uint8_t cHttpReadState = HTTP_READ_NONE;

uint8_t mqtt_connect = RESET;
char MyTopic[20] = {0,};
char MyInfoTopic[30] = {0,};
uint16_t myFcode = 3350;
uint16_t myDcode = 1;

int arrivedcount = 0;
uint32_t iAutomanCheckCount;


uint32_t mqtt_sub_msec = 60000L;			//s mqtt 재설정 1분 셋팅

uint8_t lora_debug = SET;

Http5SendDataStruct strSen_value;
uint8_t cHttpSendFlag = 0;

uint16_t cHttpWaterAdrr = 0;
uint16_t cHttpWeitghAdrr = 0;
uint16_t cHttpWatervalue = 0;
float fHttpWeitghvalue = 0;

//s lora water value
uint32_t iWeterValueData[4] = {0, 0, 0, 0};
uint16_t wWaterCompareBack[4] = {0, 0, 0, 0};
uint8_t cWeterFirstInFlag[4] = {SET, SET, SET, SET};
uint8_t cWeterSendFlag[4] = {SET, SET, SET, SET};
uint16_t wDawoonWeterSendTime;

//s lora Weitgh value
uint16_t wWeitghCompareBack[8];

uint8_t cHttpSendCount = 0;
uint8_t cHttpPendingSendCount = 0;
uint8_t cHttpWaterSendCount = 0;
uint8_t cHttpWeitghSendCount = 0;

uint8_t buffLoraIndexCount[16];

//s mqtt check
uint8_t buffMqttStatusFlag[16];
uint32_t MqttStatusFlag;
//s chick onoff manegment
uint8_t HttpTimeReadFirstFlag = 0;
uint8_t HttpDeviceReadFirstFlag[4] = {0, 0, 0, 0};
uint16_t wDawoonTime;
uint16_t wDawoonTimeBack;
uint16_t wDawoonInfoTimeBack;
uint8_t cDeviceCnt = 0;
uint8_t cDeviceFlagOn = 0;
uint8_t cDawoonInfoTimeFlagOn = 0;

uint16_t wCheckMinite;
int32_t iDeviceRunTime = 0;

uint16_t wCheckSec;

uint8_t RfDeviceEnableFlag[4] = {0, 0, 0, 0};
uint8_t FeedDeviceFirstFlag[4] = {0, 0, 0, 0};
uint8_t cFeederFlagOn = 0;
uint8_t cTimeResettingFlagOn = 0;
uint8_t cTimeWaterSettingFlagOn = 0;

uint8_t cTimeControlChangeFlag;					//s 값 변경 확인
uint8_t cControlDeviceCount;
TimeControlStruct strTimeControl[4];
uint8_t cAutoDeviceSendTiming[4];			//s 컨트롤 보드 오토만 순서 적용
TimeControlStruct strTimeControlcomp[4];

ChickManualStruct strManualSwControl[4];			//s 메뉴얼 컨트롤 회기 데이터

uint8_t cCheckTimeSetId = 0;

//s 온도보정 관련 데이터 저장
uint8_t cTempChangeFlag[4];
uint16_t wTempChangeData[4];
uint16_t wTempChangeDataback[4];

uint8_t TempDataFirstFlag[4] = {0, 0, 0, 0};


//s 16ch온습도 컨트롤 파트
Temp16ChControlStruct strTemp16ChControl[16];
uint8_t cAutoDeviceChSend[16];
uint8_t cCheck16ChSetId = 0;

//s 에러체크 루틴
uint16_t wErrorWatchdogCheckCont;			//s watchdog error eeprom저장 및 mqtt 전송
uint16_t wErrorAutomanCheckCont[RF_DEVICE_MAX];
uint16_t wErrorTempCheckCont[4];
uint16_t wErrorSensorCheckCont[4];

uint16_t wTempMaintainData[4];
uint16_t wTempMaintainDataback[4];

uint8_t mqtt_debug = SET;
uint8_t error_reconnect_count = 0;
static char MqttSendBuff[1000];
//static char HttpSendBuff[1000];

extern char mqttbuf[512];     // heo
char json_buf[1024] = {0,0,};
int httpjson_size=0;

uint8_t mqtt_First_connect = 0;

uint8_t first_check = 0;

//s Mqtt 데이터 처음 발생하는 명령 무시. 브로커에서 마지막 전송데이터를 접속하면 보내줌
uint8_t cMqttMainFirstIn = SET;
uint8_t cMqttInfoFirstIn = SET;
uint8_t cMqttSendSelectCount = 0;

uint8_t cHttpReadCount = 0;
uint8_t cHttpDeviceCount = 1;

uint8_t cWatchdogFlag = 0;

//TIMER_1MS_TASK_STATE Timer1msTaskCount;

RTC_TimeTypeDef strMainTime;
RTC_DateTypeDef strMainData;   

//s Lcd Data 2*16
uint8_t cMenuControl;
char buffGLcd1Line[GLCD_BUFF_SIZE];
char buffGLcd2Line[GLCD_BUFF_SIZE];
uint8_t cReLcdDisplay;

//s buzzer control
uint8_t cBuzzerCount;

//s rf data
uint8_t buffUartRxData[LORA_BUFF_SIZE];
uint8_t buffUartRxDataSave[LORA_BUFF_SIZE];
uint8_t AutomanData[LORA_BUFF_SIZE];
uint8_t cRxdataCount;                           //s rx data count 
uint8_t cRxComplteState;
RfDataStruct strRfRxData[16];
RfDataStruct strRfTxData[16];
uint8_t cRfProcessMode = 0;                           //s loar setting
uint8_t cRfSettingState = 0;                           //s rf setting
uint16_t cRfSettingDelay = 0;                           //s rf setting
uint16_t cRfRetryCount = 0;                           //s rf setting
uint16_t cRfFreqRetryCount = 0;                           //s rf setting

uint8_t cRfTxDelayState;
uint8_t cRfTxDelayCount;
uint8_t cRfRXBuffClearCount;
uint8_t cRfTxEventSendCount;

uint8_t cAutomanSendTimming = 20; 
uint8_t RfRxEvent = 0; 
uint8_t RfRxAliveCount = 0; 


//s Lora 232 data
uint8_t buffLoraRxData[LORA_BUFF_SIZE];
uint8_t buffLoraRxDataSave[LORA_BUFF_SIZE];
uint8_t buffLoraRxRfData[LORA_BUFF_SIZE];
uint8_t cLoraRxdataCount;                           //s rx data count 
uint8_t cLoraRxComplteState;
uint8_t cLoraProcessMode = 0;                           //s loar setting
uint8_t cLoraSettingState = 0;                           //s loar setting
uint16_t cLoraSettingDelay = 0;                           //s loar setting

uint8_t cLoraTxDelayState;
uint8_t cLoraTxDelayCount;
uint8_t cLoraRXBuffClearCount;
uint8_t cLoraTxEventSendCount;

uint8_t serial_change = 0;

uint8_t buffLCDRxData[LORA_BUFF_SIZE];
uint8_t buffLCDRxDataSave[LORA_BUFF_SIZE];
uint16_t cLCDRxdataCount = 0;

//s ammonia setting value
AmmoniaSetStruct strAmmoniaSet;
TempSetStruct strTempSet;
SerialSetStruct strSerialSet;

//s mqtt send queue
MqttSendDataStruct strMqttOut;
uint8_t cMqttSendFlag = 0;
uint8_t cMqttDataCount = 0;
uint8_t cMqttConnectCheck = 0;

//s mqtt info send queue
MqttSendDataStruct strMqttOutInfo;
uint8_t cMqttSendInfoFlag = 0;
uint8_t cMqttDataInfoCount = 0;
uint8_t cMqttConnectInfoCheck = 0;


uint8_t loop_200ms = 0;
uint8_t loop_50ms = 0;
uint8_t loop_1000ms = 0;
uint8_t second = 0;
uint8_t old_second = 0;
uint8_t event_occur = 0;

uint8_t cTimeDisplay = 0;
uint8_t cIpDisplay = 0;
uint8_t cIpDisplayEndFlag = 0;

//uint16_t wMqttTestSendCount = 0;
uint8_t testWatchdog = 0;


uint8_t cMqttNotiCount = 0;

uint8_t cMqttConnectStateFlag = RESET;
uint32_t iMqttConnectStateCount = 0;


char MqttIdData[20];
char MacIdData[30];
/**
 * @brief System clock configuration
 **/

void SystemClock_Config(void)
{
   RCC_OscInitTypeDef RCC_OscInitStruct;
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;   
  RCC_OscInitTypeDef RCC_OscInitLSE;

   
   //Enable Power Control clock
   __HAL_RCC_PWR_CLK_ENABLE();

   //The voltage scaling allows optimizing the power consumption when the device is
   //clocked below the maximum system frequency, to update the voltage scaling value
   //regarding system frequency refer to product datasheet.
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

   //Enable HSE Oscillator and activate PLL with HSE as source
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 8;
   RCC_OscInitStruct.PLL.PLLN = 360;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 7;
   HAL_RCC_OscConfig(&RCC_OscInitStruct);

   //Enable overdrive mode
   HAL_PWREx_EnableOverDrive();

   //Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
   //clocks dividers
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
   HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
   
   
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  
    RCC_OscInitLSE.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitLSE.LSEState = RCC_LSE_ON;

    if(HAL_RCC_OscConfig(&RCC_OscInitLSE) != HAL_OK){
          Error_Handler();
    }  
  

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
   
}

/* IWDG init function */
void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 2500;             //s 16초
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/**
 * @brief Random data generation callback function
 * @param[out] data Buffer where to store the random data
 * @param[in] lenght Number of bytes that are required
 * @return Error code
 **/

error_t webSocketRngCallback(uint8_t *data, size_t length)
{
   //Generate some random data
   return yarrowRead(&yarrowContext, data, length);
}


/**
 * @brief SSL/TLS initialization callback
 * @param[in] context Pointer to the MQTT client context
 * @param[in] tlsContext Pointer to the SSL/TLS context
 * @return Error code
 **/

error_t mqttTlsInitCallback(MqttClientContext *context,
   TlsContext *tlsContext)
{
   error_t error;

   //Debug message
   TRACE_INFO("MQTT: TLS initialization callback\r\n");

   //Set the PRNG algorithm to be used
   error = tlsSetPrng(tlsContext, YARROW_PRNG_ALGO, &yarrowContext);
   //Any error to report?
   if(error)
      return error;

   //Set the fully qualified domain name of the server
   error = tlsSetServerName(tlsContext, APP_SERVER_NAME);
   //Any error to report?
   if(error)
      return error;

   //Import the list of trusted CA certificates
   error = tlsSetTrustedCaList(tlsContext, trustedCaList, strlen(trustedCaList));
   //Any error to report?
   if(error)
      return error;

   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Publish callback function
 * @param[in] context Pointer to the MQTT client context
 * @param[in] topic Topic name
 * @param[in] message Message payload
 * @param[in] length Length of the message payload
 * @param[in] dup Duplicate delivery of the PUBLISH packet
 * @param[in] qos QoS level used to publish the message
 * @param[in] retain This flag specifies if the message is to be retained
 * @param[in] packetId Packet identifier
 **/

void mqttPublishCallback(MqttClientContext *context,
   const char_t *topic, const uint8_t *message, size_t length,
   bool_t dup, MqttQosLevel qos, bool_t retain, uint16_t packetId)
{
  
    uint8_t value;
    uint8_t number;  
  
   //Debug message
//   TRACE_INFO("PUBLISH packet received...\r\n");
//   TRACE_INFO("  Dup: %u, ", dup);
//   TRACE_INFO("  QoS: %u, ", qos);
//   TRACE_INFO("  Retain: %u, ", retain);
//   TRACE_INFO("  Packet Identifier: %u, ", packetId);
//   TRACE_INFO("  Topic: %s\r\n", topic);
   TRACE_INFO("  message: %s\r\n", message);
  
    JSON_Value *output_value = NULL;
    output_value = json_parse_string((char*)message);
              
          
    if(output_value == NULL){
      TRACE_INFO("JSON DATA value NULL\r\n");  
            json_value_free(output_value);         
            return;                
    }
    else{
 
      JSON_Object *root_object; 
      
      root_object = json_value_get_object(output_value);      
      
       if(strcmp(topic, MyTopic) == 0)
       {
          if(cMqttMainFirstIn)
          {
            cMqttMainFirstIn = RESET;
            json_value_free(output_value);    		  
            return;
          }    

         
          if(strcmp(json_object_get_name(root_object, 0), "CMD") != 0)
          {
              TRACE_INFO("CAN NOT FIND CMD\r\n");                     
              json_value_free(output_value);     
              return;          
          }
          else
          {
              if(strcmp(json_object_get_string(root_object, "CMD"),"INFO") == 0)
              {
                  DawoonMqttSend(MQTT_IOT_INFO_RESPONE,0,0);
                  TRACE_INFO("CMD:INFO \r\n");  

              } 
              if(strcmp(json_object_get_string(root_object, "CMD"),"Req") == 0) {
                  DawoonMqttSend(MQTT_IOT_REQ_BEBAP_RESPONE,0,0);
                  TRACE_INFO("CMD:Req \r\n");  

              }               
              else if(strcmp(json_object_get_string(root_object, "CMD"),"CHANGE") == 0) {
                if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_BEHAP) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_BEHAP)){    
                  number = (int)json_object_get_number(root_object, "VALUE"); 
                             
                  if(number < 11){
                    value = 0;
                  }
                  else if(number < 21){
                    value = 1;
                    number -= 10; 
                  }
                  
                  
                 strRfTxData[value].Cmd = RF_MAIN_CMD_BAHAP_CHANGE;	     //hhlee             
                  
                  strRfTxData[value].Relay = number;                         
                  
                  strRfTxData[value].RelayEvent = SET;			                  
                }
                else{
                  value = (int)json_object_get_number(root_object, "ADR") - 1;
                  if(value > RF_DEVICE_MAX - 1){
                      json_value_free(output_value);        
                      return;
                  }                  
                    strRfTxData[value].Id = value + 1;	

                    number = (int)json_object_get_number(root_object, "NUMBER");
                    
                    strRfTxData[value].Mode = (int)json_object_get_number(root_object, "TYPE");						
                    strRfTxData[value].Cmd = RF_MAIN_CMD_CHANGE;	
                    
                    
                    if((strRfTxData[value].Mode == MQTT_TYPE_16CH || strRfTxData[value].Mode == MQTT_TYPE_SMALL_8CH )&& number){
                        strRfTxData[value].Relay = number;
                        if(json_object_get_number(root_object, "VALUE") == 1)
                        {				
                                strRfTxData[value].RelayTimer = 0xFFFF;		//s 설정값을 그대로 사용							
                        }
                        else{
                                strRfTxData[value].RelayTimer = 0;		//s off				
                        }
                        strRfTxData[value].RelayEvent = SET;							
                    }
                    else if(number){
                        if(strRfTxData[value].Mode == MQTT_TYPE_PENDING){
                            if(json_object_get_number(root_object, "VALUE") == 1)
                            {
                                    strRfTxData[value].Relay = (number * 2) - 1;
                                    strRfTxData[value].RelayTimer = 0xFFFF;		//s 설정값을 그대로 사용
                            }
                            else
                            {				
                                    strRfTxData[value].Relay = number * 2;
                                    strRfTxData[value].RelayTimer = 0xFFFF;		//s 설정값을 그대로 사용							
                            }								
                            strRfTxData[value].RelayEvent = SET;											
                      }
                      else{
                            if(json_object_get_number(root_object, "VALUE") == 2)
                            {
                                    strRfTxData[value].Relay = number * 2 ;
                                    strRfTxData[value].RelayTimer = 0xFFFF;		//s 설정값을 그대로 사용
                            }
                            else if(json_object_get_number(root_object, "VALUE") == 1)
                            {				
                                    strRfTxData[value].Relay = (number * 2) - 1;
                                    strRfTxData[value].RelayTimer = 0xFFFF;		//s 설정값을 그대로 사용							
                            }
                            else{
                                    strRfTxData[value].Relay = number*2;							
                                    strRfTxData[value].RelayTimer = 0;		//s off				
                            }						
                            strRfTxData[value].RelayEvent = SET;					
                        }								
                    }
                    else if(strRfTxData[value].Mode == MQTT_TYPE_PENDING && number == 0){		//s 계류식 자동 시작 정지 명령
                        if(json_object_get_number(root_object, "VALUE") == 1){
                                     strRfTxData[value].Relay = number ;                       
                                strRfTxData[value].Cmd = RF_MAIN_PENDING_AUTO_START;
                        }	
                        if(json_object_get_number(root_object, "VALUE") == 0){
                                    strRfTxData[value].Relay = number;                        
                                strRfTxData[value].Cmd = RF_MAIN_PENDING_AUTO_STOP;
                        }								
                        strRfTxData[value].RelayEvent = SET;								
                    } 
                    TRACE_INFO("CMD:CHANGE %d, %d\r\n", value, number);              //hhlee   
                }
            }
            else if(strcmp(json_object_get_string(root_object, "CMD"),"STATUS") == 0) {
                if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_BEHAP) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_BEHAP)){
                  if(CONFIG_RF_DEVICE_COUNT_2 >= 1)
                    DawoonMqttSend(MQTT_IOT_BEHAP_STATUS_RESPONE, 1, 0);

                  if(CONFIG_RF_DEVICE_COUNT_2 >= 2)
                    DawoonMqttSend(MQTT_IOT_BEHAP_STATUS_RESPONE, 2, 0);                  
                  TRACE_INFO("CMD:STATUS %d\r\n", value);                  
                }
                else{
                  //상태 문의
                  //지금 전달받은 상태 전송 !!
                  value = (int)json_object_get_number(root_object, "ADR");
                  if(value > RF_DEVICE_MAX){
                      json_value_free(output_value);        
                      return;
                  }
                  MqttStatusFlag = 500;
                  //publish_message(MyTopic, client, Json_device_status(value, MQTT_STATUS_RESPONE));
                  
                  DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, value, 0);
                  TRACE_INFO("CMD:STATUS %d\r\n", value);
                }

            } 
//            else if(strcmp(json_object_get_string(root_object, "CMD"),"STATUS_RESPONE") == 0){
//                value = (int)json_object_get_number(root_object, "ADR");			
//                buffMqttStatusFlag[value-1] = RESET;	
//                TRACE_INFO("CMD:STATUS_RESPONE \r\n");                       
//            }
                                    
            else if(strcmp(json_object_get_string(root_object, "CMD"),"AM_SET") == 0) {
                // 암모니아 기준값 셋팅	
                if(strAmmoniaSet.flag == RESET){
                    strAmmoniaSet.flag = SET;
                    strAmmoniaSet.addr = (uint8_t)json_object_get_number(root_object, "ADR");
                    strAmmoniaSet.value = (int)json_object_get_number(root_object, "VALUE");			//s 소수점 한자리 포함 200 => 20.0	

                    TM_HD44780_Puts_State(0,0, "                ");    					
                    sprintf(buffGLcd1Line, "AMMONIA SET:%d",strAmmoniaSet.addr);									
                    TM_HD44780_Puts_State(0,0, buffGLcd1Line); 
                }
                else{
                    TM_HD44780_Puts_State(0,0, "                ");    					
                    sprintf(buffGLcd1Line, "AMMO SET ERR:%d",strAmmoniaSet.addr);									
                    TM_HD44780_Puts_State(0,0, buffGLcd1Line); 						
                }
                TRACE_INFO("CMD:AM_SET \r\n");                      
            } 				
            else if(strcmp(json_object_get_string(root_object, "CMD"),"TEMP_SET") == 0) {				
                // 암모니아 기준값 셋팅	
                if(strTempSet.flag == RESET){
                    strTempSet.flag = SET;
                    strTempSet.addr = (int)json_object_get_number(root_object, "ADR");
                    strTempSet.value = (int)json_object_get_number(root_object, "VALUE");			//s 110 => +1도	, 82 면 -1.8도

                    TM_HD44780_Puts_State(0,0, "                ");    					
                    sprintf(buffGLcd1Line, "TEMP SET:%d",strTempSet.addr);									
                    TM_HD44780_Puts_State(0,0, buffGLcd1Line); 
                }
                else{
                    TM_HD44780_Puts_State(0,0, "                ");    					
                    sprintf(buffGLcd1Line, "TEMP SET ERR:%d",strTempSet.addr);									
                    TM_HD44780_Puts_State(0,0, buffGLcd1Line); 						
                }
                TRACE_INFO("CMD:TEMP_SET \r\n");                   
            }
            else if(strcmp(json_object_get_string(root_object, "CMD"),"WM_SET") == 0) {				
                // 암모니아 기준값 셋팅	
              if(strSerialSet.flag == RESET){
                  strSerialSet.flag = SET;
                  strSerialSet.addr = (int)json_object_get_number(root_object, "ADR");
                  sprintf(strSerialSet.buffValue, "%s", json_object_get_string(root_object, "SERIAL"));

                    TM_HD44780_Puts_State(0,0, "                ");    					
                    sprintf(buffGLcd1Line, "SERIAL SET:%d",strSerialSet.addr);									
                    TM_HD44780_Puts_State(0,0, buffGLcd1Line); 
              }
              else{
                  TM_HD44780_Puts_State(0,0, "                ");    					
                  sprintf(buffGLcd1Line, "SERI SET ERR:%d",strSerialSet.addr);									
                  TM_HD44780_Puts_State(0,0, buffGLcd1Line); 
              }	
                TRACE_INFO("CMD:WM_SET \r\n");       
            }				
            else if(strcmp(json_object_get_string(root_object, "CMD"),"CHICK_SET") == 0) {		
                cCheckTimeSetId = (int)json_object_get_number(root_object, "ADR");
                TRACE_INFO("CMD:CHICK_SET \r\n");                         
            }
            else if(strcmp(json_object_get_string(root_object, "CMD"),"FAN_SET") == 0) {		
                cCheck16ChSetId = (int)json_object_get_number(root_object, "ADR");
                TRACE_INFO("CMD:FAN_SET \r\n");                         
            }            
            else if(strcmp(json_object_get_string(root_object, "CMD"),"MANUAL_SET") == 0) {		
                value = (int)json_object_get_number(root_object, "ADR");
                number = (int)json_object_get_number(root_object, "NUMBER");        
                strTemp16ChControl[value - 1].c16ChManualFlag[number -1] = (int)json_object_get_number(root_object, "VALUE"); 
                DawoonMqttSend(MQTT_IOT_16CH_AUTO_MANUAL, value, number);
                TRACE_INFO("CMD:MANUAL_SET \r\n");                         
            }
            else {
              //pc.printf("JSON : %s\n\r", str);
            }
            
          }
        
       }
       
       if(strcmp(topic, MyInfoTopic) == 0){
          if(cMqttInfoFirstIn){
            cMqttInfoFirstIn = RESET;
            json_value_free(output_value);    		  
            return;
          }    

         
          if(strcmp(json_object_get_name(root_object, 0), "CMD") != 0){
              TRACE_INFO("CAN NOT FIND CMD\r\n");                     
              json_value_free(output_value);     
              return;          
          }
          else{
              if(strcmp(json_object_get_string(root_object, "CMD"),"NOTI") == 0) {
                if(cMqttNotiCount){
                  cMqttNotiCount--;             //s for reset
                }
              }
              else if(strcmp(json_object_get_string(root_object, "CMD"),"RESET") == 0) {
                  int32_t passdata = (int)json_object_get_number(root_object, "PASS");
                  if(passdata == CONFIG_PASSWORD2_5){
                      TRACE_INFO("MQTT RESET. WILL BE RESET AFTER 2SEC\r\n");                          
                      HAL_Delay(2000);                      
                      NVIC_SystemReset();		//s Mqtt 명령으로 리셋
                  }
              }              
              
            else {
              //pc.printf("JSON : %s\n\r", str);
            }
            
          }
        
       }
       
        json_value_free(output_value);          
      
    }

//   //Check topic name
//   if(!strcmp(topic, "board/leds/1"))
//   {
//      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
//         BSP_LED_Toggle(LED2);
//      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
//         BSP_LED_On(LED2);
//      else
//         BSP_LED_Off(LED2);
//   }
//   else if(!strcmp(topic, "board/leds/2"))
//   {
//      if(length == 6 && !strncasecmp((char_t *) message, "toggle", 6))
//         BSP_LED_Toggle(LED3);
//      else if(length == 2 && !strncasecmp((char_t *) message, "on", 2))
//         BSP_LED_On(LED3);
//      else
//         BSP_LED_Off(LED3);
//   }


}


//s for mqtt data. 16ch input
void MakeBitText16CH(char* buff, uint32_t data)
{
	uint8_t i = 0;
	//memset(buff, 0, 17);
	
	for(i = 0; i < 16; i++){
		if(data & (1 << i))
			buff[i] = '1';
		else
			buff[i] = '0';			
	}
}

void MakeBitTextIn(char* buff, uint32_t data)
{
	uint8_t i = 0;
	//memset(buff, 0, 17);
	
	for(i = 0; i < 8; i++){
		if(data & (1 << i))
			buff[i] = '1';
		else
			buff[i] = '0';			
	}
}


void MakeBitText8CH(char* buff, uint16_t data, uint8_t len)
{
	uint8_t i = 0;
	uint16_t temp = 0;
	//memset(buff, 0, 17);
	
	if(!len)
		return;
	
	for(i = 0; i < len; i++){
		temp = (data & (0x03 << (i*2))) >> (i*2);
		if(temp == 0x01)
			buff[i] = '1';
		else if(temp == 0x02)
			buff[i] = '2';			
		else if(temp == 0)
			buff[i] = '0';		
		else
			buff[i] = '3';
	}
	for(i = 8; i <16; i++){
		buff[i] = '0';
	}		
}


void MakeBitTextPending(char* buff, uint16_t data, uint8_t len)
{
	uint8_t i, count = 0;
	uint16_t temp = 0;
	//memset(buff, 0, 17);
	
	if(!len)
		return;
	
	for(i = 0; i <16; i++){
		buff[i] = '0';
	}		
	//s 이동 라인 1
	temp = data & (BIT_PANDING_MOVEING1_ON | BIT_PANDING_MOVEING1_OFF);
	if(temp == BIT_PANDING_MOVEING1_ON)
		buff[count] = '1';
	else if(temp == BIT_PANDING_MOVEING1_OFF)
		buff[count] = '2';

	count += 2;
	//s 이동 라인 2
	temp = data & (BIT_PANDING_MOVEING2_ON | BIT_PANDING_MOVEING2_OFF);
	if(temp == BIT_PANDING_MOVEING2_ON)
		buff[count] = '1';
	else if(temp == BIT_PANDING_MOVEING2_OFF)
		buff[count] = '2';
		
	count += 2;
	//s 이동 라인 3
	temp = data & (BIT_PANDING_MOVEING3_ON | BIT_PANDING_MOVEING3_OFF);
	if(temp == BIT_PANDING_MOVEING3_ON)
		buff[count] = '1';
	else if(temp == BIT_PANDING_MOVEING3_OFF)
		buff[count] = '2';
		
	count += 2;																							
	//s 당김장치
	temp = data & BIT_PANDING_PULL;							
	if(temp == BIT_PANDING_PULL)
		buff[count] = '1';

	count++;													
	//s 계류장치
	temp = data & (BIT_PANDING_PANDING_PULL | BIT_PANDING_PANDING_ANNEAL);
	if(temp == BIT_PANDING_PANDING_PULL)
		buff[count] = '1';
	else if(temp == BIT_PANDING_PANDING_ANNEAL)
		buff[count] = '2';											
}	


void MakeBitText8CHBehap(char* buff, uint16_t data, uint8_t len)
{
	uint8_t i = 0;
	uint16_t temp = 0;
	//memset(buff, 0, 17);
	
	if(!len)
		return;
	
	for(i = 0; i < len; i++){
		temp = (data & (0x03 << (i*2))) >> (i*2);
		if(temp == 0x01)
			buff[i] = '1';
		else if(temp == 0x02)
			buff[i] = '2';			
		else if(temp == 0)
			buff[i] = '0';		
		else
			buff[i] = '3';
	}	
}


/**
 * @brief Establish MQTT connection
 **/

error_t mqttConnect(void)
{
   error_t error;
   IpAddr ipAddr;
   MqttClientCallbacks mqttClientCallbacks;

   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");

   //Resolve MQTT server name
   error = getHostByName(NULL, APP_SERVER_NAME, &ipAddr, 0);
   //Any error to report?
   if(error)
      return error;

#if (APP_SERVER_PORT == 80 || APP_SERVER_PORT == 443)
   //Register RNG callback
   webSocketRegisterRandCallback(webSocketRngCallback);
#endif

   //Initialize MQTT client context
   mqttClientInit(&mqttClientContext);
   //Initialize MQTT client callbacks
   mqttClientInitCallbacks(&mqttClientCallbacks);

   //Attach application-specific callback functions
   mqttClientCallbacks.publishCallback = mqttPublishCallback;
#if (APP_SERVER_PORT == 8883 || APP_SERVER_PORT == 443)
   mqttClientCallbacks.tlsInitCallback = mqttTlsInitCallback;
#endif

   //Register MQTT client callbacks
   mqttClientRegisterCallbacks(&mqttClientContext, &mqttClientCallbacks);

   //Set the MQTT version to be used
   mqttClientSetProtocolLevel(&mqttClientContext,
      MQTT_PROTOCOL_LEVEL_3_1_1);

#if (APP_SERVER_PORT == 1883)
   //MQTT over TCP
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_TCP);
#elif (APP_SERVER_PORT == 8883)
   //MQTT over SSL/TLS
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_TLS);
#elif (APP_SERVER_PORT == 80)
   //MQTT over WebSocket
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_WS);
#elif (APP_SERVER_PORT == 443)
   //MQTT over secure WebSocket
   mqttClientSetTransportProtocol(&mqttClientContext, MQTT_TRANSPORT_PROTOCOL_WSS);
#endif

   //Set keep-alive value
   mqttClientSetKeepAlive(&mqttClientContext, 3600);

#if (APP_SERVER_PORT == 80 || APP_SERVER_PORT == 443)
   //Set the hostname of the resource being requested
   mqttClientSetHost(&mqttClientContext, APP_SERVER_NAME);
   //Set the name of the resource being requested
   mqttClientSetUri(&mqttClientContext, APP_SERVER_URI);
#endif

   //Set client identifier
   mqttClientSetIdentifier(&mqttClientContext, MqttIdData);

   //Set user name and password
   //mqttClientSetAuthInfo(&mqttClientContext, "username", "password");

   //Set Will message
   mqttClientSetWillMessage(&mqttClientContext, "board/status",
      "offline", 7, MQTT_QOS_LEVEL_0, FALSE);
   
   lcd_view_flag = 1;

   //Debug message
   TRACE_INFO("Connecting to MQTT server %s...\r\n", ipAddrToString(&ipAddr, NULL));
   
   error_reconnect_count++;

   //Start of exception handling block
   do
   {
      //Establish connection with the MQTT server
      error = mqttClientConnect(&mqttClientContext,
         &ipAddr, APP_SERVER_PORT, TRUE);
      //Any error to report?
      if(error)
         break;

//      //Subscribe to the desired topics
//      error = mqttClientSubscribe(&mqttClientContext,
//         "board/leds/+", MQTT_QOS_LEVEL_1, NULL);
//      //Any error to report?
//      if(error)
//         break;

//      //Send PUBLISH packet
//      error = mqttClientPublish(&mqttClientContext, "board/status",
//         "online", 6, MQTT_QOS_LEVEL_1, TRUE, NULL);
//      
//      //Any error to report?
//      if(error)
//         break;
      
      //Subscribe to the desired topics
      error = mqttClientSubscribe(&mqttClientContext,
         MyTopic, MQTT_QOS_LEVEL_1, NULL);

      //Any error to report?
      if(error)
         break;            
      
      //Subscribe to the desired topics
      error = mqttClientSubscribe(&mqttClientContext,
         MyInfoTopic, MQTT_QOS_LEVEL_1, NULL);      
      
      //Any error to report?
      if(error)
         break;      
      
//      //Send PUBLISH packet
//      error = mqttClientPublish(&mqttClientContext, MyTopic,
//         "online", 6, MQTT_QOS_LEVEL_1, TRUE, NULL);
//      
//      //Any error to report?
//      if(error)
//         break;      
      
      
      
      //End of exception handling block
   } while(0);

   //Check status code
   if(error)
   {
      //Close connection
      mqttClientClose(&mqttClientContext);
   }
   lcd_view_flag = 1;
   //Return status code
   return error;
}



uint8_t http_error_flag=0;

void Http_200_check(void)
{
  
  char * ptr;
                    
  ptr = strstr(json_buf,"HTTP/1.1 200 OK");
  
  if(ptr != 0)  {
    http_error_flag = 1;
  }
  else {
  //  http_error_flag = 0;
  }
                    
  

}


/**
 * @brief HTTP client read routine
 * @return Error code
 **/

error_t HttpClientRead(uint8_t Type, uint8_t id)
{
   error_t error;
   size_t length;
   IpAddr ipAddr;
   Socket *socket;
   static char_t buffer[5000];
//   static char_t payload[5000];
   char* Authorization;
   
   memset(buffer, 0, sizeof(buffer));
   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");

   //Resolve HTTP server name
   error = getHostByName(NULL, "103.60.124.34", &ipAddr, 0);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Create a new socket to handle the request
   socket = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP);
   //Any error to report?
   if(!socket)
   {
      //Debug message
      TRACE_INFO("Failed to open socket!\r\n");
      //Exit immediately
      return ERROR_OPEN_FAILED;
   }

   //Start of exception handling block
   do
   {
      //Debug message
      TRACE_INFO("Connecting to HTTP server %s\r\n", ipAddrToString(&ipAddr, NULL));

      //Connect to the HTTP server 
      error = socketConnect(socket, &ipAddr, 80);
            
      //Any error to report?
      if(error){
        TRACE_INFO("socketConnect error\r\n");        
 //       error_reconnect_count = 100;    // 재부팅.....
        break;
      }

      //Debug message
      TRACE_INFO("Successful connection\r\n");

      //Format HTTP request
      if(Type == HTTP_GET_TIME){
              // Send an HTTP request
              //sprintf(buffer, "GET /api/Milking/Inequality/%d/7 HTTP/1.1\r\nHost: 192.168.1.42:5000\r\nConnection: close\r\n\r\n", rfid);
              if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK)){
                length = sprintf(buffer, "GET /apiCmd.post.php?compCd=%04d&uid=010%04d%04d&m=confChick&p=CURTM HTTP/1.1\r\nHost: www.cowplan.co.kr\r\nConnection: close\r\n\r\n", 
                CONFIG_FARM_PREFERENCE_1, CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5);		
              }
              else if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN)){
                length = sprintf(buffer, "GET /apiCmd.post.php?compCd=%04d&uid=010%04d%04d&m=confFan&p=CURTM HTTP/1.1\r\nHost: www.cowplan.co.kr\r\nConnection: close\r\n\r\n", 
                CONFIG_FARM_PREFERENCE_1, CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5);	               
              }
      }
      else if(Type == HTTP_GET_DEVICE_SET){
              // Send an HTTP request
              length = sprintf(buffer, "GET /apiCmd.post.php?compCd=%04d&uid=010%04d%04d&m=confChick&p=SetVal&dCd=%d HTTP/1.1\r\nHost: www.cowplan.co.kr\r\nConnection: close\r\n\r\n", 
              CONFIG_FARM_PREFERENCE_1, CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5, id);			
      }      
      else if(Type == HTTP_GET_16CH_DEVICE_SET){
              // Send an HTTP request
              length = sprintf(buffer, "GET /apiCmd.post.php?compCd=%04d&uid=010%04d%04d&m=confFan&dCd=%d HTTP/1.1\r\nHost: www.cowplan.co.kr\r\nConnection: close\r\n\r\n", 
              CONFIG_FARM_PREFERENCE_1, CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5, id);			
      }   
      
      http_error_flag = 0;
      
      //Debug message
      TRACE_INFO("\r\nread HTTP request:\r\n%s", buffer);

      //Send HTTP request
      error = socketSend(socket, buffer, length, NULL, 0);
      //Any error to report?
      if(error)
         break;

      //Debug message
      TRACE_INFO("read HTTP response header:\r\n");

      //Parse HTTP response header
      while(1)
      {
         //Read the header line by line
         error = socketReceive(socket, buffer, sizeof(buffer) - 1, &length, SOCKET_FLAG_BREAK_CRLF);
         //End of stream?
         if(error) {
            TRACE_DEBUG("RECEIVE DATA header ERROR %d",error);
            break;
         }

         if(length != 0) {    // 404 error check
            memcpy(json_buf,buffer,length);
            httpjson_size = length;
   
            Http_200_check();
         }
         

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Dump current data
         //TRACE_INFO("%s", buffer);

         //The end of the header has been reached?
         if(!strcmp(buffer, "\r\n"))
            break;
      }
      
      //TRACE_DEBUG("buffer %s\r\n",buffer);      

      if(http_error_flag == 0) {
        //http_error_flag = 0;
        TRACE_DEBUG("Can not receive HTTP/1.1 200 OK  \r\n");
        break;
      }

      

      //Debug message
      TRACE_INFO("read HTTP response body:\r\n");

      //Parse HTTP response body
      while(1)
      {
         //Read response body
         error = socketReceive(socket, buffer, sizeof(buffer) - 1, &length, 0);
         //End of stream?
         if(error) {

            TRACE_DEBUG("RECEIVE DATA body ERROR %d",error);
           
            break;
         }

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Dump current data
   
         TRACE_INFO("buffer: %s\r\n", buffer);

          Authorization = strstr(buffer,"None Authorization");          //s 권한이 없음
          
          if(Authorization != 0)  {
            TRACE_INFO("ERROR: %s\r\n", buffer);
            break;
          }
         else{
            if(Type == HTTP_GET_DEVICE_SET){
                RfDeviceEnableFlag[id-1] = SET;		//s rf데이터 전송 가능 플레그
                HttpDeviceReadFirstFlag[id-1] = RESET;	//s 읽었으므로 1시간 후 또는 이벤트가 있을때 읽음 
                if(id == cCheckTimeSetId){
                 cCheckTimeSetId = 0;				//s 이벤트로 읽기 완료
                }           
            }
            else if(Type == HTTP_GET_16CH_DEVICE_SET){
                //strTemp16ChControl[id-1].cDeviceSetting = SET;		//s rf데이터 전송 가능 플레그
                strTemp16ChControl[id-1].cHttpDeviceReadFlag = RESET;	//s 읽었으므로 1시간 후 또는 이벤트가 있을때 읽음 
                if(id == cCheck16ChSetId){
                  cCheck16ChSetId = 0;				//s 이벤트로 읽기 완료
                }           
            }            
            else if(Type == HTTP_GET_TIME){
              HttpTimeReadFirstFlag = SET;		//s 시간데이터 읽음           
            }
         }          
        
      if(Type == HTTP_GET_TIME){
          //payload = strstr(buffer, "{\"CURTM");
//        sprintf(payload, "%s", strstr(buffer, "{\"CURTM"));          
      }
      else if(Type == HTTP_GET_DEVICE_SET){	
          //payload = strstr(buffer, "{\"difTm");
//          sprintf(payload, "%s", strstr(buffer, "{\"difTm"));          
      }
      else if(Type == HTTP_GET_16CH_DEVICE_SET){
//          sprintf(payload, "%s", strstr(buffer, "{\"difTm"));          
      }
      else{
        TRACE_DEBUG("CAN NOT USE HTTP DATA %d \r\n",error);        
        break;
      }
      //sprintf(payload, "%s", strstr(buffer, "{\""));
        
//      uint32_t JsonSize = 0;	
        
//       json_char* json;
//       json_value* http_json_value;
      
//
//        while(1){	//s json {} data size
//          if(payload[JsonSize] == '}')
//              break;
//          
//          if(JsonSize > 1000){            
//            break;            
//          }
//                        
//          JsonSize++;
//        }
//        JsonSize++;

        
//        if(JsonSize > 1000){
//           TRACE_INFO("JSON DATA MAX COUNT ERROR \r\n");                      
// //          json_value_free(http_json_value);        		//s 이상있으면 의심해보자
//           return error;
//        }
//        else{

//          memset(buffer, 0, 1000);          
//          sprintf(buffer, "%.*s", JsonSize, payload);
  
          JSON_Value *output_value = NULL;
          output_value = json_parse_string(buffer);
              


          if(Type == HTTP_GET_TIME){
              cDeviceCnt  = HttpGetTimeJsonParse(output_value, &strMainData, &strMainTime);
              if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK)){
                for(uint8_t i = 0; i < cDeviceCnt; i++){
                  HttpDeviceReadFirstFlag[i] = SET;
                  strRfRxData[i].Mode = MQTT_TYPE_SMALL_8CH;										//s 계사 프로그램은 이미 장비가 정해져 있고 에러 체크를 위해 무조건 발송									
                }
              }
              HAL_RTC_SetTime(&hrtc, &strMainTime, FORMAT_BIN);
              HAL_RTC_SetDate(&hrtc, &strMainData, FORMAT_BIN);
              TRACE_INFO("save date %d.%d.%d   time %d:%d:%d\r\n", strMainData.Year, strMainData.Month, strMainData.Date, strMainTime.Hours, strMainTime.Minutes, strMainTime.Seconds);                         
          }
          else if(Type == HTTP_GET_DEVICE_SET){	
              HttpGetDongParse(output_value, id);
//              while(1);
          }	      
          else if(Type == HTTP_GET_16CH_DEVICE_SET){	
              HttpGet16ChParse(output_value, id);
//              while(1);
          }	          
          json_value_free(output_value);
//          json_value_free(http_json_value);  
//        }
        break;
      }

      break;
      
      //End of exception handling block
   } while(0);

   //Close the connection
   socketClose(socket);
   //Debug message
   TRACE_INFO("\r\nConnection closed...\r\n");

   //Return status code
   return error;
}



error_t HttpClientWrite(uint32_t subid, uint8_t mode)
{

   error_t error;
   size_t length;   
   IpAddr ipAddr;
   Socket *socket;
   char buffer[500];
   char buffer2[200];   
   memset(buffer, 0, 500);
   memset(buffer2, 0, 200);   

   //Debug message
   TRACE_INFO("\r\n\r\nResolving server name...\r\n");
   
   //Resolve HTTP server name
   error = getHostByName(NULL, "103.60.124.34", &ipAddr, 0);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_INFO("Failed to resolve server name!\r\n");
      //Exit immediately
      return error;
   }

   //Create a new socket to handle the request
   socket = socketOpen(SOCKET_TYPE_STREAM, SOCKET_IP_PROTO_TCP);
   //Any error to report?
   if(!socket)
   {
      //Debug message
      TRACE_INFO("Failed to open socket!\r\n");
      //Exit immediately
      return ERROR_OPEN_FAILED;
   }

   //Start of exception handling block
   do
   {
      //Debug message
      TRACE_INFO("Connecting to HTTP server %s\r\n", ipAddrToString(&ipAddr, NULL));
      
      error = socketConnect(socket, &ipAddr, 80);      

      //Any error to report?
      if(error){
        TRACE_INFO("socketConnect error\r\n");        
        break;
      }
      
        if(mode == HTTP_SEND_PANDING){
                sprintf(buffer, "POST /apiCmd.post.php?compCd=%04d&uid=010%04d%04d&m=ctrlFloat&dvCd=%d&flTm1=%d&flTm2=%d&flTm3=%d&cmpl1=%d&cmpl2=%d&cmpl3=%d HTTP/1.1\r\nHost:  www.cowplan.co.kr\r\nConnection: close\r\n\r\n"
                ,CONFIG_FARM_PREFERENCE_1, CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5, strSen_value.HttpData[strSen_value.NowPos].cPos, strSen_value.HttpData[strSen_value.NowPos].InTimer1, 
                strSen_value.HttpData[strSen_value.NowPos].InTimer2, strSen_value.HttpData[strSen_value.NowPos].InTimer3, strSen_value.HttpData[strSen_value.NowPos].InTimer1Event
                  ,strSen_value.HttpData[strSen_value.NowPos].InTimer2Event ,strSen_value.HttpData[strSen_value.NowPos].InTimer3Event );
            TRACE_INFO("HTTP_SEND_PANDING MAKE:%s\n\r", buffer);                     
        }
        else if(mode == HTTP_SEND_WEIGHT_KHH){
                sprintf(buffer, "POST /apiCmd.post.php?uid=010%04d%04d&compCd=%d&m=beanWGT&bCd=1&dst=%2.2f HTTP/1.1\r\nHost:  www.cowplan.co.kr\r\nConnection: close\r\n\r\n"
                ,CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5 ,myFcode, fHttpWeitghvalue);			
        }		
        else if(mode == HTTP_SEND_SENSOR){
          sprintf(buffer, "POST /apiCmd.post.php?uid=010%04d%04d&compCd=%04d&m=farmEnv&eCd=", CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5, CONFIG_FARM_PREFERENCE_1); //T/T/T/T/H/H/H/H/C/C/C/C/F/F/F/F&eVal=");

          strcat(buffer, "T/H/C/A");

           
           //실제 데이타
           strcat(buffer, "&eVal=");         
                                                           
           sprintf(buffer2, "%3.1f/%3.1f/%04d/%3.1f/", strSen_value.HttpData[strSen_value.NowPos].temp, strSen_value.HttpData[strSen_value.NowPos].humudity, 
          strSen_value.HttpData[strSen_value.NowPos].Co2, strSen_value.HttpData[strSen_value.NowPos].Ammonia);
           strncat(buffer,buffer2, strlen(buffer2));
 

          memset(buffer2, 0 , sizeof(buffer2));
           //번호 저장
           strcat(buffer, "&ePst=");   
             
           sprintf(buffer2, "%d/%d/%d/%d/", subid+1, subid+1, subid+1, subid+1);		//s 센서 값에 대한 아이디 "T/H/C/A"
           strncat(buffer,buffer2, strlen(buffer2));               
                         //if(i != (sen_cnt - 1)) strcat(Buffer, "/");
           
           strcat(buffer, " HTTP/1.1\r\n");
           strcat(buffer, "Host:  www.cowplan.co.kr\r\n");
           strcat(buffer, "Connection: close\r\n");
           strcat(buffer, "\r\n"); 
            TRACE_INFO("HTTP_SEND_SENSOR MAKE:%s\n\r", buffer);                   
        }
        else if(mode == HTTP_SEND_WATER){

          sprintf(buffer, "POST /apiCmd.post.php?compCd=%04d&uid=010%04d%04d&eCd=1&m=wtr&p=%d", CONFIG_FARM_PREFERENCE_1, CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5, subid-3); //T/T/T/T/H/H/H/H/C/C/C/C/F/F/F/F&eVal=");

           sprintf(buffer2, "/&prs=%d/&wtr=%d/", strSen_value.HttpData[strSen_value.NowPos].WaterPresure, strSen_value.HttpData[strSen_value.NowPos].WaterMakeValue);

           strncat(buffer,buffer2, strlen(buffer2));               
                         //if(i != (sen_cnt - 1)) strcat(Buffer, "/");   
           
           strcat(buffer, " HTTP/1.1\r\n");
           strcat(buffer, "Host:  www.cowplan.co.kr\r\n");
           strcat(buffer, "Connection: close\r\n");
           strcat(buffer, "\r\n");   

//				memcpy(httpcharbuff, buffer, sizeof(httpcharbuff));
            TRACE_INFO("HTTP_SEND_WATER MAKE:%s\n\r", buffer);                
        }				
        else if(mode == HTTP_SEND_WEIGHT){
                
        sprintf(buffer, "POST /apiCmd.post.php?uid=010%04d%04d&compCd=%04d&m=beanWGT&bCd=%d&wgt=%04d&dst=", 
        CONFIG_PASSWORD1_4, CONFIG_PASSWORD2_5, CONFIG_FARM_PREFERENCE_1,subid-7, strSen_value.HttpData[strSen_value.NowPos].Weitgh); //T/T/T/T/H/H/H/H/C/C/C/C/F/F/F/F&eVal=");\
        
         strcat(buffer, " HTTP/1.1\r\n");							 
         strcat(buffer, "Host:  www.cowplan.co.kr\r\n");
         strcat(buffer, "Connection: close\r\n");
         strcat(buffer, "\r\n");   
         TRACE_INFO("HTTP_SEND_WEIGHT MAKE:%s\n\r", buffer);           
//				memcpy(httpcharbuff, buffer, sizeof(httpcharbuff));
      }
			
      length = strlen(buffer);
      error = socketSend(socket, buffer, length, NULL, 0);

      
      //Any error to report?
      if(error){
         TRACE_INFO("Can not send HTTP socket data.\r\n");
          sprintf(buffGLcd1Line, "HTTP SEND ERR!%d", mode);									
          TM_HD44780_Puts_State(0,0, buffGLcd1Line);	         
        break;
      } 

      //Debug message
      TRACE_INFO("write HTTP response header:\r\n");

      //Parse HTTP response header
      while(1)
      {
         //Read the header line by line
         error = socketReceive(socket, buffer, sizeof(buffer) - 1, &length, SOCKET_FLAG_BREAK_CRLF);
         //End of stream?
         if(error){
            sprintf(buffGLcd1Line, "HTTP SEND ERR:%d ", mode);									
            TM_HD44780_Puts_State(0,0, buffGLcd1Line);	            
            TRACE_INFO("err:%s\n\r", buffer);           
           break;
         }

         //Properly terminate the string with a NULL character
         buffer[length] = '\0';
         //Dump current data
         //TRACE_INFO("%s", buffer);
  
         //The end of the header has been reached?
            
         if(!strcmp(buffer, "\r\n"))
            break;
         
      }

      //Debug message              
      TRACE_INFO("write HTTP response body:\r\n");
      sprintf(buffGLcd1Line, "HTTP SEND OK!:%d ", mode);									
      TM_HD44780_Puts_State(0,0, buffGLcd1Line);
      TRACE_INFO("HTTP SEND OK:%d\r\n", mode);      
      break;


      //End of exception handling block      
   } while(0);
   
   //Close the connection
   socketClose(socket);
   //Debug message
   TRACE_INFO("\r\nConnection closed...\r\n");

   //Return status code
   return error;   
   
}


//s uart callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART2)
  {

    buffLoraRxDataSave[cLoraRxdataCount] = buffLoraRxData[0];
    cLoraRxdataCount++;
    //HAL_UART_Receive_IT(&huart2, buffLoraRxData, 1);    
  }
  
  if(huart->Instance == USART6)
  {
    buffUartRxDataSave[cRxdataCount] = buffUartRxData[0];
    cRxdataCount++;
   // HAL_UART_Receive_IT(&huart6, buffUartRxData, 1);
  }  
  
  if(huart->Instance == UART7)
  {
    buffLCDRxDataSave[cLCDRxdataCount++] = buffLCDRxData[0];
   // HAL_UART_Receive_IT(&huart7, buffLCDRxData, 1);
  }
}



void DawoonMqttSend(uint8_t cmd, uint8_t id, uint8_t mode)
{

    uint8_t savepos = strMqttOut.SavePos + 1;
    if(savepos == MAIN_MQTT_MAX_BUFF)		//s 최대치 넘어가면 초기화
            savepos = 0;  

    if(strMqttOut.NowPos == savepos){		//s Mqtt 전송 큐 저장
          sprintf(buffGLcd2Line, "MQTT BUFF MAX %d", CONFIG_RF_TYPE_3);									
          TM_HD44780_Puts_State(0,1, buffGLcd2Line); 
           TRACE_INFO("MQTT BUFF MAX %d \r\n", CONFIG_RF_TYPE_3);                     
          HAL_Delay(3000);	
                          
          NVIC_SystemReset();		//s Mqtt 정상이 아니므로 리셋											
    }
    else{	
      strMqttOut.MqttData[strMqttOut.SavePos].cmd = cmd;      

      if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_INFO_RESPONE){
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_INFO_RESPONE \r\n");            
      }
      if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_REQ_BEBAP_RESPONE){
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_REQ_BEBAP_RESPONE \r\n");            
      }      
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_CHANGE_RESPONE){
          strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = strRfRxData[id].Id;        
          strMqttOut.MqttData[strMqttOut.SavePos].cDeviceType = strRfRxData[id].Mode;
          strMqttOut.MqttData[strMqttOut.SavePos].TxRelay = strRfTxData[strRfRxData[id].Id - 1].Relay;
          strMqttOut.MqttData[strMqttOut.SavePos].Relay = strRfRxData[id].Relay;          
          strMqttOut.MqttData[strMqttOut.SavePos].PendingMode = mode;
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_CHANGE_RESPONE \r\n");     
        TRACE_INFO("strRfRxData[id].Id %d\r\n", strRfRxData[id].Id);  
        TRACE_INFO("strRfRxData[id].Mode %d\r\n", strRfRxData[id].Mode);          
        TRACE_INFO("strRfTxData[strRfRxData[id].Id - 1].Relay %d\r\n", strRfTxData[strRfRxData[id].Id - 1].Relay);          
        TRACE_INFO("strRfRxData[id].Relay %d\r\n", strRfRxData[id].Relay);          
        TRACE_INFO("mode %d\r\n", mode);                  
      }
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_HCOW_STAT){
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = id;
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceType = strRfRxData[id-1].Mode;
        strMqttOut.MqttData[strMqttOut.SavePos].Input = strRfRxData[id-1].Input;
        strMqttOut.MqttData[strMqttOut.SavePos].Relay = strRfRxData[id-1].Relay;      
        if(mode){               //s 모두정지시 앱 시간을 지우기 위함
          TRACE_INFO("MQTT_IOT_HCOW_STAT MODE:1\r\n");            
          strMqttOut.MqttData[strMqttOut.SavePos].InTimer1 = 0;    
          strMqttOut.MqttData[strMqttOut.SavePos].InTimer2 = 0;    
          strMqttOut.MqttData[strMqttOut.SavePos].InTimer3 = 0;            
        }
        else{
          strMqttOut.MqttData[strMqttOut.SavePos].InTimer1 = strRfRxData[id-1].InTimer1;    
          strMqttOut.MqttData[strMqttOut.SavePos].InTimer2 = strRfRxData[id-1].InTimer2;    
          strMqttOut.MqttData[strMqttOut.SavePos].InTimer3 = strRfRxData[id-1].InTimer3;             
        }
        strMqttOut.MqttData[strMqttOut.SavePos].MqttFreqSend  = strRfRxData[id-1].MqttFreqSend;
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_HCOW_STAT \r\n");                  
      }            
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_STATUS_RESPONE){
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = id;
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceType = strRfRxData[id-1].Mode;
        strMqttOut.MqttData[strMqttOut.SavePos].Input = strRfRxData[id-1].Input;
        strMqttOut.MqttData[strMqttOut.SavePos].Relay = strRfRxData[id-1].Relay;      
        strMqttOut.MqttData[strMqttOut.SavePos].InTimer1 = strRfRxData[id-1].InTimer1;    
        strMqttOut.MqttData[strMqttOut.SavePos].InTimer2 = strRfRxData[id-1].InTimer2;    
        strMqttOut.MqttData[strMqttOut.SavePos].InTimer3 = strRfRxData[id-1].InTimer3;            
        strMqttOut.MqttData[strMqttOut.SavePos].MqttFreqSend  = strRfRxData[id-1].MqttFreqSend;   
        memcpy(strMqttOut.MqttData[strMqttOut.SavePos].MqttFanMode, strTemp16ChControl[id-1].c16ChManualFlag, sizeof(strMqttOut.MqttData[strMqttOut.SavePos].MqttFanMode));     //s fan 자동 모드로 추가됨

        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_STATUS_RESPONE %d\r\n", id);         
      }      
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_CHANGE_RETURN){
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = id;
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceType = strRfRxData[id-1].Mode;
        strMqttOut.MqttData[strMqttOut.SavePos].InEvent = strRfRxData[id-1].InEvent;
        strMqttOut.MqttData[strMqttOut.SavePos].RelayEvent = strRfRxData[id-1].RelayEvent;          
        strMqttOut.MqttData[strMqttOut.SavePos].Relay  = mode;        
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_CHANGE_RETURN \r\n");             
      }      
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_16CH_AUTO_MANUAL){
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = id;
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceType = strRfRxData[id-1].Mode;
        strMqttOut.MqttData[strMqttOut.SavePos].RelayEvent = strTemp16ChControl[id - 1].c16ChManualFlag[mode -1];
        strMqttOut.MqttData[strMqttOut.SavePos].Relay  = mode;        
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_16CH_AUTO_MANUAL \r\n");             
      }   
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_BEHAP_CHANGE_RESPONE){
          strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = strRfRxData[id].Id;        
          strMqttOut.MqttData[strMqttOut.SavePos].TxRelay = strRfTxData[strRfRxData[id].Id - 1].Relay;
          strMqttOut.MqttData[strMqttOut.SavePos].Relay = strRfRxData[id].Relay; 
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_BEHAP_CHANGE_RESPONE \r\n");     
        TRACE_INFO("strRfRxData[id].Id %d\r\n", strRfRxData[id].Id);  
        TRACE_INFO("strRfTxData[strRfRxData[id].Id - 1].Relay %d\r\n", strRfTxData[strRfRxData[id].Id - 1].Relay);          
        TRACE_INFO("strRfRxData[id].Relay %d\r\n", strRfRxData[id].Relay); 
      }      
      else if(strMqttOut.MqttData[strMqttOut.SavePos].cmd == MQTT_IOT_BEHAP_STATUS_RESPONE){
        strMqttOut.MqttData[strMqttOut.SavePos].cDeviceId = id;
        strMqttOut.MqttData[strMqttOut.SavePos].Relay = strRfRxData[id-1].Relay;

        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_BEHAP_STATUS_RESPONE %d\r\n", id);         
      }          
      
        strMqttOut.SavePos = savepos;		
         
        cMqttDataCount++;
     
    }      
}



void DawoonMqttSendInfo(uint8_t cmd, uint8_t id, uint8_t mode)
{

    uint8_t savepos = strMqttOutInfo.SavePos + 1;
    if(savepos == MAIN_MQTT_MAX_BUFF)		//s 최대치 넘어가면 초기화
            savepos = 0;  

    if(strMqttOutInfo.NowPos == savepos){		//s Mqtt 전송 큐 저장
          sprintf(buffGLcd2Line, "MQTT BUFF MAX %d", CONFIG_RF_TYPE_3);									
          TM_HD44780_Puts_State(0,1, buffGLcd2Line); 
           TRACE_INFO("MQTT BUFF MAX %d \r\n", CONFIG_RF_TYPE_3);                     
          HAL_Delay(3000);	
                          
          NVIC_SystemReset();		//s Mqtt 정상이 아니므로 리셋											
    }
    else{	
      strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cmd = cmd;      
      if(strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cmd == MQTT_IOT_INFO_ERROR){
        strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cDeviceId = id;
        strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].ErrorTemp = wErrorTempCheckCont[id];
        strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].ErrorSensor = wErrorSensorCheckCont[id];
        strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].ErrorAutoman= wErrorAutomanCheckCont[id];
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_INFO_ERROR \r\n");            
      }
      else if(strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cmd == MQTT_IOT_INFO_SENSOR){
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cDeviceId = strSen_value.HttpData[strSen_value.NowPos].cPos;
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].temp = (float)strSen_value.HttpData[strSen_value.NowPos].temp;
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].humudity = (float)strSen_value.HttpData[strSen_value.NowPos].humudity;							
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].Co2 = strSen_value.HttpData[strSen_value.NowPos].Co2;	
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].Ammonia = (float)strSen_value.HttpData[strSen_value.NowPos].Ammonia;  
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_INFO_SENSOR \r\n");           
      }
      else if(strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cmd == MQTT_IOT_INFO_WATER){ 
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cDeviceId = strSen_value.HttpData[strSen_value.NowPos].cPos;        
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].WaterPresure = strSen_value.HttpData[strSen_value.NowPos].WaterPresure;			
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].WaterValue = strSen_value.HttpData[strSen_value.NowPos].WaterValue;	//s 계속 더해줄지 그때 값만 보내줄지 생각해야함.
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cWaterEvent = strSen_value.HttpData[strSen_value.NowPos].cWaterEvent;        
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_INFO_WATER \r\n");                     
      }	
      else if(strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cmd == MQTT_IOT_INFO_WEIGHT){         
          strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].cDeviceId = strSen_value.HttpData[strSen_value.NowPos].cPos;        
        strMqttOutInfo.MqttData[strMqttOutInfo.SavePos].Weitgh = strSen_value.HttpData[strSen_value.NowPos].Weitgh;
        TRACE_INFO("MQTT SEND CMD:MQTT_IOT_INFO_WATER \r\n");                  
      }	      

        strMqttOutInfo.SavePos = savepos;		
         
        cMqttDataInfoCount++;
     
    }      
}

/**
 * @brief MQTT test task
 **/

void mqttSendTask(void *param)
{
   error_t error;
   uint_t buttonState;
   uint_t prevButtonState;
//   uint8_t temp;
   uint16_t number;
   uint16_t value;
   char_t buffer[16];
   char_t bufferout[20];
   char_t bufferin[16];   
   char_t bufferstatus[16];      
   char_t bufferFanMode[20];   
   //Initialize variables
   prevButtonState = 0;

   //Endless loop
   while(1)
   {
     

      //Check connection state
      //if((connectionState == APP_STATE_NOT_CONNECTED))
      if((connectionState == APP_STATE_NOT_CONNECTED) && (cMqttConnectStateFlag == SET))
      {
       
         //Update connection state
         connectionState = APP_STATE_CONNECTING;

         //Try to connect to the MQTT server
         error = mqttConnect();

         //Failed to connect to the MQTT server?
         if(error)
         {
            //Update connection state
            connectionState = APP_STATE_NOT_CONNECTED;
            //Recovery delay
            TRACE_DEBUG("Failed to connect to the MQTT server? %d\r\n",connectionState);            
            HAL_Delay(2000);
            
            cMqttConnectCheck++;
            if(cMqttConnectCheck > 5){
              TRACE_DEBUG("cMqttConnectCheck Error %d\r\n",cMqttConnectCheck);                          
              HAL_Delay(2000);
              NVIC_SystemReset();		//s http 정상이 아니므로 리셋	
            }

         }
         else
         {
            //Update connection state
            connectionState = APP_STATE_CONNECTED;
            mqtt_connect = SET;                 //s 위치 옮겨보자
            mqtt_First_connect = SET;            
            TRACE_DEBUG("APP_STATE_CONNECTED %d\r\n",connectionState);                     
         }
      }
      else
      {
         //Process incoming events
         error = mqttClientProcessEvents(&mqttClientContext, 100);

         //Connection to MQTT server lost?
         if(error != NO_ERROR && error != ERROR_TIMEOUT)
         {
            //Close connection
            mqttClientClose(&mqttClientContext);
            //Update connection state
            connectionState = APP_STATE_NOT_CONNECTED;
            //Recovery delay
            TRACE_DEBUG("Connection to MQTT server lost? %d\r\n",connectionState);               
            osDelayTask(2000);
         
         }
         else
         {
            //Initialize status code
            error = NO_ERROR;

            //Get user button state
            buttonState = BSP_PB_GetState(BUTTON_KEY);

            //Any change detected?
            if(buttonState != prevButtonState)
            {
//               if(buttonState)
//                  strcpy(buffer, "pressed");
//               else
//                  strcpy(buffer, "released");
//
//               //Send PUBLISH packet
//               error = mqttClientPublish(&mqttClientContext, "board/buttons/1",
//                  buffer, strlen(buffer), MQTT_QOS_LEVEL_1, TRUE, NULL);
//
               //Save current state
               prevButtonState = buttonState;
            }
            
            //s 보낼 데이터가 있다면
           if(mqtt_connect == SET){
             if(cMqttSendSelectCount == MQTT_IOT_SEND){
                if(cMqttDataCount && (cMqttSendFlag == 0)){
                  cMqttSendFlag = SET;

                  if((strMqttOut.MqttData[strMqttOut.NowPos].cmd != 0) && (strMqttOut.MqttData[strMqttOut.NowPos].cmd < MQTT_IOT_END)){
                    
  //                      JSON_Value *root_value = json_value_init_object();
  //                      JSON_Object *root_object = json_value_get_object(root_value);  
                    
  //                      char *serialized_string = NULL;                      
                    
                   //Send PUBLISH packet
                    memset(MqttSendBuff, 0, sizeof(MqttSendBuff));
                               
                    if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_INFO_RESPONE){
                      sprintf(MqttSendBuff, "{\"CMD\":\"INFO_RESPONE\",\"VERSION\":\"DW_IOT_V%d_%02d\",\"MAC\":\"%s\",\"STAT\":\"RUN\",\"RUNTIME\":%d, \"SUB_COUNT\":%d}",
                              IOT_VERSION_H, IOT_VERSION_L, MacIdData, iDeviceRunTime, CONFIG_RF_DEVICE_COUNT_2);                   		
                    }
                    else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_REQ_BEBAP_RESPONE){
                      sprintf(MqttSendBuff, "{\"CMD\":\"INFO\",\"NAME\":\"DW_BEHAP\",\"MAC\":\"%s\",\"STAT\":\"RUN\"}",
                              MacIdData);                   		
                    }                    
                    else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_CHANGE_RESPONE){
                        if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_PENDING && strMqttOut.MqttData[strMqttOut.NowPos].TxRelay == 0 ){                        
                          number = 0;
                          if(strMqttOut.MqttData[strMqttOut.NowPos].PendingMode == RF_SUB_PENDING_AUTO_START)
                            value = 1;
                          if(strMqttOut.MqttData[strMqttOut.NowPos].PendingMode == RF_SUB_PENDING_AUTO_STOP)
                            value = 0;       
                        }
                        else{
                            TRACE_INFO("MQTT_IOT_CHANGE_RESPONE value [%d] [%d] [%d]\r\n", strMqttOut.MqttData[strMqttOut.NowPos].Relay, (0x01 << (strMqttOut.MqttData[strMqttOut.NowPos].TxRelay - 1)), strMqttOut.MqttData[strMqttOut.NowPos].TxRelay);                          
                            value = strMqttOut.MqttData[strMqttOut.NowPos].Relay & (0x01 << (strMqttOut.MqttData[strMqttOut.NowPos].TxRelay - 1));                           
                          
                            if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_16CH || strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_SMALL_8CH){

                              number = strMqttOut.MqttData[strMqttOut.NowPos].TxRelay;
                                    if(value)
                                            value = 1;
                            }
                            else if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_PENDING){
                                number = (strMqttOut.MqttData[strMqttOut.NowPos].TxRelay + 1) / 2;
                                if(value){
                                    TRACE_INFO("strMqttOut.MqttData[strMqttOut.NowPos].TxRelay MQTT_TYPE_PENDING [%d] [%d]\r\n", strMqttOut.MqttData[strMqttOut.NowPos].TxRelay, ((strMqttOut.MqttData[strMqttOut.NowPos].TxRelay) % 2));                          

                                    if((strMqttOut.MqttData[strMqttOut.NowPos].TxRelay) % 2){
                                            value = 1;
                                    }
                                    else{
                                            value = 0;                                    
                                    }
                                }
                            }
                            else{
                                number = (strMqttOut.MqttData[strMqttOut.NowPos].TxRelay + 1) / 2;
                                if(value){
                                    TRACE_INFO("strMqttOut.MqttData[strMqttOut.NowPos].TxRelay [%d] [%d]\r\n", strMqttOut.MqttData[strMqttOut.NowPos].TxRelay, ((strMqttOut.MqttData[strMqttOut.NowPos].TxRelay) % 2));                          

                                  if((strMqttOut.MqttData[strMqttOut.NowPos].TxRelay) % 2){
                                            value = 1;
                                    }
                                    else{
                                            value = 2;					
                                    }
                                }
                            }
                        }                            
                      sprintf(MqttSendBuff, "{\"CMD\":\"CHANGE_RESPONE\",\"ADR\":%d,\"TYPE\":%d,\"NUMBER\":%d,\"VALUE\":%d}",
                              strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId, strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType, number, value);
                    }                       
                     else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_HCOW_STAT){

                      memset(bufferout, 0, sizeof(bufferout));
                      memset(bufferin, 0, sizeof(bufferin));              
                      memset(bufferstatus, 0, sizeof(bufferstatus)); 

                      if((strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_16CH) || (strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_SMALL_8CH)){
                              MakeBitText16CH(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay);				
                      }
                      else if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_8CH){
                              MakeBitText8CH(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay, 8);		
                      }
                      else if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_PENDING){
                              MakeBitTextPending(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay, 8);		
                      }			
                      else{
                              for(uint8_t i = 0; i < 16; i++){
                                      bufferout[i] = '0';
                              }			
                      }

                      MakeBitTextIn(bufferin, strMqttOut.MqttData[strMqttOut.NowPos].Input);						
                      
                      if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == 0){
                          sprintf(bufferstatus, "NO_DEVICE");			
                      }
                      else{
                          sprintf(bufferstatus, "OK"); 
                      } 
                                       
                       
                      sprintf(MqttSendBuff, "{\"CMD\":\"HCOW_STAT\",\"ADR\":%d,\"TYPE\":%d,\"OUT\":\"%s\",\"IN\":\"%s\",\"MOVE_TIME1\":%d,\"MOVE_TIME2\":%d,\"MOVE_TIME3\":%d,\"STATUS\":\"%s\",\"AUTO_MODE\":%d}",
                              strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId, strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType, bufferout, bufferin, strMqttOut.MqttData[strMqttOut.NowPos].InTimer1,
                              strMqttOut.MqttData[strMqttOut.NowPos].InTimer2, strMqttOut.MqttData[strMqttOut.NowPos].InTimer3, bufferstatus, strMqttOut.MqttData[strMqttOut.NowPos].MqttFreqSend);
                    }      
                     else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_STATUS_RESPONE){
                       
                      memset(bufferout, 0, sizeof(bufferout));
                      memset(bufferin, 0, sizeof(bufferin));              
                      memset(bufferstatus, 0, sizeof(bufferstatus)); 
                      memset(bufferFanMode, 0, sizeof(bufferFanMode));                       
                      

                      if((strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_16CH) || (strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_SMALL_8CH)){
                              MakeBitText16CH(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay);				
                      }
                      else if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_8CH){
                              MakeBitText8CH(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay, 8);		
                      }
                      else if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == MQTT_TYPE_PENDING){
                              MakeBitTextPending(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay, 8);		
                      }			
                      else{
                              for(uint8_t i = 0; i < 16; i++){
                                      bufferout[i] = '0';
                              }			
                      }

                      MakeBitTextIn(bufferin, strMqttOut.MqttData[strMqttOut.NowPos].Input);						
                      
                      if(strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType == 0){
                          sprintf(bufferstatus, "NO_DEVICE");			
                      }
                      else{
                          sprintf(bufferstatus, "OK"); 
                      } 
                       
                      //fan 자동 모드로 추가됨                 
                      for(uint8_t i = 0; i < 16; i++){
                        if(strMqttOut.MqttData[strMqttOut.NowPos].MqttFanMode[i] == SET){
                              bufferFanMode[i] = '1';
                        }
                        else{
                              bufferFanMode[i] = '0';                          
                        }
                         
                      }		                      
                      
                      sprintf(MqttSendBuff, "{\"CMD\":\"STATUS_RESPONE\",\"ADR\":%d,\"TYPE\":%d,\"OUT\":\"%s\",\"IN\":\"%s\",\"MOVE_TIME1\":%d,\"MOVE_TIME2\":%d,\"MOVE_TIME3\":%d,\"STATUS\":\"%s\",\"AUTO_MODE\":%d,\"FAN_MODE\":\"%s\"}",
                              strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId, strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType, bufferout, bufferin, strMqttOut.MqttData[strMqttOut.NowPos].InTimer1,
                              strMqttOut.MqttData[strMqttOut.NowPos].InTimer2, strMqttOut.MqttData[strMqttOut.NowPos].InTimer3, bufferstatus, strMqttOut.MqttData[strMqttOut.NowPos].MqttFreqSend, bufferFanMode);
                      //sprintf(MqttSendBuff, "{\"CMD\":\"NOTI\",\"FCODE\":%d}",myFcode);    
                     }    
                    else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_CHANGE_RETURN){
                        memset(buffer, 0, sizeof(buffer));                    
                        if(strMqttOut.MqttData[strMqttOut.NowPos].InOutEvent){
                          sprintf(buffer, "IN");	
                          number = strMqttOut.MqttData[strMqttOut.NowPos].InEvent;		
                          strRfRxData[strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId -1].InEvent= 0;	
                          value = strMqttOut.MqttData[strMqttOut.NowPos].Input;                        
                        }
                        else{
                          sprintf(buffer, "OUT");
                          number = strMqttOut.MqttData[strMqttOut.NowPos].RelayEvent;				
                          strRfRxData[strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId -1].RelayEvent = 0;		
                          value = strMqttOut.MqttData[strMqttOut.NowPos].Relay;
                        }                                     
                      
                       sprintf(MqttSendBuff, "{\"CMD\":\"CHANGE_RETURN\",\"ADR\":%d,\"TYPE\":%d,\"FUNC\":%s,\"NUMBER\":%d,\"VALUE\":%d}", 
                               strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId , strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType, buffer, number, value);
                    } 
                    else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_16CH_AUTO_MANUAL){

                      sprintf(MqttSendBuff, "{\"CMD\":\"MANUAL_SET_RESPONE\",\"ADR\":%d,\"TYPE\":%d,\"NUMBER\":%d,\"VALUE\":%d}",
                              strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId, strMqttOut.MqttData[strMqttOut.NowPos].cDeviceType, strMqttOut.MqttData[strMqttOut.NowPos].Relay, strMqttOut.MqttData[strMqttOut.NowPos].RelayEvent);
                    }      
                    else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_BEHAP_CHANGE_RESPONE){                     
                        value = strMqttOut.MqttData[strMqttOut.NowPos].Relay & (0x01 << (strMqttOut.MqttData[strMqttOut.NowPos].TxRelay - 1));              
                        number = (strMqttOut.MqttData[strMqttOut.NowPos].TxRelay + 1) / 2;
                        sprintf(buffer, "STOP");                        
                        if(value){
                            TRACE_INFO("strMqttOut.MqttData[strMqttOut.NowPos].TxRelay [%d] [%d]\r\n", strMqttOut.MqttData[strMqttOut.NowPos].TxRelay, ((strMqttOut.MqttData[strMqttOut.NowPos].TxRelay) % 2));                          

                          if((strMqttOut.MqttData[strMqttOut.NowPos].TxRelay) % 2){
                               sprintf(buffer, "FORWARD");
                            }
                            else{
                               sprintf(buffer, "REVERSE");				
                            }
                        }
                        
                      sprintf(MqttSendBuff, "{\"CMD\":\"CHANGE_RESPONE\",\"ACT\":%s,\"VALUE\":%d,\"FCODE\":%d}",
                              buffer, strMqttOut.MqttData[strMqttOut.NowPos].TxRelay, myFcode);                        
                      
//                        strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId = strRfRxData[id].Id;        
//                        strMqttOut.MqttData[strMqttOut.NowPos].TxRelay = strRfTxData[strRfRxData[id].Id - 1].Relay;
//                        strMqttOut.MqttData[strMqttOut.NowPos].Relay = strRfRxData[id].Relay; 

                    }      
                    else if(strMqttOut.MqttData[strMqttOut.NowPos].cmd == MQTT_IOT_BEHAP_STATUS_RESPONE){
                        memset(bufferout, 0, sizeof(bufferout));                       
                        MakeBitText8CHBehap(bufferout, strMqttOut.MqttData[strMqttOut.NowPos].Relay, 8);		
//                        strMqttOut.MqttData[strMqttOut.NowPos].cDeviceId = id;
//                        strMqttOut.MqttData[strMqttOut.NowPos].Relay = strRfRxData[id-1].Relay;  
                      
                        sprintf(MqttSendBuff, "{\"CMD\":\"STATUS_RESPONE\",\"OUTn\":%s}", bufferout);   
                    }                              
                    else{
                        TRACE_DEBUG("error mqtt commend %d\r\n",strMqttOut.MqttData[strMqttOut.NowPos].cmd);			
                    }                  
                    
                        TRACE_INFO("MQTT SEND : %s\r\n", MqttSendBuff);                   
                        error = mqttClientPublish(&mqttClientContext, MyTopic,
                        MqttSendBuff, strlen(MqttSendBuff), MQTT_QOS_LEVEL_1, TRUE, NULL);									

                  }		                    
                  
                  if(error == NO_ERROR){
                    cMqttSendFlag = 0;
                    strMqttOut.NowPos++;

                    if(strMqttOut.NowPos >= MAIN_MQTT_MAX_BUFF){
                            strMqttOut.NowPos = 0;
                    }	                    
                    
                    TRACE_DEBUG(" mqtt SEND Now pos %d\r\n",strMqttOut.NowPos);		                  
                    cMqttDataCount--;
                  }
                  else{
                    TRACE_DEBUG(" mqtt SEND Error %d\r\n",error);		                  
                  }
                  
                  //break;            //s save finish   
              
             }
                       
            }
            else if(cMqttSendSelectCount == MQTT_INFO_SEND){
                if(cMqttDataInfoCount && (cMqttSendInfoFlag == 0)){
                  cMqttSendInfoFlag = SET;

                  if((strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd != 0) && (strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd < MQTT_IOT_INFO_END)){
                    
  //                      JSON_Value *root_value = json_value_init_object();
  //                      JSON_Object *root_object = json_value_get_object(root_value);  
                    
  //                      char *serialized_string = NULL;                      
                    
                   //Send PUBLISH packet
                    memset(MqttSendBuff, 0, sizeof(MqttSendBuff));
                    if(strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd == MQTT_IOT_INFO_NOTI){
  //                          json_object_set_string(root_object, "CMD", "NOTI");
  //                          json_object_set_number(root_object, "FCODE", myFcode);
  //                          
  //                          serialized_string = json_serialize_to_string_pretty(root_value);                          
  //                          sprintf(MqttSendBuff, "{%s}",serialized_string);            
                      sprintf(MqttSendBuff, "{\"CMD\":\"NOTI\",\"FCODE\":%d,\"RUNTIME\":%d}",myFcode, iDeviceRunTime);    
                      cMqttNotiCount++;
                      if(cMqttNotiCount > 4 ){
                         TRACE_INFO("NETWORK MAY BE CLOSED. NEXT TIME RESET %d\r\n", cMqttNotiCount);  
                      }
                      
                      if(cMqttNotiCount > 5){
                         TRACE_INFO("NETWORK MAY BE CLOSED. WILL BE RESET AFTER 2MINIUT%d\r\n", cMqttNotiCount);                          
                          HAL_Delay(2000);                      
                          NVIC_SystemReset();		//s Mqtt 정상이 아니므로 리셋	                     
                      }
               
                    }                      
                    else if(strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd == MQTT_IOT_INFO_SENSOR){
                          sprintf(MqttSendBuff, "{\"DATA\":\"SENSOR\",\"ADR\":%d,\"TEMP\":%3.1f,\"HUM\":%3.1f,\"CO2\":%04d,\"AMM\":%3.1f}",
                                  strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cDeviceId + 1, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].temp, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].humudity,
                                  strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].Co2, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].Ammonia);                        		
                    }
                    else if(strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd == MQTT_IOT_INFO_WATER){
                          sprintf(MqttSendBuff, "{\"DATA\":\"WATER\",\"ADR\":%d,\"WV\":%d,\"WP\":%d,\"STAT\":%d}",
                                  strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cDeviceId + 1, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].WaterValue, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].WaterPresure,
                                  strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cWaterEvent);                         		
                    }                              
                    else if(strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd == MQTT_IOT_INFO_WEIGHT){
                          sprintf(MqttSendBuff, "{\"DATA\":\"WEIGHT\",\"ADR\":%d,\"WEI\":%d}",
                                  strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cDeviceId + 1, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].Weitgh);
                    } 
                    else if(strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd == MQTT_IOT_INFO_ERROR){
                      sprintf(MqttSendBuff, "{\"CMD\":\"ERROR\",\"RUNTIME\":%d,\"WATCHDOG\":%d,\"ADR\":%d,\"AUTO\":%d, \"TEMP\":%d, \"SENSOR\":%d}",
                              iDeviceRunTime , wErrorWatchdogCheckCont, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cDeviceId + 1, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].ErrorAutoman, 
                              strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].ErrorTemp, strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].ErrorSensor);
                    }                          
                    else{
                                      TRACE_DEBUG("error mqtt commend %d\r\n",strMqttOutInfo.MqttData[strMqttOutInfo.NowPos].cmd);			
                    }                  
                    
                        TRACE_INFO("MQTT SEND : %s\r\n", MqttSendBuff);                   
                        error = mqttClientPublish(&mqttClientContext, MyInfoTopic,
                        MqttSendBuff, strlen(MqttSendBuff), MQTT_QOS_LEVEL_1, TRUE, NULL);									

                  }		                    
                  
                  if(error == NO_ERROR){
                    cMqttSendInfoFlag = 0;
                    strMqttOutInfo.NowPos++;

                    if(strMqttOutInfo.NowPos >= MAIN_MQTT_MAX_BUFF){
                            strMqttOutInfo.NowPos = 0;
                    }	                    
                    
                    TRACE_DEBUG(" mqtt SEND Now pos %d\r\n",strMqttOutInfo.NowPos);		                  
                    cMqttDataInfoCount--;
                  }
                  else{
                    TRACE_DEBUG(" mqtt SEND Error %d\r\n",error);		                  
                  }
                  
                  //break;            //s save finish   
              
             }
                       
            }            
           }
            //Failed to publish data?
            if(error)
            {
               //Close connection
               mqttClientClose(&mqttClientContext);
               //Update connection state
               connectionState = APP_STATE_NOT_CONNECTED;
               //Recovery delay
               osDelayTask(2000);
            }   
         }
      }
      cMqttSendSelectCount++;
      if(cMqttSendSelectCount > MQTT_INFO_SEND){
        cMqttSendSelectCount = MQTT_IOT_SEND;
      }
      
      osDelayTask(5);
   }
}

uint8_t  testhttpread = 0;

/**
 * @brief LED blinking task
 **/

void blinkTask(void *param)
{
   //Endless loop
   while(1)
   {
      BSP_LED_Toggle(LED1);

      if(cBuzzerCount){
        cBuzzerCount--;
        HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_SET);         
      }
      else{
        HAL_GPIO_WritePin(BUZ_GPIO_Port, BUZ_Pin, GPIO_PIN_RESET);              
      }
      
        //Loop delay
        osDelayTask(100);     
   }
}



/**
 * @brief Http Get Task
 **/

uint8_t http_main_flag = 0,http_main_flag2 = 0;

void HttpGetTask(void *param)
{
//  static long ticktime=0;
  
   //Endless loop
   while(1)
   {
     
     if(connectionState == APP_STATE_CONNECTED){     
     
        if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK)){
            cHttpReadCount++;
                
            if((cCheckTimeSetId != 0) && (HttpTimeReadFirstFlag != 0)){
                    cHttpReadCount = 0; //s 5초동안 http 읽지 못함
                    HttpClientRead(HTTP_GET_DEVICE_SET, cCheckTimeSetId);						
            }
    
            if(cHttpReadCount >= 5){
                    if(HttpTimeReadFirstFlag == RESET){				//s 한시간마다 읽을까 하루마다 읽을까?
                            HttpClientRead(HTTP_GET_TIME, 1);
                    }
                    else{
                      if(HttpDeviceReadFirstFlag[cHttpDeviceCount - 1] == SET){
                          HttpClientRead(HTTP_GET_DEVICE_SET, cHttpDeviceCount);
                          cHttpDeviceCount++;
                      }
                      if(cHttpDeviceCount > cDeviceCnt){		//s 오버 플로어 방지
                              cHttpDeviceCount = 1;
                      }									
                    }
                    cHttpReadCount = 0;									
            }
            
            cWatchdogFlag |= 0x04;
        }     
        else if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN)){
            cHttpReadCount++;
    
            if((cCheck16ChSetId != 0) && (HttpTimeReadFirstFlag != 0)){
                    cHttpReadCount = 0; //s 5초동안 http 읽지 못함
                    HttpClientRead(HTTP_GET_16CH_DEVICE_SET, cCheck16ChSetId);						
            }            
            
            if(cHttpReadCount >= 5){
                if(HttpTimeReadFirstFlag == RESET){				//s 한시간마다 읽을까 하루마다 읽을까?
                   HttpClientRead(HTTP_GET_TIME, 1);
                   TRACE_DEBUG("HTTP_GET_TIME :%d\r\n", 1);                                    
                }
                else{
                  if(strTemp16ChControl[cHttpDeviceCount - 1].cHttpDeviceReadFlag == SET){
                        HttpClientRead(HTTP_GET_16CH_DEVICE_SET, cHttpDeviceCount);
                      TRACE_DEBUG("HttpClientRead  HTTP_GET_16CH_DEVICE_SET :%d\r\n", cHttpDeviceCount);                           
                      cHttpDeviceCount++;
                  }							

                  for(uint8_t i = 0; i < 16; i++){
                    if(strTemp16ChControl[cHttpDeviceCount - 1].cDeviceSetting == SET){
                      //TRACE_DEBUG("strTemp16ChControl[cHttpDeviceCount - 1].cDeviceSetting  SET :%d\r\n", cHttpDeviceCount);                                 
                      break; //s 데이터가 있음
                    }
                    else{
                      if(cHttpDeviceCount < 17){
                        cHttpDeviceCount++;
                      }
                      else{
                        cHttpDeviceCount = 1;          //s 16ch 한바퀴 돌았으므로 0부터 시작
                      }
                    }
                  } 
                }
                
                cHttpReadCount = 0;									
            }
        }             
        
     }     
         
     //Loop delay
      osDelayTask(1000);      
   }
}


void HttpSendTask(void *param)
{
//  static long ticktime=0;
  
   //Endless loop
    while(1) {

        if(cHttpSendCount && (cHttpSendFlag == 0)){				
          sprintf(buffGLcd1Line, "HTTP READY!:%03d  ", cHttpSendCount);									
          TM_HD44780_Puts_State(0,0, buffGLcd1Line);           
				
          cHttpSendFlag = 1;
          if(strSen_value.HttpData[strSen_value.NowPos].cType == HTTP_TYPE_LORA){				
            if(strSen_value.HttpData[strSen_value.NowPos].cPos < 4){			//s 환경 센서	
                    if(strSen_value.HttpData[strSen_value.NowPos].Event == 1){			//s 이전데이터와 같으면 서버에만 데이터를 보내지 않음						
                            HttpClientWrite(strSen_value.HttpData[strSen_value.NowPos].cPos, HTTP_SEND_SENSOR);	
                    }
                    //publish_message(MyTopic, client, Json_ret_Sen_data_send(&strSen_value.HttpData[strSen_value.NowPos]));	
                    DawoonMqttSendInfo(MQTT_IOT_INFO_SENSOR, strSen_value.NowPos, 0);                    
            }
            else if(strSen_value.HttpData[strSen_value.NowPos].cPos < 8){		//s 음수 센서
                    if(strSen_value.HttpData[strSen_value.NowPos].Event == 1){			//s 이전데이터와 같으면 서버에만 데이터를 보내지 않음
                            HttpClientWrite(strSen_value.HttpData[strSen_value.NowPos].cPos, HTTP_SEND_WATER);		
                            strSen_value.HttpData[strSen_value.NowPos].WaterMakeValue = 0;                       
                    }
                    //publish_message(MyTopic, client, Json_ret_Sen_data_send(&strSen_value.HttpData[strSen_value.NowPos]));
                    DawoonMqttSendInfo(MQTT_IOT_INFO_WATER, strSen_value.NowPos, 0);                  
            }			
            else if(strSen_value.HttpData[strSen_value.NowPos].cPos < 16){		//s 사료 저울 센서
                    if(strSen_value.HttpData[strSen_value.NowPos].Event == 1){			//s 이전데이터와 같으면 서버에만 데이터를 보내지 않음
                            HttpClientWrite(strSen_value.HttpData[strSen_value.NowPos].cPos, HTTP_SEND_WEIGHT);
                    }
                    //publish_message(MyTopic, client, Json_ret_Sen_data_send(&strSen_value.HttpData[strSen_value.NowPos]));
                    DawoonMqttSendInfo(MQTT_IOT_INFO_WEIGHT, strSen_value.NowPos, 0);                             
            }
          }
          else if(strSen_value.HttpData[strSen_value.NowPos].cType == HTTP_TYPE_AUTOMAN){			//s 계류기 정보 이동
                  HttpClientWrite(strSen_value.HttpData[strSen_value.SavePos].cPos, HTTP_SEND_PANDING);		
          }
          
          cHttpSendFlag = 0;
          strSen_value.NowPos++;

          if(strSen_value.NowPos >= MAIN_HTTP_MAX_BUFF){
                  strSen_value.NowPos = 0;
          }						

                cHttpSendCount--;				
        } 
        else if(cHttpWeitghSendCount && (cHttpSendFlag == 0)){
                cHttpSendFlag = 1;
                //HttpClientWrite(0, HTTP_SEND_WEIGHT_KHH);
                cHttpSendFlag = 0;
                cHttpWeitghSendCount--;
        }				

        if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK)){
        
                if((cWatchdogFlag & 0x07) == 0x07){	//s 다른 루프도 정상적이라면
                  cWatchdogFlag = 0;
                  __HAL_IWDG_RELOAD_COUNTER(&hiwdg);                  //s watchdog count reset
                  //TRACE_DEBUG(" __HAL_IWDG_RELOAD_COUNTER 0x07 \r\n");	                        
                        
                }
        }
        else{
                if((cWatchdogFlag & 0x03) == 0x03){	//s 다른 루프도 정상적이라면
                  cWatchdogFlag = 0;
                  __HAL_IWDG_RELOAD_COUNTER(&hiwdg);                  //s watchdog count reset
                  //TRACE_DEBUG(" __HAL_IWDG_RELOAD_COUNTER 0x03 \r\n");	                        
                }				
        }
     //Loop delay		
      osDelayTask(100);         //100mS대기!
    }
}


/**
 * @brief user  Task
 **/
void userTask(void *param)
{
//  static long ticktime=0;
  
   //Endless loop
   while(1)
   {
      SW_check();
      setting_mode();
      
      if(loop_50ms++ > 5)
      {
          parse_nx8040(buffLCDRxDataSave, cLCDRxdataCount);
          loop_50ms = 0;
          
          if(lcd_icon.current_page == 15)
          {
            char tmp[10];
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fC", strSen_value.HttpData[0].temp);
            NxLcdText25(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dppm", strSen_value.HttpData[0].Co2);
            NxLcdText26(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dkg", strSen_value.HttpData[0].Weitgh);
            NxLcdText33(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[0].humudity);
            NxLcdText34(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[0].Ammonia);
            NxLcdText41(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dL", strSen_value.HttpData[0].WaterValue);
            NxLcdText42(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fC", strSen_value.HttpData[1].temp);
            NxLcdText27(tmp);
            sprintf(tmp, "%dppm", strSen_value.HttpData[1].Co2);
            NxLcdText28(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dkg", strSen_value.HttpData[1].Weitgh);
            NxLcdText35(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[1].humudity);
            NxLcdText36(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[1].Ammonia);
            NxLcdText43(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dL", strSen_value.HttpData[1].WaterValue);
            NxLcdText44(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fC", strSen_value.HttpData[2].temp);
            NxLcdText29(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dppm", strSen_value.HttpData[2].Co2);
            NxLcdText30(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dkg", strSen_value.HttpData[2].Weitgh);
            NxLcdText37(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[2].humudity);
            NxLcdText38(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[2].Ammonia);
            NxLcdText45(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dL", strSen_value.HttpData[2].WaterValue);
            NxLcdText46(tmp);
           memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fC", strSen_value.HttpData[3].temp);
            NxLcdText31(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dppm", strSen_value.HttpData[3].Co2);
            NxLcdText32(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dkg", strSen_value.HttpData[3].Weitgh);
            NxLcdText39(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[3].humudity);
            NxLcdText40(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[3].Ammonia);
            NxLcdText47(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dL", strSen_value.HttpData[3].WaterValue);
            NxLcdText48(tmp);

          }
          else if(lcd_icon.current_page == 16)
          {
            char tmp[10];
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fC", strSen_value.HttpData[0].temp);
            NxLcdText19(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dppm", strSen_value.HttpData[0].Co2);
            NxLcdText26(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dkg", strSen_value.HttpData[0].Weitgh);
            NxLcdText33(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[0].humudity);
            NxLcdText40(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%2.1fppm", strSen_value.HttpData[0].Ammonia);
            NxLcdText47(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp, "%dL", strSen_value.HttpData[0].WaterValue);
            NxLcdText54(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp,"%d.%d.%d",lora_date[0].year,lora_date[0].month,lora_date[0].date);
            NxLcdText20(tmp);
            NxLcdText27(tmp);
            NxLcdText34(tmp);
            NxLcdText41(tmp);
            NxLcdText48(tmp);
            NxLcdText55(tmp);
            memset((void*)tmp, 0x00, 10);
            sprintf(tmp,"%d:%d",lora_date[0].hour, lora_date[0].min);
            NxLcdText21(tmp);
            NxLcdText28(tmp);
            NxLcdText35(tmp);
            NxLcdText42(tmp);
            NxLcdText49(tmp);
            NxLcdText56(tmp);
          }
      }
      
      if(event_occur)
      {
          global_tick = GTick();
          if(lcd_icon.current_page == 2 || lcd_icon.current_page == 3 || lcd_icon.current_page == 4)
          {
              if(loop_1000ms++ > 100)
              {
                  second++;
                  if(old_second != second)
                  {
                      if(lcd_icon.current_page == 2)
                          NxLcdShowCom(18,1);
                      else if(lcd_icon.current_page == 3 || lcd_icon.current_page == 4)
                          NxLcdShowCom(30,1);

                      NxLcdSencond(second);
                      old_second = second;
                  }
                  loop_1000ms = 0;
              }
          }
         
          if(global_tick - local_tick > 0 && global_tick - local_tick < 4000)
          {
              if(lcd_icon.current_page == 14)
                  NxLcdShowCom(19,1);
          }
          else if(global_tick - local_tick > 5000 && global_tick - local_tick < 7000)
          {
              if(lcd_icon.current_page == 2)
                  NxLcdShowCom(17,1);
              else if(lcd_icon.current_page == 3 || lcd_icon.current_page == 4)
                  NxLcdShowCom(29,1);
          }
          else if (global_tick - local_tick > 8000 && global_tick - local_tick < 10000)
          {
                if(lcd_icon.current_page == 14)
                   NxLcdShowCom(19,0);
                else
                {
                  second = 0;
                  old_second = 0;
               
                  if(lcd_icon.current_page == 2)
                      NxLcdShowCom(18,0);
                  else if(lcd_icon.current_page == 3 || lcd_icon.current_page == 4)
                      NxLcdShowCom(30,0);
                  
                  if(lcd_icon.current_page == 2)
                      NxLcdShowCom(17,0);
                  else if(lcd_icon.current_page == 3 || lcd_icon.current_page == 4)
                      NxLcdShowCom(29,0);
                }
                
                global_tick = 0;
                local_tick = 0;
                event_occur = 0;
                
                if(lcd_icon.current_page != 1 && lcd_icon.current_page != 2)
                  Set_Page16(lcd_icon.iconbuffer[lcd_icon.current_page]);
                else if(lcd_icon.current_page == 2)
                {
                  char bufferout[16] = {0,};
                  MakeBitText8CH(bufferout, lcd_icon.iconbuffer[lcd_icon.current_page], 16);
                  Set_Page8(bufferout);
                }
          }
      }
      
      
        if(loop_200ms++ >= 19) {
            loop_200ms = 0;

            if(wCheckSec++ >= 5){			//s 계사 수동 컨트롤 용
                    wCheckSec = 0;
                    wCheckMinite++;
					
                    if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK)){						
                      for(uint8_t j = 0; j < 4; j++){
                          if(strManualSwControl[j].cFeedSwFlag){
                                  if (strManualSwControl[j].wFeedManualTime){
                                          strManualSwControl[j].wFeedManualTime--;
                                  }
                                  else{
                                          strManualSwControl[j].cFeedSwFlag = RESET;
                                          //publish_message(MyTopic, client, Json_device_status(j+1, MQTT_STATUS_RESPONE));										
                                  }
                          }
                          if(strManualSwControl[j].cLight1SwFlag){
                                  if (strManualSwControl[j].wLight1ManualTime){
                                          strManualSwControl[j].wLight1ManualTime--;
                                  }
                                  else{
                                          strManualSwControl[j].cLight1SwFlag = RESET;				
                                          //publish_message(MyTopic, client, Json_device_status(j+1, MQTT_STATUS_RESPONE));												
                                  }
                          }
                          if(strManualSwControl[j].cLight2SwFlag){
                                  if (strManualSwControl[j].wLight2ManualTime){
                                          strManualSwControl[j].wLight2ManualTime--;
                                  }
                                  else{
                                          strManualSwControl[j].cLight2SwFlag = RESET;		
                                          //publish_message(MyTopic, client, Json_device_status(j+1, MQTT_STATUS_RESPONE));												
                                  }
                          }
                          if(strManualSwControl[j].cHeater1SwFlag){
                                  if (strManualSwControl[j].wHeater1ManualTime){
                                          strManualSwControl[j].wHeater1ManualTime--;
                                  }
                                  else{
                                          strManualSwControl[j].cHeater1SwFlag = RESET;		
                                          //publish_message(MyTopic, client, Json_device_status(j+1, MQTT_STATUS_RESPONE));												
                                  }
                          }
                          if(strManualSwControl[j].cHeater2SwFlag){
                                  if (strManualSwControl[j].wHeater2ManualTime){
                                          strManualSwControl[j].wHeater2ManualTime--;
                                  }
                                  else{
                                          strManualSwControl[j].cHeater2SwFlag = RESET;				
                                          //publish_message(MyTopic, client, Json_device_status(j+1, MQTT_STATUS_RESPONE));												
                                  }
                          }
                          if(strManualSwControl[j].cHeater3SwFlag){
                                  if (strManualSwControl[j].wHeater3ManualTime){
                                          strManualSwControl[j].wHeater3ManualTime--;
                                  }
                                  else{
                                          strManualSwControl[j].cHeater3SwFlag = RESET;				
                                          //publish_message(MyTopic, client, Json_device_status(j+1, MQTT_STATUS_RESPONE));												
                                  }
                          }							
                      }						
                }
            }
            
            if(wCheckMinite >= 60){
                wCheckMinite = 0;
//                iDeviceRunTime++;
            }
            
            if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN)){
              
                HAL_RTC_GetTime(&hrtc, &strMainTime, FORMAT_BIN);
                HAL_RTC_GetDate(&hrtc, &strMainData, FORMAT_BIN);  

                wDawoonTime = (strMainTime.Hours * 60) + strMainTime.Minutes;
                
//                sprintf(buffGLcd1Line, "TIME %02d:%02d:%02d ", strMainTime.Hours, strMainTime.Minutes,  strMainTime.Seconds);
//                
                
                if(wDawoonTime == 0){              //s 날자가 바뀌면 정보 전달                                     
                  if(cDawoonInfoTimeFlagOn == RESET){
                      cDawoonInfoTimeFlagOn = SET;
                      wDawoonInfoTimeBack = 0;
                      //wDawoonWeterSendTime = 58;              //s 58분에 보내고자 함
                      cTimeResettingFlagOn = RESET;
                       if(connectionState == APP_STATE_CONNECTED){     
                          DawoonMqttSendInfo(MQTT_IOT_INFO_NOTI, 0, 0);
                       }
                      memset(cWeterSendFlag, SET, sizeof(cWeterSendFlag));                       
                     TRACE_DEBUG("wDawoonTime == 0 [%d]\r\n", wDawoonTime);                           
                  }

                }
                else{
                  cDawoonInfoTimeFlagOn = RESET;
                }                
                
              
              if(wDawoonTime > 720){		//s 정오에 시간 업데이트 진행
                  if(cTimeResettingFlagOn == RESET){
                      cTimeResettingFlagOn = SET;
                      HttpTimeReadFirstFlag = RESET;		//s 시간 업데이트
                  }
                }                
                
                if(wDawoonTime >= (wDawoonInfoTimeBack + 1)){                      //s 1분마다 장비 상태를 알림          
                   wDawoonInfoTimeBack = wDawoonTime;
                   iDeviceRunTime += 1;
                     if(connectionState == APP_STATE_CONNECTED){ 
                        DawoonMqttSendInfo(MQTT_IOT_INFO_NOTI, 0, 0); 
                     }                                   
                }


                  if(wDawoonTime >= (wDawoonTimeBack + 10)){			//s 1시간마다 장비 데이터 업데이트 받음
                      if(cDeviceFlagOn == RESET){
                          cDeviceFlagOn = SET;		//s 00분일때 한번만 들어오게
                          wDawoonTimeBack = wDawoonTime;
                          TRACE_DEBUG("(wDawoonTime >= (wDawoonTimeBack + 60) [%d]\r\n", wDawoonTime);				
                          for(uint8_t i = 0; i < 16; i++){
                            if(strTemp16ChControl[i].cDeviceSetting == SET){
                              strTemp16ChControl[i].cHttpDeviceReadFlag = SET;              //s 읽을수 있게 플래그를 셋팅함
                              TRACE_DEBUG("strTemp16ChControl[i].cHttpDeviceReadFlag SET :%d\r\n", i+1);  
                            }
                          }
                      }
                  }
                  else{
                      cDeviceFlagOn = RESET;
                  }

                
//                if(wDawoonTime >= wDawoonWeterSendTime){                 //s 58분에 보내려함
//                   wDawoonWeterSendTime = ((strMainTime.Hours + 2) * 60) - 2;                  
//                   memset(cWeterSendFlag, SET, sizeof(cWeterSendFlag));
//                     TRACE_DEBUG("wDawoonTime >= (wDawoonWeterSendTime [%d]\r\n", wDawoonInfoTimeBack);                               
//                }
                
                  if(strMainTime.Minutes == 58){                 //s 58분에 보내려함
                    if(cTimeWaterSettingFlagOn == RESET){
                      cTimeWaterSettingFlagOn = SET;
                     //wDawoonWeterSendTime = ((strMainTime.Hours + 2) * 60) - 2;                  
                      memset(cWeterSendFlag, SET, sizeof(cWeterSendFlag));
                       TRACE_DEBUG("((strMainTime.Minutes == 58) && (cTimeWaterSettingFlagOn == RESET)) wDawoonTime[%d]\r\n", wDawoonTime);          
                    }
                  }                           
                  else{
                    cTimeWaterSettingFlagOn = RESET;
                  }                
                
            }
        }

        if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK) ){		
                cTimeDisplay++;
                if(cTimeDisplay >= 50){
                        if(cIpDisplay){
                                cIpDisplay--;			//s ip 볼 시간 좀 주자		1s 주기
                                cIpDisplayEndFlag = SET;
                        }
                        else{
                          
                              HAL_RTC_GetTime(&hrtc, &strMainTime, FORMAT_BIN);
                              HAL_RTC_GetDate(&hrtc, &strMainData, FORMAT_BIN);  

                              wDawoonTime = (strMainTime.Hours * 60) + strMainTime.Minutes;
                              
                              sprintf(buffGLcd1Line, "TIME %02d:%02d:%02d ", strMainTime.Hours, strMainTime.Minutes,  strMainTime.Seconds);       
                              TM_HD44780_Puts_State(0,0,buffGLcd1Line);                             

                                if(wDawoonTime < 2){		//s 기준시간 전에 flag 온시킴 3분간 체크
                                    if(cFeederFlagOn == RESET){

                                        TRACE_DEBUG("wDawoonTime < 2 [%d]\r\n", wDawoonTime);
                                                                       
                                        cTimeResettingFlagOn = RESET;				//s 시간 업데이트 진행	플레그 셋
                                        wDawoonTimeBack = 0;
                                        wDawoonInfoTimeBack = 0;           
                                        //wDawoonWeterSendTime = 58;              //s 58분에 보내고자 함

                                        for(uint8_t i = 0; i < cDeviceCnt; i++){
//                                          if(RfDeviceEnableFlag[i] == SET){
                                              memset(strTimeControl[i].cFeedTimeFlag, SET, sizeof(strTimeControl[i].cFeedTimeFlag));
                                              HttpDeviceReadFirstFlag[i] = SET;
                                              strRfRxData[i].Mode = MQTT_TYPE_SMALL_8CH;
//                                          }
                                        }
                                        cFeederFlagOn = SET;
                                         if(connectionState == APP_STATE_CONNECTED){     
                                            DawoonMqttSendInfo(MQTT_IOT_INFO_NOTI, 0, 0); 
                                         }                                        

                                          memset(cWeterSendFlag, SET, sizeof(cWeterSendFlag));
                                         
                                    }
                                }
                                else{
                                    cFeederFlagOn = RESET;
                                }
                                
                                if(wDawoonTime > 720){		//s 정오에 시간 업데이트 진행
                                    if(cTimeResettingFlagOn == RESET){
                                        cTimeResettingFlagOn = SET;
                                        HttpTimeReadFirstFlag = RESET;		//s 시간 업데이트
                                    }
                                }

                                
                                if(wDawoonTime >= (wDawoonTimeBack + 10)){			//s 1시간마다 장비 데이터 업데이트 받음
                                    if(cDeviceFlagOn == RESET){
                                        cDeviceFlagOn = SET;		//s 00분일때 한번만 들어오게
                                        wDawoonTimeBack = wDawoonTime;
                                        TRACE_DEBUG("(wDawoonTime >= (wDawoonTimeBack + 60) [%d]\r\n", wDawoonTime);				
                                        for(uint8_t i = 0; i < cDeviceCnt; i++){
                                                HttpDeviceReadFirstFlag[i] = SET;
                                                strRfRxData[i].Mode = MQTT_TYPE_SMALL_8CH;										//s 계사 프로그램은 이미 장비가 정해져 있고 에러 체크를 위해 무조건 발송									
                                        }
                                    }
                                }
                                else{
                                    cDeviceFlagOn = RESET;
                                }
                                
                                
                               if(wDawoonTime >= (wDawoonInfoTimeBack + 1)){			//s 1분마다 장비 상태를 알림       
                                   wDawoonInfoTimeBack = wDawoonTime;
                                    iDeviceRunTime += 1;                                   
                                     if(connectionState == APP_STATE_CONNECTED){     
                                        DawoonMqttSendInfo(MQTT_IOT_INFO_NOTI, 0, 0); 
                                     }                   
                               }                                
                                
                                if(strMainTime.Minutes == 58){                 //s 58분에 보내려함
                                  if(cTimeWaterSettingFlagOn == RESET){
                                    cTimeWaterSettingFlagOn = SET;
                                   //wDawoonWeterSendTime = ((strMainTime.Hours + 2) * 60) - 2;                  
                                    memset(cWeterSendFlag, SET, sizeof(cWeterSendFlag));
                                     TRACE_DEBUG("((strMainTime.Minutes == 58) && (cTimeWaterSettingFlagOn == RESET)) wDawoonTime[%d]\r\n", wDawoonTime);          
                                  }
                                }                       
                                else{
                                  cTimeWaterSettingFlagOn = RESET;
                                }
                                
                                
                                if(cIpDisplayEndFlag){    
                                        TM_HD44780_Puts_State(0,0,"                ");                                                       
                                        cIpDisplayEndFlag = RESET;
                                }
                        }
                        cTimeDisplay = 0;

					
        }
//        for(uint8_t i = 0; i < 4; i++){
//            if(RfDeviceEnableFlag[i] == SET){
//                if((wErrorAutomanCheckCont[i] > 100) || (wErrorTempCheckCont[i] > 100) || (wErrorSensorCheckCont[i] > 100)){	
//                     cBuzzerCount = 2;	//s 경고
//                }
//            }
//        }
    }				
        
    if(MqttStatusFlag != 0){
            MqttStatusFlag--;
            if(MqttStatusFlag == 0){
                    memset(buffMqttStatusFlag, 0, sizeof(buffMqttStatusFlag));
            }
                    
    }
    
    cWatchdogFlag |= 0x02;     

    if((cMqttConnectStateFlag == RESET) && (netInterface[0].ipv4Context.addr != 0)){             //s dhcp가 붙고나서 Mqtt연결시도
      if(iMqttConnectStateCount++ >100){        //s 1초 기다림
        cMqttConnectStateFlag = SET;
      }
   }
     //Loop delay
      osDelayTask(10);   
      
   }
}

/**
 * @brief Uart Task
 **/
void UartTask(void *param)
{
	#ifdef WIRELESS1_ENABLE
		wireless1.attach(&wireless1_Rx_interrupt, Serial::RxIrq);
    wireless1.attach(&wireless1_Tx_interrupt, Serial::TxIrq);
	#endif
    //char get_rx = 0;
	#ifdef WIRELESS2_ENABLE	
		lora2.attach(&lora2_Rx_interrupt, Serial::RxIrq);
		lora2.attach(&lora2_Tx_interrupt, Serial::TxIrq);
	#endif
 //   WOut.address = 1;

    while(1) {
            
        switch(CONFIG_RF_TYPE_3) {
          case DEVICE_TYPE_AUTOMAN_3025:				//s 신규 모듈 변경으로 인한 로라 삭제후 추가
              if(serial_change == 0){
                RfAutomanLoop(0);	
              }                     
              else if(serial_change == 1){		
                RfloraLoop();		
              }					
          break;

          case DEVICE_TYPE_AUTOMAN:
            if(serial_change == 0){
              RfAutomanLoop(1);	
            }						
            else if(serial_change == 1){		
              RfloraLoop();		
            }			
          break;
						
          case DEVICE_TYPE_LAZER_WEIGHT:
            if(serial_change == 0){
                    //RfAutomanLoop();	
            }				
            else if(serial_change == 1){		
                RfloraLoop();		
            }	
            break;

          case DEVICE_TYPE_AUTOMAN_WATER:
            if(serial_change == 0){
              RfAutomanLoop(1);	
            }					
            else if(serial_change == 1){			
               RfloraLoop();		
            }
            break;
            
            case DEVICE_TYPE_LORA_SENSOR:
              if(serial_change == 0){
                      //RfAutomanLoop();	
              }						
              else if(serial_change == 1){		
                  RfloraLoop();		
              }
            break;

          case DEVICE_TYPE_AUTOMAN_3025_CHICK:				//s 신규 모듈 변경으로 인한 로라 삭제후 추가
            if(serial_change == 0){
                RfAutomanLoop(0);	
            }									
            else if(serial_change == 1){
                RfloraLoop();
            }		
          break;

        case DEVICE_TYPE_AUTOMAN_CHICK:
          if(serial_change == 0){
              RfAutomanLoop(1);	
          }						
          else if(serial_change == 1){		
              RfloraLoop();
          }					
          break;

          case DEVICE_TYPE_AUTOMAN_3025_BEHAP:				//s 신규 모듈 변경으로 인한 로라 삭제후 추가
            if(serial_change == 0){
                RfAutomanBehapLoop(0);	
            }									
            else if(serial_change == 1){
                RfloraLoop();
            }		
          break;

        case DEVICE_TYPE_AUTOMAN_BEHAP:
          if(serial_change == 0){
              RfAutomanBehapLoop(1);	
          }						
          else if(serial_change == 1){		
              RfloraLoop();
          }					
          break;          
          
          
				
            case 0:
            default:
                break;

        }


    while(testWatchdog);        
        
        //일정시간동안 버퍼값이 갱신되지 않으면!!
        serial_change++;
        
        if(serial_change >= 2){
                serial_change = 0;
        }
        cWatchdogFlag |= 0x01; //s for watchdog
        
      osDelayTask(1);     
    }
}

/**
 * @brief Main entry point
 * @return Unused value
 **/

int_t main(void)
{  
   error_t error;
   uint_t i;
   uint8_t   event = 0;
   
   uint32_t value;
   NetInterface *interface;
   OsTask *task;
   MacAddr macAddr;
#if (APP_USE_DHCP == DISABLED)
   Ipv4Addr ipv4Addr;
#endif
#if (APP_USE_SLAAC == DISABLED)
   Ipv6Addr ipv6Addr;  
#endif

   //HAL library initialization
   HAL_Init();
   //Configure the system clock
       
   
    
    
   
   SystemClock_Config();

   //Initialize kernel
   osInitKernel();
   //Configure debug UART
   debugInit(115200);

   //Start-up message
   TRACE_INFO("\r\n");
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("*** IOT BASEBOARD V%d.%02d ***\r\n", IOT_VERSION_H, IOT_VERSION_L);
   TRACE_INFO("***********************************\r\n");
   TRACE_INFO("Copyright: 2010-2018 Oryx Embedded SARL\r\n");
   TRACE_INFO("Compiled: %s %s\r\n", __DATE__, __TIME__);
   TRACE_INFO("Target: STM32F429\r\n");
   TRACE_INFO("\r\n");

   //LED configuration
   BSP_LED_Init(LED1);
   BSP_LED_Init(LED2);
   BSP_LED_Init(LED3);

   //Clear LEDs
   BSP_LED_Off(LED1);
   BSP_LED_Off(LED2);
   BSP_LED_Off(LED3);

   //Initialize user button
   BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

   //Enable RNG peripheral clock
   __HAL_RCC_RNG_CLK_ENABLE();
   //Initialize RNG
   RNG_Handle.Instance = RNG;
   HAL_RNG_Init(&RNG_Handle);

   //Generate a random seed
   for(i = 0; i < 32; i += 4)
   {
      //Get 32-bit random value
      HAL_RNG_GenerateRandomNumber(&RNG_Handle, &value);

      //Copy random value
      seed[i] = value & 0xFF;
      seed[i + 1] = (value >> 8) & 0xFF;
      seed[i + 2] = (value >> 16) & 0xFF;
      seed[i + 3] = (value >> 24) & 0xFF;
   }
   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  MX_GPIO_Init();
  MX_DMA_Init();  
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_UART7_Init();
  MX_RTC_Init();
  MX_IWDG_Init();

  HAL_GPIO_WritePin(NET_RESET_GPIO_Port, NET_RESET_Pin, GPIO_PIN_RESET);       
  
  if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {
      BSP_LED_On(LED3);
      wErrorWatchdogCheckCont = SET;            //s watchdog 확인용
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
  
  lll_eeprom_init();
  load_eeprom();
  
  LCD_init();                   //s lcd init
  

  if(CONFIG_RF_FREQ_ADDR1_9 > 3 ){
    event = 1;
    CONFIG_RF_FREQ_ADDR1_9 = 3;
  }

  if(CONFIG_RF_FREQ_ADDR2_10 >= RF_AUTOMAN_FREQ_GR3_MAX){
    if(CONFIG_RF_FREQ_ADDR1_9 == 0){
      if(CONFIG_RF_FREQ_ADDR2_10 >= RF_AUTOMAN_FREQ_GR0_MAX){
        CONFIG_RF_FREQ_ADDR2_10 = RF_AUTOMAN_FREQ_GR0_MAX;
        event = 1;
      }
    }
    else if(CONFIG_RF_FREQ_ADDR1_9 == 1){
      if(CONFIG_RF_FREQ_ADDR2_10 >= RF_AUTOMAN_FREQ_GR1_MAX){
        CONFIG_RF_FREQ_ADDR2_10 = RF_AUTOMAN_FREQ_GR1_MAX;
        event = 1;
      }
    }    
    else if(CONFIG_RF_FREQ_ADDR1_9 == 2){
      if(CONFIG_RF_FREQ_ADDR2_10 >= RF_AUTOMAN_FREQ_GR2_MAX){
        CONFIG_RF_FREQ_ADDR2_10 = RF_AUTOMAN_FREQ_GR2_MAX;
        event = 1;
      }
    }  
    else if(CONFIG_RF_FREQ_ADDR1_9 == 3){
      if(CONFIG_RF_FREQ_ADDR2_10 >= RF_AUTOMAN_FREQ_GR3_MAX){
        CONFIG_RF_FREQ_ADDR2_10 = RF_AUTOMAN_FREQ_GR3_MAX;
        event = 1;
      }
    }      
  }
  
  if(event){
    save_all_eeprom();
  }
  
  read_eeprom(&lcd_icon);
  
  HAL_UART_Receive_DMA(&huart2, buffLoraRxData, 1); 
  HAL_UART_Receive_DMA(&huart6, buffUartRxData, 1);   
  HAL_UART_Receive_DMA(&huart7, buffLCDRxData, 1);
  
  myFcode = Config.Value[0];
  sprintf(MyTopic, "dawoon/iotCtrl/%d",myFcode);   
  sprintf(MyInfoTopic, "dawoon/iotCtrl/%dH",myFcode);     
  memset(MqttIdData, 0, sizeof(MqttIdData));
  sprintf(MqttIdData, "id:IOTFarm%04d", myFcode);
  TRACE_INFO("MqttIdData : %s\r\n", MqttIdData);
  memset(MacIdData, 0, sizeof(MacIdData));  
  sprintf(MacIdData, "00-A2-C2-E3-%02d-%02d", myFcode / 100, myFcode % 100);
  TRACE_INFO("MacIdData : %s\r\n", MacIdData);
  
  
    if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK)){	//s automan 3024m
            cAutomanSendTimming = 30;
    }
    else if(CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_BEHAP){	//s automan 3024m
            cAutomanSendTimming = 22;
    }
    else if(CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_BEHAP){	//s automan 3007m
            cAutomanSendTimming = 15;
    }    
    else{
            cAutomanSendTimming = 20;
    }
  
    sprintf(buffGLcd1Line, "IOT CTRL Ver%d.%02d", IOT_VERSION_H, IOT_VERSION_L);		
    TM_HD44780_Puts_State(0, 0, buffGLcd1Line);    
    sprintf(buffGLcd2Line, "FARM:%04d DEV:%d", myFcode, CONFIG_RF_TYPE_3);		
    TM_HD44780_Puts_State(0, 1, buffGLcd2Line);        

    
//    strTemp16ChControl[0].cHttpDeviceReadFlag = SET;
//    strTemp16ChControl[0].cDeviceSetting = SET;
    for(uint8_t i = 0; i < 16; i ++){           //s 처음은 자동 셋팅
      memset(strTemp16ChControl[i].c16ChFirstManualFlag, SET, sizeof(strTemp16ChControl[i].c16ChFirstManualFlag));
    }   
    
  HAL_Delay(1000);
  HAL_GPIO_WritePin(NET_RESET_GPIO_Port, NET_RESET_Pin, GPIO_PIN_SET);       
  HAL_Delay(1000);
  
  NxChangeBaud(115200);
  MX_UART7_115200_Init();
  HAL_Delay(1000);
  
  NxLcdPage(1);
  if(lcd_icon.current_page == 3 && lcd_icon.current_page == 4)
    Set_Page16(lcd_icon.iconbuffer[lcd_icon.current_page]);
  else if(lcd_icon.current_page == 2)
  {
    char bufferout[16] = {0,};
    MakeBitText8CH(bufferout, lcd_icon.iconbuffer[lcd_icon.current_page], 16);
    Set_Page8(bufferout);
  }
  
  memset(buffGLcd2Line, 0, sizeof(buffGLcd2Line));
  sprintf(buffGLcd2Line, "AUTO GR:%d CH:%d", CONFIG_RF_FREQ_ADDR1_9, CONFIG_RF_FREQ_ADDR2_10);		
  TM_HD44780_Puts_State(0, 1, buffGLcd2Line);        
  
  __HAL_IWDG_START(&hiwdg);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
   
  if(lcd_icon.current_page == 2)
    NxLcdShowCom(17,0);
  else if(lcd_icon.current_page == 3 && lcd_icon.current_page == 4)
    NxLcdShowCom(29,0);

   //PRNG initialization
   error = yarrowInit(&yarrowContext);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize PRNG!\r\n");
   }

   //Properly seed the PRNG
   error = yarrowSeed(&yarrowContext, seed, sizeof(seed));
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to seed PRNG!\r\n");
   }

   //TCP/IP stack initialization
   error = netInit();
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize TCP/IP stack!\r\n");
   }

   //Configure the first Ethernet interface
   interface = &netInterface[0];

   //Set interface name
   netSetInterfaceName(interface, "eth0");
   //Set host name
   netSetHostname(interface, "MQTTClientDemo");
   //Select the relevant network adapter
   netSetDriver(interface, &stm32f4x9EthDriver);
   netSetPhyDriver(interface, &lan8742PhyDriver);
   //Set host MAC address
   macStringToAddr(MacIdData, &macAddr);
   netSetMacAddr(interface, &macAddr);

   //Initialize network interface
   error = netConfigInterface(interface);
   //Any error to report?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to configure interface %s!\r\n", interface->name);
   }

#if (IPV4_SUPPORT == ENABLED)
#if (APP_USE_DHCP == ENABLED)
   //Get default settings
   dhcpClientGetDefaultSettings(&dhcpClientSettings);
   //Set the network interface to be configured by DHCP
   dhcpClientSettings.interface = interface;
   //Disable rapid commit option
   dhcpClientSettings.rapidCommit = FALSE;

   //DHCP client initialization
   error = dhcpClientInit(&dhcpClientContext, &dhcpClientSettings);
   //Failed to initialize DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize DHCP client!\r\n");
   }

   //Start DHCP client
   error = dhcpClientStart(&dhcpClientContext);
   //Failed to start DHCP client?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start DHCP client!\r\n");
   }
#else
   //Set IPv4 host address
   ipv4StringToAddr(APP_IPV4_HOST_ADDR, &ipv4Addr);
   ipv4SetHostAddr(interface, ipv4Addr);

   //Set subnet mask
   ipv4StringToAddr(APP_IPV4_SUBNET_MASK, &ipv4Addr);
   ipv4SetSubnetMask(interface, ipv4Addr);

   //Set default gateway
   ipv4StringToAddr(APP_IPV4_DEFAULT_GATEWAY, &ipv4Addr);
   ipv4SetDefaultGateway(interface, ipv4Addr);

   //Set primary and secondary DNS servers
   ipv4StringToAddr(APP_IPV4_PRIMARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 0, ipv4Addr);
   ipv4StringToAddr(APP_IPV4_SECONDARY_DNS, &ipv4Addr);
   ipv4SetDnsServer(interface, 1, ipv4Addr);
#endif
#endif

#if (IPV6_SUPPORT == ENABLED)
#if (APP_USE_SLAAC == ENABLED)
   //Get default settings
   slaacGetDefaultSettings(&slaacSettings);
   //Set the network interface to be configured
   slaacSettings.interface = interface;

   //SLAAC initialization
   error = slaacInit(&slaacContext, &slaacSettings);
   //Failed to initialize SLAAC?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to initialize SLAAC!\r\n");
   }

   //Start IPv6 address autoconfiguration process
   error = slaacStart(&slaacContext);
   //Failed to start SLAAC process?
   if(error)
   {
      //Debug message
      TRACE_ERROR("Failed to start SLAAC!\r\n");
   }
#else
   //Set link-local address
   ipv6StringToAddr(APP_IPV6_LINK_LOCAL_ADDR, &ipv6Addr);
   ipv6SetLinkLocalAddr(interface, &ipv6Addr);

   //Set IPv6 prefix
   ipv6StringToAddr(APP_IPV6_PREFIX, &ipv6Addr);
   ipv6SetPrefix(interface, 0, &ipv6Addr, APP_IPV6_PREFIX_LENGTH);

   //Set global address
   ipv6StringToAddr(APP_IPV6_GLOBAL_ADDR, &ipv6Addr);
   ipv6SetGlobalAddr(interface, 0, &ipv6Addr);

   //Set default router
   ipv6StringToAddr(APP_IPV6_ROUTER, &ipv6Addr);
   ipv6SetDefaultRouter(interface, 0, &ipv6Addr);

   //Set primary and secondary DNS servers
   ipv6StringToAddr(APP_IPV6_PRIMARY_DNS, &ipv6Addr);
   ipv6SetDnsServer(interface, 0, &ipv6Addr);
   ipv6StringToAddr(APP_IPV6_SECONDARY_DNS, &ipv6Addr);
   ipv6SetDnsServer(interface, 1, &ipv6Addr);
#endif
#endif

   //Create MQTT test task
   task = osCreateTask("MQTT", mqttSendTask, NULL, 3000, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

   //Create a task to blink the LED
   task = osCreateTask("Blink", blinkTask, NULL, 500, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {
      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }

//Create a task to Http Get
   task = osCreateTask("HTTP_GET", HttpGetTask, NULL, 1000, OS_TASK_PRIORITY_HIGH);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }
   
////Create user task
   task = osCreateTask("User Task", userTask, NULL, 1000, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }   
   
////Create user task
   task = osCreateTask("Uart Task", UartTask, NULL, 1000, OS_TASK_PRIORITY_NORMAL);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }   
      
//Create a task to Http Send
   task = osCreateTask("HTTP_Send", HttpSendTask, NULL, 1000, OS_TASK_PRIORITY_HIGH);
   //Failed to create the task?
   if(task == OS_INVALID_HANDLE)
   {      //Debug message
      TRACE_ERROR("Failed to create task!\r\n");
   }      
   
   
   //Start the execution of tasks
   osStartKernel();

   //This function should never return
   return 0;
}

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
    printf("error_%s[%d]\r\n", file, line);         
    HAL_Delay(1000);    
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
