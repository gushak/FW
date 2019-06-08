#ifndef _Mbed_MAIN_H_
#define _Mbed_MAIN_H_
#include "stm32f4xx_hal.h"
#include "parson.h"
#include "str.h"
#include <stdlib.h>
#include <string.h>	
#include "rf_modules.h"

/* Private define ------------------------------------------------------------*/

#define LCD_E_Pin GPIO_PIN_7
#define LCD_E_GPIO_Port GPIOE
#define LCD_RS_Pin GPIO_PIN_8
#define LCD_RS_GPIO_Port GPIOE
#define LCD_D4_Pin GPIO_PIN_12
#define LCD_D4_GPIO_Port GPIOE
#define LCD_D5_Pin GPIO_PIN_14
#define LCD_D5_GPIO_Port GPIOE
#define LCD_D6_Pin GPIO_PIN_15
#define LCD_D6_GPIO_Port GPIOE
#define LCD_D7_Pin GPIO_PIN_10
#define LCD_D7_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define SW1_Pin GPIO_PIN_8
#define SW1_GPIO_Port GPIOC
#define SW2_Pin GPIO_PIN_10
#define SW2_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_9
#define SW3_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_11
#define LED1_GPIO_Port GPIOC
#define BUZ_Pin GPIO_PIN_2
#define BUZ_GPIO_Port GPIOD
#define NET_RESET_Pin GPIO_PIN_0
#define NET_RESET_GPIO_Port GPIOE
/* -----------------------------------------------------------------------------*/


#define BUZZER_TIME_COUNT  1

#define GLCD_BUFF_SIZE  17

#define METER_TEST_MQTT_EN  0
#define METER_SETTING_MQTT_EN  1

//s MQTT DATA SEND CHECK STATE
#define MQTT_PUBLISH_NOT_CONNECT 	0
#define MQTT_PUBLISH_DATA_SEND 		1
#define MQTT_PUBLISH_DATA_SENDING 2

#define MAIN_HTTP_MAX_BUFF 101
#define MAIN_MQTT_MAX_BUFF 20

//Constant definitions
#define APP_SERVER_HTTP_PORT 5000
#define APP_REQUEST_URI "/api/V1/Local/MDevice/Info/"
#define APP_LOCALTIME_URI "/api/V1/Local/MDevice/localtime"
#define APP_RMMSETTING_URI "/api/V1/Local/Mdevice/RMMSetting/"
#define APP_INTAKE_URI "api/V1/Local/Mdevice/IntakeState/"

#define APP_SEND_URI "/api/V1/Local/Mdevice"
#define APP_FEED_SEND_URI "/api/V1/Local/MDevice/RMSIntakeFeed/"
#define DAWOON_HTTP_HEADER_FIELDS "Content-Type: application/json\r\nContent-Length: "


#define IOT_VERSION_H 2
#define IOT_VERSION_L 10

#define PACKET_STX1					0xAA
#define PACKET_STX2					0xBB
#define PACKET_STX3025			0xAB
#define PACKET_ETX						0x0D

#define AUTOMAN_SEND_OP_CODE    0x01
#define RF_DEVIEC_TPYE_ASK      100

#define LORA_BUFF_SIZE  128

//#define AUTOMAN_DATA_SIZE			0x13
//#define AUTOMAN_EX_SIZE			0x10
//#define AUTOMAN_INFO_SIZE			0x01


#define RF_DEVICE_MAX					16

//s 사이클론에서 변경됨
#define RF_DATAPOS_FARMCODE1		0	
#define RF_DATAPOS_FARMCODE2		1	
#define RF_DATAPOS_ID				2		
#define RF_DATAPOS_MODE			3	
#define RF_DATAPOS_CMD				4	
#define RF_DATAPOS_RELAYOUTL		5	
#define RF_DATAPOS_RELAYOUTH		6	
#define RF_DATAPOS_RELAYTIMERL	        7	
#define RF_DATAPOS_RELAYTIMERH	        8	
#define RF_DATAPOS_INPUT			9

#define RF_DATAPOS_INPUTTIME1L	        10	
#define RF_DATAPOS_INPUTTIME1H	        11	
#define RF_DATAPOS_INPUTTIME2L	        12	
#define RF_DATAPOS_INPUTTIME2H	        13	
#define RF_DATAPOS_INPUTTIME3L	        14	
#define RF_DATAPOS_INPUTTIME3H	        15	

