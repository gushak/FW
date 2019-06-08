
#include "main.h"
#include "stm32f4xx_hal.h"
#include "Http_Read_Parser.h"


#include <stdlib.h>
#include <string.h>	

#include "parson.h"
#include "debug.h"
#include "eeprom_4seri.h"

static char_t JsonStrData[20];


uint16_t DawoonTimeParse(char *str)
{
  uint16_t temp;
  
  temp = atoi(str) * 60;
  temp = atoi(&str[3]) + temp;	
  
  return temp;

}

uint8_t HttpGetTimeJsonParse(JSON_Value* json, RTC_DateTypeDef *data, RTC_TimeTypeDef *time)
{
  uint8_t devicecount = 0;

   JSON_Object *root_object;
    JSON_Array *array;
    
   root_object = json_value_get_object(json);

  if(strcmp(json_object_get_name(root_object, 0), "CURTM") != 0){
      printf("JSON_PARSE:Invalid\r\n");
  } 
  else {
    sprintf(JsonStrData, "%s", json_object_get_string(root_object, "CURTM"));

    time->Hours = atoi(&JsonStrData[11]);
    time->Minutes = atoi(&JsonStrData[14]);
    time->Seconds = atoi(&JsonStrData[17]);
     
    data->Year = atoi(JsonStrData) - 2000;
    data->Month = atoi(&JsonStrData[5]);
    data->Date = atoi(&JsonStrData[8]);
    data->WeekDay = RTC_WEEKDAY_MONDAY;
    
    if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_CHICK) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_CHICK) ){
      devicecount = (uint8_t)json_object_get_number(root_object, "DongCnt");
    }
    else if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN) ){
        array = json_object_get_array(root_object, "Dong");
        for(uint8_t i = 0; i < 16; i++){
          strTemp16ChControl[i].cDeviceSetting = (uint8_t)json_array_get_number(array, i);	//s 사용가능여부 확인		
          if(strTemp16ChControl[i].cDeviceSetting == SET){
            strTemp16ChControl[i].cHttpDeviceReadFlag = SET;     
          }
        }	
      
    }
    
  }
  return devicecount;
}


void HttpGetDongParse(JSON_Value* json, uint8_t id)
{
  
  int tempdiff;
  int itemConut;  
  uint8_t saveid = id - 1;  

   JSON_Object *root_object;
    JSON_Array *array;


   root_object = json_value_get_object(json);
  
  if(strcmp(json_object_get_name(root_object, 0), "difTm") != 0){
      printf("JSON_PARSE:Invalid\r\n");
  } 
  else{  
    tempdiff = (int)json_object_get_number(root_object, "difTm");

    wTempChangeData[saveid] = (uint16_t)(100 + tempdiff);	//s 온도 변화 적용
    if( wTempChangeData[saveid] != wTempChangeDataback[saveid]){
      cTempChangeFlag[saveid] = SET;
      wTempChangeDataback[saveid] = wTempChangeData[saveid];
    }			
  
    itemConut = (int)json_object_get_number(root_object, "spdTm");
    strTimeControl[saveid].wLightTime = (uint8_t)(itemConut / 10);    

    itemConut = (int)json_object_get_number(root_object, "white");
    strTimeControl[saveid].wLightBright = (uint8_t)(itemConut / 10);        

    itemConut = (int)json_object_get_number(root_object, "feedCnt");
    strTimeControl[saveid].cFeedCnt = (uint8_t)itemConut;
        
    array = json_object_get_array(root_object, "feedVal");
    for(uint8_t i = 0; i < itemConut; i++){
      sprintf(JsonStrData, "%s", json_array_get_string(array, i));
      strTimeControl[saveid].wFeedTime[i] = DawoonTimeParse(JsonStrData);
      if(strTimeControl[saveid].wFeedTim   e[i] >= wDawoonTime){
              strTimeControl[saveid].cFeedTimeFlag[i] = SET;	//s 현재시간보다 크면 가능 feed 가능시간으로 봄
      }        
//      TRACE_DEBUG("strTimeControl[saveid].wFeedTime [%s] [%d]\r\n",JsonStrData, strTimeControl[saveid].wFeedTime[i]);        
    }

    itemConut = (int)json_object_get_number(root_object, "LightCnt");					
    strTimeControl[saveid].cLightCnt = (uint8_t)itemConut;	  
	
    array = json_object_get_array(root_object, "lightStart");    
    for(uint8_t i = 0; i < itemConut; i++){
      sprintf(JsonStrData, "%s", json_array_get_string(array, i));  
      strTimeControl[saveid].wLightOnTime[i] = DawoonTimeParse(JsonStrData);
      if(strTimeControlcomp[saveid].wLightOnTime[i] != strTimeControl[saveid].wLightOnTime[i]){
              strTimeControl[saveid].cDataChangeFlag |= 1 << (RF_CONTROL_LIGHT1 + i);
      }						
    }

    array = json_object_get_array(root_object, "lightStop");    
    for(uint8_t i = 0; i < itemConut; i++){
      sprintf(JsonStrData, "%s", json_array_get_string(array, i));    
      strTimeControl[saveid].wLightOffTime[i] = DawoonTimeParse(JsonStrData);
      if(strTimeControlcomp[saveid].wLightOffTime[i] != strTimeControl[saveid].wLightOffTime[i]){
              strTimeControl[saveid].cDataChangeFlag |= 1 << (RF_CONTROL_LIGHT1 + i);
      }												
    }					

    itemConut = (int)json_object_get_number(root_object, "HeatCnt");
    strTimeControl[saveid].cHeaterCnt = (uint8_t)itemConut;  
  
    array = json_object_get_array(root_object, "heatStart");    
    for(uint8_t i = 0; i < itemConut; i++){
      strTimeControl[saveid].wHeaterOnValue[i] = (int16_t)json_array_get_number(array, i);	//s 온도 변화 적용						
      if(strTimeControlcomp[saveid].wHeaterOnValue[i] != strTimeControl[saveid].wHeaterOnValue[i]){
              strTimeControl[saveid].cDataChangeFlag |= 1 << (RF_CONTROL_HEATER1 + i);
      }									
    }		
    
    array = json_object_get_array(root_object, "heatStop");     
    for(uint8_t i = 0; i < itemConut; i++){
      strTimeControl[saveid].wHeaterOffValue[i] = (int16_t)json_array_get_number(array, i);;	//s 온도 변화 적용
      if(strTimeControlcomp[saveid].wHeaterOffValue[i] != strTimeControl[saveid].wHeaterOffValue[i]){
              strTimeControl[saveid].cDataChangeFlag |= 1 << (RF_CONTROL_HEATER1 + i);
      }												
    }						
    
    memcpy(&strTimeControlcomp[saveid], &strTimeControl[saveid], sizeof(strTimeControlcomp[saveid]));  
  }
  
}


