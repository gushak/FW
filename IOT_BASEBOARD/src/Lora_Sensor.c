//#include "hw_control.h"


#include "main.h"
#include "debug.h"
#include "Lora_Sensor.h"
#include <stdlib.h> 
#include <string.h>
#include "usart.h"
#include "eeprom_4seri.h"
#include "lcd_hd44780.h"
#include "rtc.h"


#define WIRELESS_BUFF_SIZE 255


char tx_line2[WIRELESS_BUFF_SIZE];
char rx_line2[WIRELESS_BUFF_SIZE];


char wireless2Rxdata[WIRELESS_BUFF_SIZE];
char wireless2RxdataSave[WIRELESS_BUFF_SIZE];

uint8_t cwireless2RxCount = 0;
uint8_t cRxComp2Count = 0;

lora_date_st lora_date[10];

//s slt lora version General_BS_031_v166a14_TRACK A_7.27
char* TrdpcCmd = "$Trdpc=56:1001:1\r\n";				//s Ư�� RS���� �����͸� ������
char* Tbd2c37Cmd = "$Tbd2c=37:1001\r\n";				//s Ư�� RS���� �����͸� ������
char* Tbd2c120Cmd = "$Tbd2c=120:0\r\n";					//s ��� RS�� ���� �����
char* RparCmd = "$Rpar=\r\n";										//s ��� ������ ǥ����	
char* Tbd1Cmd = "$Tbd1=\r\n";										//s ��� RS���� �����
char* Tbd2Cmd = "$Tbd2=1001\r\n";								//s Ư�� RS���� alive ��Ŷ�� �䱸��
char* WresetCmd = "$Wreset=\r\n";								//s �ζ��� ����
char* Tbd3Cmd = "$Tbd3=42:220:1000:10\r\n";


#define LORA_SENSOR_TASK 				0
#define LORA_WATER_TASK  	1
//#define LORA_WATER_PRESURE_TASK 2
#define LORA_WEIGHT_TASK 				2

uint8_t LoraResetCmd = 0;
//uint8_t LoraResetWait = 10/WAIT_TIME_VLAUE;
uint16_t LoraResetWait = 2;
uint8_t LoraTbd1Cmd = 0;
//uint8_t LoraTbd1Wait = 10/WAIT_TIME_VLAUE;
uint16_t LoraTbd1Wait = 3;
uint8_t LoraTbd2c37Cmd = 0;
uint8_t LoraTbd2c37Wait = 5;
uint8_t LoraDeviceSelect =	LORA_SENSOR_TASK;
uint8_t LoraSensorDeviceCount = 0; //s ȯ�漾���� ���� ȣ��
uint8_t LoraWaterValueDeviceCount = 0; //s �������� ���� ȣ��
uint8_t LoraWaterPresureDeviceCount = 0; //s ���а� ���� ȣ��
uint8_t LoraWeightDeviceCount = 0; //s ��ṫ�԰� ���� ȣ��

enum LORA_CMD_STATE {
    LORA_CMD_START = 0,
    LORA_CMD_RESET = 100,
    LORA_CMD_SEND1 = 200,
    LORA_CMD_RECEIVE1 = 1000,
    LORA_CMD_MAX = 1500		//s 3��
};

uint16_t LoraTaskCount = LORA_CMD_START;
uint8_t Lora2TxSize = 0;
char parse_lora2[15][30];
int parse_lora2_num[15];
uint16_t wLoraDataCount = 0;


uint8_t lora_line_parse(char *Buffer)
{
      char *token_r, *last;		
      int token_cnt = 0;

      char *buf = NULL;	
      buf = Buffer;
      
      while((token_r = strtok_r(buf,",", &last))!= NULL)
      {
          buf = NULL;				
          if(token_cnt == 0) {  }			//s �ι�°���� ���ں��� ���
          else if(token_cnt < 14) { 	//s 15�� ������

                  sprintf((char *)&parse_lora2[token_cnt], "%s", token_r);			
                  parse_lora2_num[token_cnt] = atoi(parse_lora2[token_cnt]);
                  
          }
          if(++token_cnt >= 15) break;
      }
      
      return token_cnt;		
      //printf("\n\r parse end  \n\r");	
}