#define RF_DATAPOS_FLAG                        16

#define RF_DATAPOS_CRCL	                16
#define RF_DATAPOS_CRCH	                17
#define RF_DATAPOS_ETX	                        18

#define RF_CONTROL_FEEDER			  0
//#define RF_CONTROL_MOVEING			1
//#define RF_CONTROL_LIGHT1			  2
//#define RF_CONTROL_LIGHT2 			3
//#define RF_CONTROL_HEATER1			4
//#define RF_CONTROL_HEATER2 			5
//#define RF_CONTROL_HEATER3 			6
#define RF_CONTROL_LIGHT1			  1
#define RF_CONTROL_LIGHT2 			2
#define RF_CONTROL_HEATER1			3
#define RF_CONTROL_HEATER2 			4
#define RF_CONTROL_HEATER3 			5


//s 배합 컨트롤러 추가
#define RF_BAHAP_DATAPOS_ID			0		
#define RF_BAHAP_DATAPOS_CMD		1	
#define RF_BAHAP_DATAPOS_RELAY_POS	2	

#define SUB_TYPE_16CH        1
#define SUB_TYPE_8CH         2
#define SUB_TYPE_PENDING     3
#define SUB_TYPE_SMALL_8CH   4			//s 181022 Aß°¡

#define MQTT_TYPE_16CH      10
#define MQTT_TYPE_8CH       20
#define MQTT_TYPE_PENDING   30
#define MQTT_TYPE_SMALL_8CH 40			//s 181022 Aß°¡

#define MQTT_PANDING_NUM_MOVEING1  1
#define MQTT_PANDING_NUM_MOVEING2  3
#define MQTT_PANDING_NUM_MOVEING3  5
#define MQTT_PANDING_NUM_PULL      7
#define MQTT_PANDING_NUM_PANDING   8

#define SW_PANDING_MOVEING1_ON  0
#define SW_PANDING_MOVEING1_OFF 1
#define SW_PANDING_MOVEING2_ON  4
#define SW_PANDING_MOVEING2_OFF 5
#define SW_PANDING_MOVEING3_ON  8
#define SW_PANDING_MOVEING3_OFF 9
#define SW_PANDING_PULL         12
#define SW_PANDING_PANDING_PULL   14
#define SW_PANDING_PANDING_ANNEAL 15

#define SW_CONTROL_FEEDER			  0
//#define SW_CONTROL_MOVEING			1
#define SW_CONTROL_LIGHT1			  2
#define SW_CONTROL_LIGHT2 			3
#define SW_CONTROL_HEATER1			5
#define SW_CONTROL_HEATER2 			6
#define SW_CONTROL_HEATER3 			7

#define RELAY_PANDING_MOVEING1_ON  0
#define RELAY_PANDING_MOVEING1_OFF 2
#define RELAY_PANDING_MOVEING2_ON  4
#define RELAY_PANDING_MOVEING2_OFF 6
#define RELAY_PANDING_MOVEING3_ON  8
#define RELAY_PANDING_MOVEING3_OFF 10
#define RELAY_PANDING_PULL         12
#define RELAY_PANDING_PANDING_PULL   14
#define RELAY_PANDING_PANDING_ANNEAL 15

#define BIT_PANDING_MOVEING1_ON  	0x0001 << RELAY_PANDING_MOVEING1_ON
#define BIT_PANDING_MOVEING1_OFF 	0x0001 << RELAY_PANDING_MOVEING1_OFF
#define BIT_PANDING_MOVEING2_ON  	0x0001 << RELAY_PANDING_MOVEING2_ON
#define BIT_PANDING_MOVEING2_OFF 	0x0001 << RELAY_PANDING_MOVEING2_OFF
#define BIT_PANDING_MOVEING3_ON  	0x0001 << RELAY_PANDING_MOVEING3_ON
#define BIT_PANDING_MOVEING3_OFF 	0x0001 << RELAY_PANDING_MOVEING3_OFF
#define BIT_PANDING_PULL         	0x0001 << RELAY_PANDING_PULL
#define BIT_PANDING_PANDING_PULL   0x0001 << RELAY_PANDING_PANDING_PULL
#define BIT_PANDING_PANDING_ANNEAL 0x0001 << RELAY_PANDING_PANDING_ANNEAL