void HttpGet16ChParse(JSON_Value* json, uint8_t id)
{
  
//  int tempdiff;
//  int itemConut;  
//  uint8_t buffdata[100];
  uint8_t saveid = id - 1;  
  size_t jsoncount = 0;
  uint8_t devicech = 0;
  
   JSON_Object *root_object;
   JSON_Array *array;

//    json_array_get_object(root_object, "heatStart");  
    
    
   array = json_value_get_array(json);
   jsoncount = json_array_get_count(array);

  TRACE_DEBUG("jsoncount [%d]\r\n",jsoncount);      
   memset(strTemp16ChControl[saveid].c16ChSettingFlag, 0, sizeof(strTemp16ChControl[saveid].c16ChSettingFlag));
   for(uint8_t i = 0; i < jsoncount; i++){
     root_object = json_array_get_object(array, i);
//     memcpy(buffdata, json_object_dotget_string( root_object,"DEVCD"), 10);
//     TRACE_DEBUG("DEVCD [%s]\r\n",buffdata);           
//     memcpy(buffdata, json_object_dotget_string( root_object,"OFFHUMID"), 10);
//     TRACE_DEBUG("OFFHUMID [%s]\r\n",buffdata);                
     devicech = (int)json_object_get_number(root_object,"DEVCH") - 1;
     strTemp16ChControl[saveid].c16ChSettingFlag[devicech] = SET;
     if(strTemp16ChControl[saveid].c16ChFirstManualFlag[devicech] == SET){
       strTemp16ChControl[saveid].c16ChFirstManualFlag[devicech] = RESET;
      strTemp16ChControl[saveid].c16ChManualFlag[devicech]  = SET;         
     }
     strTemp16ChControl[saveid].cSensorId[devicech] = (int)json_object_get_number(root_object,"SENCH") - 1;
     strTemp16ChControl[saveid].cTempEnalbeFlag[devicech] = (int)json_object_get_number(root_object,"TEMABLE");
     strTemp16ChControl[saveid].cHumEnalbeFlag[devicech] = (int)json_object_get_number(root_object,"HUMABLE");     
     strTemp16ChControl[saveid].w16ChTempOnValue[devicech] = (int)json_object_get_number(root_object,"ONTMPR");     
     strTemp16ChControl[saveid].w16ChTempOffValue[devicech] = (int)json_object_get_number(root_object,"OFFTMPR");     
     strTemp16ChControl[saveid].w16ChHumOnValue[devicech] = (int)json_object_get_number(root_object,"ONHUMID");     
     strTemp16ChControl[saveid].w16ChHumOffValue[devicech] = (int)json_object_get_number(root_object,"OFFHUMID");
   }

}

