/*

File : hc165.c
Create : 181007

Author: skkim

*/
#include "hc165.h"
#include "main.h"
#include "menu.h"
/* Private */


HC165_TASK_STATE eHc165TaskState;
uint8_t cHc165Position;
uint32_t iHc165DataValue;
uint8_t cHc165ReadFlag;
uint8_t cHc165ReadFirstFlag;
uint16_t wHc165TaskCount;
uint32_t iHc165DataValueBack;




void Hc165Task(void)
{
  
//    uint32_t datatemp32 = 0;  
  
  switch(eHc165TaskState){
  case HC165_TASK_END:
    if(cHc165ReadFlag == SET){
      cHc165ReadFlag = RESET;
      iHc165DataValue = 0;
      eHc165TaskState = HC165_TASK_LOAD_RESET;
      cHc165ReadFirstFlag = SET;        //s 초창기 무조건 리드 데이터 발생함
    }
    break;
  case HC165_TASK_LOAD_RESET:    
      HAL_GPIO_WritePin(IN_LOAD_GPIO_Port, IN_LOAD_Pin, GPIO_PIN_RESET);    
      eHc165TaskState = HC165_TASK_LOAD_SET;      
    break;    
  case HC165_TASK_LOAD_SET:    
      HAL_GPIO_WritePin(IN_LOAD_GPIO_Port, IN_LOAD_Pin, GPIO_PIN_SET);    
      eHc165TaskState = HC165_TASK_CLK_LOW;      
    break;        
  case HC165_TASK_CLK_LOW:    
      HAL_GPIO_WritePin(IN_CLK_GPIO_Port, IN_CLK_Pin, GPIO_PIN_RESET);    
      eHc165TaskState = HC165_TASK_CLK_HIGH;      
      if(cHc165ReadFirstFlag){
        cHc165ReadFirstFlag = RESET;
        iHc165DataValueBack = HAL_GPIO_ReadPin(IN_MISO_GPIO_Port, IN_MISO_Pin);
        iHc165DataValue = iHc165DataValueBack << cHc165Position;
        cHc165Position++;
      }
    break;     
  case HC165_TASK_CLK_HIGH:    
      HAL_GPIO_WritePin(IN_CLK_GPIO_Port, IN_CLK_Pin, GPIO_PIN_SET);    
      eHc165TaskState = HC165_TASK_MOSI_READ;      
    break;     
  case HC165_TASK_MOSI_READ:    
      iHc165DataValueBack = HAL_GPIO_ReadPin(IN_MISO_GPIO_Port, IN_MISO_Pin);   
      iHc165DataValue |= iHc165DataValueBack << cHc165Position;
//      if(cHc165Position < HC164_INPUT_MAX){             //s 읽는 숫자보다 작으면 CLK LOW 돌아감
      if(cHc165Position < 24){             //s 읽는 숫자보다 작으면 CLK LOW 돌아감
        eHc165TaskState = HC165_TASK_CLK_LOW;
        cHc165Position++;
      }
      else{
        HAL_GPIO_WritePin(IN_CLK_GPIO_Port, IN_CLK_Pin, GPIO_PIN_RESET);          
        cHc165Position = 0;
        //datatemp32 = iHc165DataValue & 0x00FFFFFF;      
        //iHc165DataValue = ChangeSw8ChData(datatemp32);
        iHc165DataValue = iHc165DataValue & 0x00FFFFFF; 
        if(iHc165DataValueResult != iHc165DataValue){
          iHc165DataValueResult = iHc165DataValue;
          SwapDataProcess(iHc165DataValueResult, buffSpiReadData);  
          cHc165ProcessFlag = SET;
        }
        
        eHc165TaskState = HC165_TASK_END;          
      }
    break;       
 
    default:
      break;      
  }
  wHc165TaskCount++;
      
  if(wHc165TaskCount >= 200){
    wHc165TaskCount = 0;
    cHc165ReadFlag = SET;
  }
  
  
}


//uint32_t ChangeSw8ChData(uint32_t data)
//{
//    uint32_t RelayBack = 0;
//    uint8_t position = 0;
//  
//    if(CONFIG_8CH_REVERSE && (CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH)){
//      for(uint8_t i = 0; i< 24; i++){
//        position = i % 3;
//        if(position == 1){
//          if(data & (1 << i)){
//            RelayBack |= (0x01 << (i+1));
//          }
//        }
//        else if(position == 2){
//          if(data & (1 << i)){
//            RelayBack |= (0x01 << (i-1));
//          }            
//        }
//        else{
//          if(data & (1 << i)){
//            RelayBack |= (0x01 << (i));
//          }            
//        }
//      }
//    }
//    else{
//      RelayBack = data;
//    }  
//    
//    return RelayBack;
//  
//}


void SwapDataProcess(uint32_t data32, uint8_t* buffdata)
{
    uint8_t i, k;
    uint8_t buffback[3];
//    uint8_t buffback2[3];
//    uint8_t position = 0;    
    
  
    buffback[0] = (uint8_t)data32 & 0x000000FF;
    buffback[1] = (uint8_t)((data32 & 0x0000FF00) >> 8);
    buffback[2] = (uint8_t)((data32 & 0x00FF0000) >> 16); 
          

    for(k = 0; k< 3; k++){    
     buffdata[k] = 0;     
      for(i = 0; i< 8; i++){
        if(buffback[k] & (0x01 << i)){
           buffdata[k] |= (0x01 << (7 - i));
        }
      }
    } 

//    if(CONFIG_8CH_REVERSE && (CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH)){
//      for(k = 0; k< 3; k++){    
//       buffdata[k] = 0; 
//      
//        for(i = 0; i< 8; i++){
//          position = i % 3;
//          
//          if(position == 2){
//            if(buffback2[k] & (0x01 << i)){
//               buffdata[k] |= (0x01 << (i - 2));
//            }            
//          }
//          else if(position == 1){
//            if(buffback2[k] & (0x01 << i)){
//               buffdata[k] |= (0x01 << (i));
//            }                
//          }
//          else if(position == 0){
//            if(buffback2[k] & (0x01 << i)){
//               buffdata[k] |= (0x01 <<  (i + 2));
//            }                
//          }
//
//        }
//      }       
//    }    
//    else{
//      buffdata[0] = buffback2[0];
//      buffdata[1] = buffback2[1];
//      buffdata[2] = buffback2[2];      
//    }    
    
    
    //printf("buffdata[%x][%x][%x] \r\n",buffdata[0],buffdata[1], buffdata[2]);   
    
}