//s http sending data of kind
#define HTTP_SEND_PANDING			0 
#define HTTP_SEND_WEIGHT_KHH	1 
#define HTTP_SEND_SENSOR			3 
#define HTTP_SEND_WATER				4 
//define HTTP_SEND_WATER_PRESURE	5 
#define HTTP_SEND_WEIGHT			6 

//s http sending device of kind
#define HTTP_TYPE_AUTOMAN			0 
#define HTTP_TYPE_LORA				1 

//s http get device of kind
#define HTTP_GET_TIME					0 
#define HTTP_GET_DEVICE_SET		                1 
#define HTTP_GET_16CH_DEVICE_SET		2 

#define AUTOMAN_ALIVE_TIMMING 20

//s Device Type Define
#define DEVICE_TYPE_AUTOMAN_3025                 1
#define DEVICE_TYPE_AUTOMAN			2	
#define DEVICE_TYPE_LAZER_WEIGHT 	        3
#define DEVICE_TYPE_AUTOMAN_WATER            4
#define DEVICE_TYPE_LORA_SENSOR                 5
#define DEVICE_TYPE_AUTOMAN_3025_CHICK      6
#define DEVICE_TYPE_AUTOMAN_CHICK		7	
#define DEVICE_TYPE_AUTOMAN_3025_BEHAP     8
#define DEVICE_TYPE_AUTOMAN_BEHAP		9	

#define SERIAL_RF_RX_CLEAR_COUNT  100    //s rx buff clear count 200ms

#define MQTT_EVENT_IN			1
#define MQTT_EVENT_OUT		0

#define MQTT_STATUS_RESPONE			0
#define MQTT_HCOW_STAT					1

#define MQTT_STATUS_RESPONE			0
#define MQTT_HCOW_STAT					1

#define MQTT_IOT_SEND			0
#define MQTT_INFO_SEND			1

#define HTTP_SERVER 0   // 원본은  1세팅  0은 변경 mqtt client 테스트.
#define Watchdog 0

#define nomqtt 0
#define test_rftag 1



#if nomqtt
#define MQTT_RECONNECTING_SET 100
#else
#define MQTT_RECONNECTING_SET 1
#endif

#define DEBUG_HEO 0

#define OBServer        0

#define GTick()   osGetSystemTime()
extern uint8_t MqttConnectOK;
extern uint8_t http_main_flag,http_main_flag2;
extern uint8_t lcd_view_flag;
////s timer 1ms
//typedef enum
//{
//    TIMER_1MS_TASK_IN_MENU = 0,    
//    TIMER_1MS_TASK_CHECK_MODE_CHANGE,
//    TIMER_1MS_TASK_IN_24V_LOAD,
//    TIMER_1MS_TASK_IN_24V,
//    TIMER_1MS_TASK_IN_SIG,   
//
//    TIMER_1MS_TASK_OUT_RELAY_PENDING,    
//    TIMER_1MS_TASK_OUT_RELAY,    
//    TIMER_1MS_TASK_OUT_RELAY_LATCH,
//    TIMER_1MS_TASK_IN_24V_READ,  
//    TIMER_1MS_TASK_IN_ADC_START,
//
//    TIMER_1MS_TASK_IN_ADC,      
//    TIMER_1MS_TASK_IN_MENU2,        
//    TIMER_1MS_TASK_OUT_LCD,
//    TIMER_1MS_TASK_OUT_RF,
//    TIMER_1MS_TASK_OUT_BUZZER,
//    
//    TIMER_1MS_TASK_MAX = 20
//}TIMER_1MS_TASK_STATE;


typedef struct
{
	uint8_t cType;	
	uint8_t cPos;
	float temp;
	float humudity;
	uint16_t Co2;
	int Weitgh;	
	float Ammonia;	
	uint8_t Fire;
	uint8_t Elc;
	uint32_t WaterValue;
	uint32_t WaterMakeValue;        
	uint32_t WaterPresure;	
	uint8_t  cWaterEvent;	
	uint16_t InTimer1;              	// input1 port intime
	uint16_t InTimer2;              	// input2 port intime
	uint16_t InTimer3;              	// input3 port intime      
	uint8_t InTimer1Event;              	// input1 port intime event
	uint8_t InTimer2Event;              	// input2 port intime event
	uint8_t InTimer3Event;              	// input3 port intime event
	uint8_t Event;		
}SensorStruct;