void RfLoraRxParser(uint8_t rx_data, uint8_t* buffdata)
{
  
    uint8_t savepos = 0;
    uint8_t dot_cnt;
    int32_t valuecheck = 0;      
  
    buffLoraRxRfData[wLoraDataCount++] = rx_data;

    if(rx_data == 0x0a)		//������ ����Ÿ �� ��� (�ٹٲ�)
    {
      if(strstr((const char *)buffLoraRxRfData, "Urup")!=NULL) {	//s Urup:1001:1001:1:2(count):1111(data)
              
        memset(parse_lora2,0, sizeof(parse_lora2));
        memset(parse_lora2_num,0, sizeof(parse_lora2_num));			
        if(mqtt_First_connect == SET){									
          if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_LORA_SENSOR) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN) ||
                  (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK) ){					

                  dot_cnt = lora_line_parse((char*)buffLoraRxRfData);
                  TRACE_DEBUG("buffLoraRxRfData [%s]\r\n", buffLoraRxRfData);                  
                  TRACE_DEBUG("lora_line_parse count [%d]\r\n", dot_cnt);

                  TM_HD44780_Puts_State(0,1, "                ");    					
                  sprintf(buffGLcd2Line, " adrr:%d", parse_lora2_num[1]);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 								

                  if(parse_lora2_num[1] < 16){		//s 1~4 ȯ�� ������.  //s 5~8 ������   //s 9 ~ 16 ��� ����
                                            
                      savepos = strSen_value.SavePos + 1;
                      if(savepos == MAIN_HTTP_MAX_BUFF)		//s �ִ�ġ �Ѿ�� �ʱ�ȭ
                          savepos = 0;
                      
                      if(strSen_value.NowPos == savepos){		//s http ���� ť ����
                        sprintf(buffGLcd2Line, "HTTP BUFF MAX %d", CONFIG_RF_TYPE_3);									
                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 
                        HAL_Delay(3000);	
                                
                        NVIC_SystemReset();		//s http ������ �ƴϹǷ� ����									
                                                                            
                      }
                      else{
                          strSen_value.HttpData[strSen_value.SavePos].cType = HTTP_TYPE_LORA;
                          strSen_value.HttpData[strSen_value.SavePos].cPos = parse_lora2_num[1] - 1;

                          //s display part
                          if(strSen_value.HttpData[strSen_value.SavePos].cPos < 4){

                            valuecheck = parse_lora2_num[1] + parse_lora2_num[2] + parse_lora2_num[3] + parse_lora2_num[4] + parse_lora2_num[5];
                            
                            if(valuecheck == parse_lora2_num[7]){               //s ���� ���� ���ٸ� ��Ȯ�� ���� �����Ͷ� �Ǵ���
                            
                              strSen_value.HttpData[strSen_value.SavePos].temp = (float)(parse_lora2_num[2]	/ 10.0);
                              strSen_value.HttpData[strSen_value.SavePos].humudity = (float)(parse_lora2_num[3]	/ 10.0);							
                              strSen_value.HttpData[strSen_value.SavePos].Co2 = parse_lora2_num[4];										
                              strSen_value.HttpData[strSen_value.SavePos].Ammonia = (float)(parse_lora2_num[5] / 10.0);
                              strSen_value.HttpData[strSen_value.SavePos].Fire = parse_lora2[6][0] - 0x30;	
                              strSen_value.HttpData[strSen_value.SavePos].Elc = parse_lora2[6][1] - 0x30;			

                              HAL_RTC_GetTime(&hrtc, &strMainTime, FORMAT_BIN);
                              HAL_RTC_GetDate(&hrtc, &strMainData, FORMAT_BIN);  

                              lora_date[0].min = strMainTime.Minutes;
                              lora_date[0].hour = strMainTime.Hours;
                              lora_date[0].month = strMainData.Month;
                              lora_date[0].date = strMainData.Date;
                              lora_date[0].year = strMainData.Year;
                                                                                        
                              sprintf(buffGLcd2Line, "A:%3.1f", strSen_value.HttpData[strSen_value.SavePos].Ammonia);									
                                TM_HD44780_Puts_State(9,1, buffGLcd2Line);                                 
                                strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ����
//                                if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN)){
//                                    strTemp16ChControl[parse_lora2_num[1] - 1].wTempData = parse_lora2_num[2];
//                                    strTemp16ChControl[parse_lora2_num[1] - 1].wHumData = parse_lora2_num[3];
//                                    wTempMaintainData[parse_lora2_num[1] - 1] = parse_lora2_num[2];
//                                }
//                                else{
                                  strTimeControl[parse_lora2_num[1] - 1].wTempData = parse_lora2_num[2];
                                  strTimeControl[parse_lora2_num[1] - 1].wHumData = parse_lora2_num[3];
                                  wTempMaintainData[parse_lora2_num[1] - 1] = parse_lora2_num[2];
//                                }
				
                                
                                if(wTempMaintainData[parse_lora2_num[1] - 1] != wTempMaintainDataback[parse_lora2_num[1] - 1]){	//s ���� �����Ͱ� 100�� ���� �߻��ϸ� �µ����� �������� �ν�
                                        wTempMaintainData[parse_lora2_num[1] - 1] = wTempMaintainDataback[parse_lora2_num[1] - 1];
                                        wErrorTempCheckCont[parse_lora2_num[1] - 1] = 0;
                                }
                                else{
                                        wErrorTempCheckCont[parse_lora2_num[1] - 1]++;				
                                        if(wErrorTempCheckCont[parse_lora2_num[1] - 1] > 10000){	//s ���� �÷ο� ����
                                                wErrorTempCheckCont[parse_lora2_num[1] - 1] = 10000;
                                        }											
                                }
                                
                                if(wErrorSensorCheckCont[parse_lora2_num[1] - 1]){
                                        wErrorSensorCheckCont[parse_lora2_num[1] - 1]--;
                                        if(wErrorSensorCheckCont[parse_lora2_num[1] - 1] > 10){
                                                wErrorSensorCheckCont[parse_lora2_num[1] - 1]--;
                                        }
                                }
                                
                                if((wErrorAutomanCheckCont[parse_lora2_num[1] - 1] > 100) || (wErrorTempCheckCont[parse_lora2_num[1] - 1] > 100)){
                                        //publish_message(MyTopic, client, Json_Error_Alarm(myFcode, parse_lora2_num[1] - 1));			
                                        DawoonMqttSendInfo(MQTT_IOT_INFO_ERROR, parse_lora2_num[1] - 1, 0);
                                        strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ������ ����
                                }
                                else{			//s ������ �߻��ϸ� ������ ������ �������� ����. push �˶��� ����
                                        strSen_value.HttpData[strSen_value.SavePos].Event = 1;				//s �����͸� ����			
                                }	
                                
                                if(mqtt_First_connect){		//s mqtt ���ᶧ�� �����														
                                        TempDataFirstFlag[parse_lora2_num[1] - 1] = SET;	
                                }		
                            }
                            else{               //s ������ ���� ������ �����͸� ������ ����
                               strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ������ ���� 
                            }
                                  
                          }
                          else if(strSen_value.HttpData[strSen_value.SavePos].cPos < 8){
                              uint8_t weterpos = strSen_value.HttpData[strSen_value.SavePos].cPos - 4;
                              
                            valuecheck = parse_lora2_num[1] + parse_lora2_num[2] + parse_lora2_num[3] + parse_lora2_num[4];
                            
                            if(valuecheck == parse_lora2_num[6]){               //s ���� ���� ���ٸ� ��Ȯ�� ���� �����Ͷ� �Ǵ���                              
                              
                              strSen_value.HttpData[strSen_value.SavePos].WaterPresure = parse_lora2_num[2];			
                              strSen_value.HttpData[strSen_value.SavePos].WaterValue = parse_lora2_num[3];	//s ��� �������� �׶� ���� �������� �����ؾ���.
                              strSen_value.HttpData[strSen_value.SavePos].cWaterEvent = parse_lora2_num[4];									
                                strSen_value.HttpData[strSen_value.SavePos].Fire = parse_lora2[5][0] - 0x30;	
                              strSen_value.HttpData[strSen_value.SavePos].Elc = parse_lora2[5][1] - 0x30;			
	
                                                                                          
                              strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ������ ����                                    
                              sprintf(buffGLcd2Line, "V:%04d", strSen_value.HttpData[strSen_value.SavePos].WaterValue);	
                              TM_HD44780_Puts_State(9,1, buffGLcd2Line);               
                              if((strSen_value.HttpData[strSen_value.SavePos].cWaterEvent & 0xF0) == 0){                //s ���ʰ��� ������ ���� �����ͷ� �Ǵ���
                                if(strSen_value.HttpData[strSen_value.SavePos].WaterValue < 1000){                      //s 1000���� ũ�� �߸��� ������
                                    //TRACE_DEBUG("strSen_value.HttpData[strSen_value.SavePos].WaterValue [%d],[%d] [%d][%d] \r\n", strSen_value.HttpData[strSen_value.SavePos].WaterValue, wWaterCompareBack[weterpos], weterpos, strSen_value.HttpData[strSen_value.SavePos].cPos);
                                                                    
                                  if(strSen_value.HttpData[strSen_value.SavePos].WaterValue != wWaterCompareBack[weterpos]){
                                    if(cWeterFirstInFlag[weterpos] == SET){       //s ó�� �����ʹ� ������� ����. �� �����͸� ����
                                      cWeterFirstInFlag[weterpos] = RESET;
                                      iWeterValueData[weterpos] = 0;
                                      TRACE_DEBUG("cWeterFirstInFlag\r\n");
                                    }
                                    else{
                                      if(strSen_value.HttpData[strSen_value.SavePos].WaterValue < wWaterCompareBack[weterpos]){           //s ���� �÷ο� �߻�
                                        iWeterValueData[weterpos] +=  1000 + strSen_value.HttpData[strSen_value.SavePos].WaterValue - wWaterCompareBack[weterpos];                                       
                                      }
                                      else{
                                        iWeterValueData[weterpos] += strSen_value.HttpData[strSen_value.SavePos].WaterValue - wWaterCompareBack[weterpos]; 
                                      }
                                    }
                                    TRACE_DEBUG("WaterValue [%d],[%d] , [%d],  [%d]\r\n", iWeterValueData[weterpos], wWaterCompareBack[weterpos], 
                                                strSen_value.HttpData[strSen_value.SavePos].WaterValue, (1000 + strSen_value.HttpData[strSen_value.SavePos].WaterValue - wWaterCompareBack[weterpos]) );
                                    
                   
                                    wWaterCompareBack[weterpos] = strSen_value.HttpData[strSen_value.SavePos].WaterValue;
                                    strSen_value.HttpData[strSen_value.SavePos].WaterMakeValue = iWeterValueData[weterpos];
                                    
                                    
                                    if(cWeterSendFlag[weterpos] == SET){
                                      cWeterSendFlag[weterpos] = RESET;
                                      iWeterValueData[weterpos] = 0;
                                      strSen_value.HttpData[strSen_value.SavePos].Event = 1;				//s �����͸� ����                                    
                                    }
                                  }
                                }
                              }
                            }
                            else{               //s ������ ���� ������ �����͸� ������ ����
                               strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ������ ���� 
                            }                            
                          }	
                          else if(strSen_value.HttpData[strSen_value.SavePos].cPos < 16){                            
                              uint8_t weitghrpos = strSen_value.HttpData[strSen_value.SavePos].cPos - 8;

                            valuecheck = parse_lora2_num[1] + parse_lora2_num[2];
                            
                            if(valuecheck == parse_lora2_num[4]){               //s ���� ���� ���ٸ� ��Ȯ�� ���� �����Ͷ� �Ǵ���                                           
                              
                              strSen_value.HttpData[strSen_value.SavePos].Weitgh = parse_lora2_num[2];										
                              strSen_value.HttpData[strSen_value.SavePos].Fire = parse_lora2[3][0] - 0x30;	
                              strSen_value.HttpData[strSen_value.SavePos].Elc = parse_lora2[3][1] - 0x30;
                                                                                          
                              if(strSen_value.HttpData[strSen_value.SavePos].Weitgh != wWeitghCompareBack[weitghrpos]){
                                wWeitghCompareBack[weitghrpos] = strSen_value.HttpData[strSen_value.SavePos].Weitgh;
                                strSen_value.HttpData[strSen_value.SavePos].Event = 1;				//s �����͸� ����
                              }
                              else{
                                strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ������ ����
                              }
                              sprintf(buffGLcd2Line, "W:%04d", strSen_value.HttpData[strSen_value.SavePos].Weitgh);	
                              TM_HD44780_Puts_State(9,1, buffGLcd2Line);										
                            }
                          }
                          else{               //s ������ ���� ������ �����͸� ������ ����
                              strSen_value.HttpData[strSen_value.SavePos].Event = 0;				//s �����͸� ������ ���� 
                          }  
                          strSen_value.SavePos = savepos;									
                          cHttpSendCount++;												

                      }
                  }
                  memset(buffLoraRxRfData, 0, sizeof(buffLoraRxRfData));                  
                  wLoraDataCount = 0;
            }

          }
          buffLoraIndexCount[parse_lora2_num[1] - 1]++; //s ���Ž� �ε��� ������Ŵ
          memset(buffLoraRxRfData, 0, sizeof(buffLoraRxRfData));                  
          wLoraDataCount = 0;    
      }
      else if(strstr((const char *)buffLoraRxRfData, "Ubup")!=NULL) {		
        if(mqtt_First_connect == SET){								
          if(CONFIG_RF_TYPE_3 == DEVICE_TYPE_LAZER_WEIGHT){
              TM_HD44780_Puts_State(0,1, "                ");    					
              sprintf(buffGLcd2Line, "dst:%d", parse_lora2_num[3]);									
              TM_HD44780_Puts_State(0,1, buffGLcd2Line); 
              fHttpWeitghvalue = (float)parse_lora2_num[3] / 1000.0;				
              cHttpWeitghSendCount++;
          }
        }
      }		
                            
    }			

    
}