typedef struct{
  uint8_t     NowPos;
  uint8_t     SavePos;	
  SensorStruct HttpData[MAIN_HTTP_MAX_BUFF]; 
}Http5SendDataStruct;


typedef struct{
  uint8_t     flag;
  uint8_t   addr;	
  uint32_t	value;
}AmmoniaSetStruct;

typedef struct{
  uint8_t     flag;
  uint8_t   addr;	
  uint32_t	value;
}TempSetStruct;

typedef struct{
  uint8_t     flag;
  uint8_t   addr;	
  char	buffValue[20];
}SerialSetStruct;


typedef struct
{
	
	uint8_t  cFeedTimeFlag[20];
	uint8_t  cFeedSendFlag;	
	uint8_t cFeedPos;	
	uint8_t cFeedCnt;
	uint16_t wFeedTime[20];
	uint16_t wFeedOnTime;

	uint16_t wLightTime;
	uint16_t wLightBright;
//	bool wMovingTimeFlag;	
//	uint16_t wMovingTime;	
//	uint16_t wMovingOnTime;		

	uint8_t cLightCnt;	
	//bool  cLightOnTimeFlag[2];
	uint16_t wLightOnTime[2];	
	uint8_t  cLightOffTimeFlag[2];
	uint16_t wLightOffTime[2];	
	uint16_t wLightOnOffTime;	
	
	uint8_t cHeaterCnt;		
	uint8_t  cHeaterOnFlag[3];
	int16_t wHeaterOnValue[3];		
	uint8_t  cHeaterOffFlag[3];
	int16_t wHeaterOffValue[3];	
	int16_t wTempData;
	int16_t wHumData;
	
	uint8_t  cDataChangeFlag;
	
}TimeControlStruct;


typedef struct
{
	uint8_t cDeviceSetting;
        uint8_t cHttpDeviceReadFlag;     
        uint8_t cSensorId[16];                
        uint8_t cTempEnalbeFlag[16];             
        uint8_t cHumEnalbeFlag[16];              
	uint8_t c16ChSettingFlag[16];		
	uint8_t c16ChManualFlag[16];        
	uint8_t c16ChFirstManualFlag[16];            
	uint8_t  c16ChOnFlag[16];
	int16_t w16ChTempOnValue[16];	
	int16_t w16ChHumOnValue[16];		
	uint8_t  c16ChOffFlag[16];
	int16_t w16ChTempOffValue[16];	
	int16_t w16ChHumOffValue[16];
	int16_t wTempData;
	int16_t wHumData;        
	
	uint8_t  cDataChangeFlag;
	
}Temp16ChControlStruct;


typedef struct
{
	uint8_t  cFeedSwFlag;	
	uint16_t wFeedManualTime;

	uint8_t  cLight1SwFlag;		
	uint16_t wLight1ManualTime;

	uint8_t  cLight2SwFlag;		
	uint16_t wLight2ManualTime;
	
	uint8_t  cHeater1SwFlag;		
	uint16_t wHeater1ManualTime;

	uint8_t  cHeater2SwFlag;		
	uint16_t wHeater2ManualTime;
	
	uint8_t  cHeater3SwFlag;		
	uint16_t wHeater3ManualTime;
}ChickManualStruct;


typedef enum
{
  MQTT_IOT_NONE,
  MQTT_IOT_INFO_RESPONE,
  MQTT_IOT_CHANGE_RESPONE,
  MQTT_IOT_HCOW_STAT,  		
  MQTT_IOT_STATUS_RESPONE,
  
  MQTT_IOT_CHANGE_RETURN,		
  MQTT_IOT_16CH_AUTO_MANUAL,		        //s 16CH 메뉴얼 수동자동모드
  MQTT_IOT_BEHAP_CHANGE_RESPONE,  
  MQTT_IOT_BEHAP_STATUS_RESPONE,	
  MQTT_IOT_REQ_BEBAP_RESPONE, 
  
  MQTT_IOT_END,
   
}MQTT_IOT_CMD_STATE;   