void RfloraLoop(void)
{
    if(mqtt_First_connect == SET){
	if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_WATER) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_LORA_SENSOR) ||
							(CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN) ||
						(CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK)){		//s �ش� ����϶���

		if(CONFIG_LORA_WATER_VALUE_DCODE_7 || CONFIG_LORA_WEIGHT_DCODE_8 || CONFIG_LORA_SENSOR_DCODE_6){		//s �����Ұ��� �������� ���� ���
                    cLoraRXBuffClearCount++;

                    if(cLoraProcessMode == 0){
                        //s ó�� ���� ���� �� �ý��� ���¿��� �����
                        uint8_t SendDataSize;        
                        char  SendData[20];     
                        
                    switch(cLoraSettingState){
                      case LORA_SETTING_START_WAIT:
                        if(cLoraSettingDelay > 1500){
                          cLoraSettingState = LORA_SETTING_WGID1;
                          cLoraSettingDelay = 0;
                        }
                        break;
                      case LORA_SETTING_WGID1:
                          memset(SendData, 0 , sizeof(SendData));
                          SendDataSize = sprintf(SendData, "$Wgid=1:29\r\n");                             //s �׷� ���̵� ����              

                          HAL_UART_Transmit(&huart2, (uint8_t*)SendData, SendDataSize, 10000);  	 //s lora RS to reset	    
                          cLoraSettingState = LORA_SETTING_WGID1_WAIT;              
                        TRACE_DEBUG("LORA_SETTING_WGID1\r\n");	                          
                        break;                      
                      case LORA_SETTING_WGID1_WAIT: 
                        if(cLoraSettingDelay > 1500){
                          cLoraSettingState = LORA_SETTING_WGID2;
                          cLoraSettingDelay = 0;
                        }                      
                        break;                      
                      case LORA_SETTING_WGID2:
                          memset(SendData, 0 , sizeof(SendData));
                          SendDataSize = sprintf(SendData, "$Wgid=2:1\r\n");                             //s �׷� ���̵� ����              

                          HAL_UART_Transmit(&huart2, (uint8_t*)SendData, SendDataSize, 10000);  	 //s lora RS to reset       
                          cLoraSettingState = LORA_SETTING_WGID2_WAIT;                  
                        TRACE_DEBUG("LORA_SETTING_WGID2\r\n");	                           
                        break;                      
                      case LORA_SETTING_WGID2_WAIT: 
                        if(cLoraSettingDelay > 1500){
                          cLoraSettingState = LORA_SETTING_WNTWID;
                          cLoraSettingDelay = 0;
                        }                      
                        break;                      
                      case LORA_SETTING_WNTWID:
                          memset(SendData, 0 , sizeof(SendData));
                          SendDataSize = sprintf(SendData, "$Wntwid=%d\r\n", 100);          //s ��Ʈ��ũ ���̵� ����                    

                          HAL_UART_Transmit(&huart2, (uint8_t*)SendData, SendDataSize, 10000);  	 //s lora RS to reset 
                          cLoraSettingState = LORA_SETTING_WNTWID_WAIT;                                              
                        TRACE_DEBUG("LORA_SETTING_WNTWID\r\n");	                              
                        break;                      
                      case LORA_SETTING_WNTWID_WAIT:     
                        if(cLoraSettingDelay > 1500){
                          cLoraSettingState = LORA_SETTING_WFLASH;
                          cLoraSettingDelay = 0;
                        }                      
                        break;                      
                      case LORA_SETTING_WFLASH:
                          memset(SendData, 0 , sizeof(SendData));
                          SendDataSize = sprintf(SendData, "$Wflash= \r\n" );                                     //s �÷����� ����

                          HAL_UART_Transmit(&huart2, (uint8_t*)SendData, SendDataSize, 10000);  	 //s lora RS to reset         
                          cLoraSettingState = LORA_SETTING_WFLASH_WAIT;      
                        TRACE_DEBUG("LORA_SETTING_WFLASH\r\n");                          
                        break;                      
                      case LORA_SETTING_WFLASH_WAIT:
                        if(cLoraSettingDelay > 1500){
                          cLoraSettingState = LORA_SETTING_WRESET;
                          cLoraSettingDelay = 0;
                        }                      
                        break;                      
                      case LORA_SETTING_WRESET:
                          memset(SendData, 0 , sizeof(SendData));
                          SendDataSize = sprintf(SendData, "$Wreset=\r\n");              //s �ζ� ���� ���� 

                          HAL_UART_Transmit(&huart2, (uint8_t*)SendData, SendDataSize, 10000);  	 //s lora RS to reset         
                          cLoraSettingState = LORA_SETTING_WRESET_WAIT;                            
                        TRACE_DEBUG("LORA_SETTING_WRESET\r\n");                                    
                        break;                      
                      case LORA_SETTING_WRESET_WAIT:
                        if(cLoraSettingDelay > 1500){
                          cLoraSettingState = LORA_SETTING_END;
                          cLoraSettingDelay = 0;
                          cLoraProcessMode = SET;
                        }                      
                        break;             
                      default:
                        break;
                    }                      
                      cLoraSettingDelay++;        

                      
                    }
                    else{
                    
                      if(cLoraRxdataCount)
                      {
                         for(uint8_t i = 0; i < cLoraRxdataCount; i++)
                         {
                            RfLoraRxParser(buffLoraRxDataSave[i], buffLoraRxRfData);
                         }
                         cLoraRxdataCount = 0;
                         memset(buffLoraRxDataSave, 0, sizeof(buffLoraRxDataSave));
                         cLoraRXBuffClearCount = 0;
                      }
                      if(cLoraRXBuffClearCount > SERIAL_RF_RX_CLEAR_COUNT)
                      {
                        cLoraRXBuffClearCount = 0;
                        cLoraRxdataCount = 0;
                        memset(buffLoraRxDataSave, 0, sizeof(buffLoraRxDataSave));      
                        wLoraDataCount = 0;
                        //HAL_UART_Receive_IT(&huart2, buffLoraRxData, 1);   
                        
                      }   								
                      
                      switch(LoraTaskCount) {
                        case LORA_CMD_RESET:
                          if(LoraResetCmd == 0) {
                            LoraResetWait--;
                            TRACE_DEBUG("LoraReset[%d]\r\n", LoraResetWait);
                            if(LoraResetWait == 0) {
                                LoraResetCmd = 1;
                                Lora2TxSize = sprintf(tx_line2,"%s",Tbd2c120Cmd);  //s "$Tbd2c=120:0\r\n";

                                HAL_UART_Transmit(&huart2, (uint8_t*)tx_line2, Lora2TxSize, 10000);  	 //s lora RS to reset	                                  
                                TRACE_DEBUG("[L]LoraReset\r\n");
                                wLoraDataCount = 0;
                                TM_HD44780_Puts_State(0,1, "                ");  	
                                TM_HD44780_Puts_State(0,1, " Lora Rs Reset ");    	                                  
                            }

                          }
                          if(LoraTbd1Cmd == 0 && LoraResetCmd == 1) {			//s ������ �Ϸ�Ǿ��� Event-driven���� ������ ���� 
                            if(LoraTbd1Wait){  
                                    LoraTbd1Wait--;
                            }
                            if(mqtt_First_connect){
                              if(LoraTbd1Wait == 0) {
                                LoraTbd1Cmd = 1;				//s �� �������???
                                memset(tx_line2, 0, sizeof(tx_line2));
                                //Lora2TxSize = sprintf(tx_line2,"%s",Tbd3Cmd);    //s "$Tbd3=42:220:1000:10\r\n";
                                Lora2TxSize = sprintf(tx_line2,"%s",Tbd1Cmd);    //s "$Tbd1=\r\n";   ��� RS���� �����;								
                                  HAL_UART_Transmit(&huart2, (uint8_t*)tx_line2, Lora2TxSize, 10000);  	 //s lora BD1-pkt broadcase
                                
                                TRACE_DEBUG("[L]LoraTbd3Cmd\r\n");

                                TM_HD44780_Puts_State(0,1, "                ");  	
                                TM_HD44780_Puts_State(0,1, " Lora Brod Set "); 
                                LoraTbd2c37Wait = 3;						
                              }
                            }

                          }
                          break;

                          case LORA_CMD_SEND1:

                            if(LoraResetCmd == 1 && LoraTbd1Cmd == 1) {
                                            
                              if(LoraTbd2c37Wait == 0) {
                                              LoraTbd2c37Cmd = 1;
                                              memset(tx_line2, 0, sizeof(tx_line2));
                                              //Lora2TxSize = sprintf(tx_line2,"%s",TrdpcCmd); //"$Trdpc=56:1001:1\r\n";
                                      //if(LoraDeviceCount == 0){
                                      
                                if(LoraDeviceSelect == LORA_SENSOR_TASK){
                                  if((strAmmoniaSet.flag == SET) && (strAmmoniaSet.addr == (LoraSensorDeviceCount + 1))){		//s setting data�� ����
                                    Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,%d,%d,0\r\n",LoraSensorDeviceCount+1001, LoraSensorDeviceCount+1001, 
                                            "00000000", buffLoraIndexCount[LoraSensorDeviceCount], 1, strAmmoniaSet.value); //s ���� ������� ��ġ�� ��� �� ȣ����	

                                    TM_HD44780_Puts_State(0,0, "                ");    					
                                    sprintf(buffGLcd1Line, "AMMO SET SEND:%d",strAmmoniaSet.addr);									
                                    TM_HD44780_Puts_State(0,0, buffGLcd1Line);                                   

                                    memset(&strAmmoniaSet, 0, sizeof(strAmmoniaSet));
                                  }
                                  else if((strTempSet.flag == SET) && (strTempSet.addr == (LoraSensorDeviceCount + 1))){
                                    Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,%d,%d,0\r\n",LoraSensorDeviceCount+1001, LoraSensorDeviceCount+1001, 
                                            "00000000", buffLoraIndexCount[LoraSensorDeviceCount], 2, strTempSet.value); //s ���� ������� ��ġ�� ��� �� ȣ����	

                                    TM_HD44780_Puts_State(0,0, "                ");    					
                                    sprintf(buffGLcd1Line, "TEMP SET SEND:%d",strTempSet.addr);									
                                    TM_HD44780_Puts_State(0,0, buffGLcd1Line);
                                      memset(&strTempSet, 0, sizeof(strTempSet));	                    
                                  }
                                  else if(cTempChangeFlag[LoraSensorDeviceCount] == SET){		//s �� �����ͷ� �����
                                      cTempChangeFlag[LoraSensorDeviceCount] = RESET;
                                      Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,%d,%d,0\r\n",LoraSensorDeviceCount+1001, LoraSensorDeviceCount+1001, 
                                              "00000000", buffLoraIndexCount[LoraSensorDeviceCount], 2, wTempChangeData[LoraSensorDeviceCount]); //s ���� ������� ��ġ�� ��� �� ȣ����	

                                      TM_HD44780_Puts_State(0,0, "                ");    					
                                      sprintf(buffGLcd1Line, "TEMP SET SEND:%d",strTempSet.addr);									
                                      TM_HD44780_Puts_State(0,0, buffGLcd1Line);
                                      memset(&strTempSet, 0, sizeof(strTempSet));	                                                                                                                                                
                                  }												
                                  else{
                                      Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,0,0\r\n",LoraSensorDeviceCount+1001, LoraSensorDeviceCount+1001, 
                                              "00000000", buffLoraIndexCount[LoraSensorDeviceCount]); //s ���� ������� ��ġ�� ��� �� ȣ����	
                                      TM_HD44780_Puts_State(0,0, "                ");    					
                                      sprintf(buffGLcd1Line, "SENSOR PULL:%d",LoraSensorDeviceCount+1);									
                                      TM_HD44780_Puts_State(0,0, buffGLcd1Line);													
                                  }
                                  wErrorSensorCheckCont[LoraSensorDeviceCount]++;	
                                  TRACE_DEBUG("wErrorSensorCheckCont[%d] : [%d]\r\n", LoraSensorDeviceCount, wErrorSensorCheckCont[LoraSensorDeviceCount]);
                                  if(wErrorSensorCheckCont[LoraSensorDeviceCount] > 100){
                                          if(wErrorSensorCheckCont[LoraSensorDeviceCount] > 10000){
                                             wErrorSensorCheckCont[LoraSensorDeviceCount] = 10000;
                                          }
                                          //publish_message(MyTopic, client, Json_Error_Alarm(myFcode, LoraSensorDeviceCount));		//s error mqtt�� �߻�
                                          DawoonMqttSendInfo(MQTT_IOT_INFO_ERROR, LoraSensorDeviceCount, 0);
                                  }
                                  
                                }
                                else if(LoraDeviceSelect == LORA_WATER_TASK){
                                  if((strSerialSet.flag == SET) && (strSerialSet.addr == (LoraWaterValueDeviceCount + 5))){		//s setting data�� ����
                                      Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,%d,%s,0\r\n",LoraWaterValueDeviceCount+1005, LoraWaterValueDeviceCount+1005, 
                                                  "00000000", buffLoraIndexCount[LoraWaterValueDeviceCount+4], 3, strSerialSet.buffValue); //s ���� ������� ��ġ�� ��� �� ȣ����	

                                      TM_HD44780_Puts_State(0,0, "                ");    					
                                      sprintf(buffGLcd1Line, "SERI SET SEND:%d",strSerialSet.addr);									
                                      TM_HD44780_Puts_State(0,0, buffGLcd1Line);	
                                      memset(&strSerialSet, 0, sizeof(strSerialSet));
                                  }
                                  else{
                                      Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,0\r\n",LoraWaterValueDeviceCount+1005, LoraWaterValueDeviceCount+1005, 
                                            "00000000", buffLoraIndexCount[LoraWaterValueDeviceCount+4]); //s ���� ������� ��ġ�� ��� �� ȣ����	
                                      TM_HD44780_Puts_State(0,0, "                ");    					
                                      sprintf(buffGLcd1Line, "WATER PULL:%d",LoraWaterValueDeviceCount+1);									
                                      TM_HD44780_Puts_State(0,0, buffGLcd1Line);
                                      memset(&strSerialSet, 0, sizeof(strSerialSet));												
                                  }
                                }										
                                else if(LoraDeviceSelect == LORA_WEIGHT_TASK){
                                    Lora2TxSize = sprintf(tx_line2,"$Trdpc=56:%d:1,%d,%s,%d,0\r\n",LoraWeightDeviceCount+1009, LoraWeightDeviceCount+1009, 
                                            "00000000", buffLoraIndexCount[LoraWeightDeviceCount+8]); //s ���� ������� ��ġ�� ��� �� ȣ����	
                                      TM_HD44780_Puts_State(0,0, "                ");    					
                                      sprintf(buffGLcd1Line, "WEIGHT PULL:%d",LoraWeightDeviceCount+1);									
                                      TM_HD44780_Puts_State(0,0, buffGLcd1Line);
                                      memset(&strSerialSet, 0, sizeof(strSerialSet));			
                                }											
                                TRACE_DEBUG("Lora:%s\r\n", tx_line2);
                                HAL_UART_Transmit(&huart2, (uint8_t*)tx_line2, Lora2TxSize, 10000);  	 
                                                                                          
                                if(LoraDeviceSelect == LORA_SENSOR_TASK){
                                    LoraSensorDeviceCount++;											
                                    if(LoraSensorDeviceCount >= CONFIG_LORA_SENSOR_DCODE_6){
                                        if(CONFIG_LORA_WATER_VALUE_DCODE_7 != 0){
                                                LoraDeviceSelect = LORA_WATER_TASK;
                                        }
                                        else if(CONFIG_LORA_WEIGHT_DCODE_8 != 0){
                                                LoraDeviceSelect = LORA_WEIGHT_TASK;
                                        }
                                        else{		//s sensor �Ѱ��ۿ� ������� �ʴ� ��� ��� ���� �ٽ���
                                                LoraWeightDeviceCount= 0;		//s ó������ �ּҰ����� �ٽ� �б� ������
                                                LoraTbd1Cmd = 0;			//s ��� ���� �ٽ� ������
                                                LoraTbd1Wait = 2;													
                                        }
                                        LoraSensorDeviceCount= 0;		//s ó������ �ּҰ����� �ٽ� �б� ������
                                    }
                                }
                                else if(LoraDeviceSelect == LORA_WATER_TASK){
                                    LoraWaterValueDeviceCount++;												
                                    if(LoraWaterValueDeviceCount >= CONFIG_LORA_WATER_VALUE_DCODE_7){
                                        if(CONFIG_LORA_WEIGHT_DCODE_8 != 0){
                                                LoraDeviceSelect = LORA_WEIGHT_TASK;
                                        }
                                        else if(CONFIG_LORA_SENSOR_DCODE_6 != 0){
                                                LoraDeviceSelect = LORA_SENSOR_TASK;
                                                LoraWeightDeviceCount= 0;		//s ó������ �ּҰ����� �ٽ� �б� ������
                                                LoraTbd1Cmd = 0;			//s ��� ���� �ٽ� ������
                                                LoraTbd1Wait = 2;								                                              
                                        }
                                        else{		//s water �Ѱ��ۿ� ������� �ʴ� ��� ��� ���� �ٽ���
                                                LoraWeightDeviceCount= 0;		//s ó������ �ּҰ����� �ٽ� �б� ������
                                                LoraTbd1Cmd = 0;			//s ��� ���� �ٽ� ������
                                                LoraTbd1Wait = 2;													
                                        }												
                                        LoraWaterValueDeviceCount= 0;		//s ó������ �ּҰ����� �ٽ� �б� ������
                                        //LoraTbd1Cmd = 0;			//s ��� ���� �ٽ� ������
                                    }
                                }		
                                else if(LoraDeviceSelect == LORA_WEIGHT_TASK){
                                    LoraWeightDeviceCount++;												
                                    if(LoraWeightDeviceCount >= CONFIG_LORA_WEIGHT_DCODE_8){
                                        if(CONFIG_LORA_SENSOR_DCODE_6 != 0){
                                                LoraDeviceSelect = LORA_SENSOR_TASK;
                                        }
                                        else if(CONFIG_LORA_WATER_VALUE_DCODE_7 != 0){
                                                LoraDeviceSelect = LORA_WATER_TASK;
                                        }
                                        LoraWeightDeviceCount= 0;		//s ó������ �ּҰ����� �ٽ� �б� ������
                                        LoraTbd1Cmd = 0;			//s ��� ���� �ٽ� ������
                                        LoraTbd1Wait = 2;
                                    }
                                }
                                        LoraTbd2c37Wait = 3;			//s 10�� ���� �ҿ��
                            } 
                            else {
                                    if(LoraTbd2c37Wait){
                                            LoraTbd2c37Wait--;
                                    }
                            }
                                  
                          }
                          break;

                        default:
                            break;

            }

                  
            LoraTaskCount++;

            if(LoraTaskCount >= LORA_CMD_MAX)
              LoraTaskCount = LORA_CMD_START;                
          }
        }
        


      }		//s �ζ� ���� ���� ���� Ÿ��ũ ����		
    }
}