typedef enum
{
  MQTT_IOT_INFO_NONE,
  MQTT_IOT_INFO_NOTI,	
  MQTT_IOT_INFO_ERROR,
  MQTT_IOT_INFO_SENSOR,
  MQTT_IOT_INFO_WATER,

  MQTT_IOT_INFO_WEIGHT,
  MQTT_IOT_INFO_END,
   
}MQTT_IOT_INFO_CMD_STATE;   

typedef struct{
    uint8_t Flag;
    uint8_t cmd;           //s milk data
    uint16_t wFcode;
    uint8_t cDeviceId;
    uint8_t cDeviceType;
    
    uint16_t Relay;
    uint16_t TxRelay;
    //uint16_t RelayBit;
    uint8_t Input;    
    uint8_t PendingMode;
    uint8_t StatusMode;    
    uint8_t MqttFreqSend;
    uint8_t MqttFanMode[16];    
    uint8_t InOutEvent;		    
    uint8_t InEvent;
    uint8_t RelayEvent;
    uint16_t InTimer1;              	// input1 port intime
    uint16_t InTimer2;              	// input2 port intime
    uint16_t InTimer3;              	// input3 port intime      	
    uint8_t Event;		   

    float temp;
    float humudity;
    uint16_t Co2;
    int Weitgh;	
    float Ammonia;	
    uint8_t Fire;
    uint8_t Elc;
    uint32_t WaterValue;		
    uint32_t WaterPresure;	
    uint8_t  cWaterEvent;	
    
    uint16_t ErrorTemp;
    uint16_t ErrorSensor;
    uint16_t ErrorAutoman;
}Mqtt_Out_struct;


typedef struct{
  uint8_t     NowPos;
  uint8_t     SavePos;	
  Mqtt_Out_struct MqttData[MAIN_MQTT_MAX_BUFF]; 
}MqttSendDataStruct;

extern uint8_t second;

extern uint8_t   mqtt_json_robot_flag;
extern uint8_t ROBOT_ID;
extern uint8_t ROBOT_POS;

extern uint8_t wait_network_flag;
extern uint8_t robot_list;
extern uint8_t backup_device;


extern char buffGLcd1Line[GLCD_BUFF_SIZE];
extern char buffGLcd2Line[GLCD_BUFF_SIZE];
extern unsigned char cReLcdDisplay;

extern Http5SendDataStruct strSen_value;
extern uint8_t cHttpSendCount;

extern uint16_t cHttpWaterAdrr;
extern uint16_t cHttpWeitghAdrr;
extern uint16_t cHttpWatervalue;
extern float fHttpWeitghvalue;

extern uint8_t cHttpSendCount;
extern uint8_t cHttpPendingSendCount;
extern uint8_t cHttpWaterSendCount;
extern uint8_t cHttpWeitghSendCount;

//s buzzer control
extern uint8_t cBuzzerCount;

extern uint8_t mqtt_First_connect;

extern uint8_t first_check;

extern uint16_t wDawoonTime;


extern uint8_t RfDeviceEnableFlag[4];
extern uint8_t FeedDeviceFirstFlag[4];
extern uint8_t cFeederFlagOn;
extern uint8_t cTimeResettingFlagOn;

extern uint8_t cTempChangeFlag[4];
extern uint16_t wTempChangeData[4];
extern uint16_t wTempChangeDataback[4];

extern uint8_t TempDataFirstFlag[4];


extern uint16_t wTempMaintainData[4];
extern uint16_t wTempMaintainDataback[4];

extern uint32_t iWeterValueData[4];
extern uint8_t cWeterFirstInFlag[4];
extern uint8_t cWeterSendFlag[4];
extern uint16_t wWaterCompareBack[4];
extern uint16_t wWeitghCompareBack[8];


extern uint8_t buffLoraIndexCount[RF_DEVICE_MAX];

extern uint8_t cAutoDeviceSendTiming[4];			//s 컨트롤 보드 오토만 순서 적용
extern TimeControlStruct strTimeControl[4];
extern TimeControlStruct strTimeControlcomp[4];

extern ChickManualStruct strManualSwControl[4];

//s rf data
extern uint8_t buffUartRxData[LORA_BUFF_SIZE];
extern uint8_t buffUartRxDataSave[LORA_BUFF_SIZE];
extern uint8_t AutomanData[LORA_BUFF_SIZE];
extern uint8_t cRxdataCount;                           //s rx data count 
extern uint8_t cRxComplteState;
extern RfDataStruct strRfRxData[RF_DEVICE_MAX];
extern RfDataStruct strRfTxData[RF_DEVICE_MAX];
extern uint8_t cRfProcessMode;                           //s rf setting
extern uint8_t cRfSettingState;                           //s rf setting
extern uint16_t cRfSettingDelay;                           //s rf setting
extern uint16_t cRfRetryCount;                           //s rf setting
extern uint16_t cRfFreqRetryCount;                           //s rf setting

extern uint8_t cRfTxDelayState;
extern uint8_t cRfTxDelayCount;
extern uint8_t cRfRXBuffClearCount;
extern uint8_t cRfTxEventSendCount;

extern uint8_t cAutomanSendTimming;
extern uint8_t RfRxEvent; 
extern uint8_t RfRxAliveCount; 

extern uint32_t iAutomanCheckCount;

//s Lora 232 data
extern uint8_t buffLoraRxData[LORA_BUFF_SIZE];
extern uint8_t buffLoraRxDataSave[LORA_BUFF_SIZE];
extern uint8_t buffLoraRxRfData[LORA_BUFF_SIZE];
extern uint8_t cLoraRxdataCount;                           //s rx data count 
extern uint8_t cLoraRxComplteState;
extern uint8_t cLoraProcessMode;                           //s loar setting
extern uint8_t cLoraSettingState;                           //s loar setting
extern uint16_t cLoraSettingDelay;                           //s loar setting

extern uint8_t cLoraTxDelayState;
extern uint8_t cLoraTxDelayCount;
extern uint8_t cLoraRXBuffClearCount;
extern uint8_t cLoraTxEventSendCount;


//s 16ch온습도 컨트롤 파트
extern Temp16ChControlStruct strTemp16ChControl[16];
extern uint8_t cAutoDeviceChSend[16];

//s 에러체크 루틴
extern uint16_t wErrorWatchdogCheckCont;			//s watchdog error eeprom저장 및 mqtt 전송
extern uint16_t wErrorAutomanCheckCont[RF_DEVICE_MAX];
extern uint16_t wErrorTempCheckCont[4];
extern uint16_t wErrorSensorCheckCont[4];


//s ammonia setting value
extern AmmoniaSetStruct strAmmoniaSet;
extern TempSetStruct strTempSet;
extern SerialSetStruct strSerialSet;

//void HttpGetJsonParse(json_value* json, uint8_t cowindex);

error_t httpClientWrite(char* buffCowdata,uint8_t type);
error_t httpClientRead(uint8_t Type, uint8_t id);

void NETWORK_INIT(void);

uint8_t LCD_Init(void);

void  Lprintf(uint16_t xpos,uint16_t ypos,uint8_t * buf);
void LCD_SEND(void);
void LCD_SEND_2(void);

void DawoonMqttSend(uint8_t cmd, uint8_t id, uint8_t mode);
void MakeBitText16CH(char* buff, uint32_t data);
void MakeBitTextIn(char* buff, uint32_t data);
void MakeBitText8CH(char* buff, uint16_t data, uint8_t len);
void MakeBitTextPending(char* buff, uint16_t data, uint8_t len);
void DawoonMqttSendInfo(uint8_t cmd, uint8_t id, uint8_t mode);

void MX_IWDG_Init(void);

extern void read_eeprom(Lcdicon_st* lcd);

extern RTC_TimeTypeDef strMainTime;
extern RTC_DateTypeDef strMainData;   
#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

//void RobotMqttSend(uint8_t cmd, uint8_t id, uint8_t value, uint8_t pos);
//bool HttpClientRead(uint8_t meterid, uint32_t rfid);
//void DbMilkLiveMqttSend(CowMeterStruct* milkdata);
//MbedJSONValue JsonRobotSend(uint16_t fcode, Mqtt_Out_struct* robot)
#endif

