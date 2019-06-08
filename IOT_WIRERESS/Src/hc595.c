/*

File : hc595.c
Create : 181007

Author: skkim

*/
#include "hc595.h"
#include "main.h"
#include "sw_panel.h"

/* Private */
HC_595_STATE eHc595TaskState;
uint8_t cHc595Position;
uint16_t wHc595DataValue;
uint8_t cHc595WriteFlag;
uint8_t cHc595ReadFirstFlag;
uint16_t wHc595TaskCount = 100;
uint32_t iHc595DataValueBack;

uint8_t hc595OutTask(uint16_t relayvalue)
{
  uint8_t readevent = 0;


  switch(eHc595TaskState){
  case HC595_TASK_END:
    if(cHc595WriteFlag == SET){
      cHc595WriteFlag = RESET;
      cHc595Position = 0;
      wHc595DataValue = relayvalue;
      eHc595TaskState = HC595_TASK_LATCH_RESET;
      cHc595ReadFirstFlag = SET;        //s 초창기 무조건 리드 데이터 발생함
    }
    break;
  case HC595_TASK_LATCH_RESET:    
      HAL_GPIO_WritePin(OUT_LATCH_GPIO_Port, OUT_LATCH_Pin, GPIO_PIN_RESET);  
      eHc595TaskState = HC595_TASK_CLK_LOW;      
    break;     
     case HC595_TASK_CLK_LOW:    
      HAL_GPIO_WritePin(OUT_CLK_GPIO_Port, OUT_CLK_Pin, GPIO_PIN_RESET);   
      eHc595TaskState = HC595_TASK_CLK_HIGH;      
      if(cHc595ReadFirstFlag){
        cHc595ReadFirstFlag = RESET;
        if(wHc595DataValue & 0x0001) {
          HAL_GPIO_WritePin(OUT_MOSI_GPIO_Port, OUT_MOSI_Pin, GPIO_PIN_RESET);  
        }
        else{
          HAL_GPIO_WritePin(OUT_MOSI_GPIO_Port, OUT_MOSI_Pin, GPIO_PIN_SET);            
        }
//        iHc165DataValueBack = HAL_GPIO_ReadPin(IN_MISO_GPIO_Port, IN_MISO_Pin);
//        iHc165DataValue = wHc595DataValue << cHc595Position;
        cHc595Position++;
      }
    break;    
  case HC595_TASK_CLK_HIGH:    
      HAL_GPIO_WritePin(OUT_CLK_GPIO_Port, OUT_CLK_Pin, GPIO_PIN_SET);    
      eHc595TaskState = HC595_TASK_MISO_WRITE;      
    break;        
  case HC595_TASK_MISO_WRITE:    
      if(wHc595DataValue & (0x0001 << cHc595Position)){  
        HAL_GPIO_WritePin(OUT_MOSI_GPIO_Port, OUT_MOSI_Pin, GPIO_PIN_RESET);  
      }
      else{
        HAL_GPIO_WritePin(OUT_MOSI_GPIO_Port, OUT_MOSI_Pin, GPIO_PIN_SET);            
      }
//      iHc165DataValueBack = HAL_GPIO_ReadPin(IN_MISO_GPIO_Port, IN_MISO_Pin);   
//      iHc165DataValue |= iHc165DataValueBack << cHc165Position;
      if(cHc595Position < 16){             //s 읽는 숫자보다 작으면 CLK LOW 돌아감
        eHc595TaskState = HC595_TASK_CLK_LOW;
        cHc595Position++;
      }
      else{
        HAL_GPIO_WritePin(OUT_CLK_GPIO_Port, OUT_CLK_Pin, GPIO_PIN_RESET);      
        HAL_GPIO_WritePin(OUT_LATCH_GPIO_Port, OUT_LATCH_Pin, GPIO_PIN_SET);          
        cHc595Position = 0;
      
//        iHc165DataValue = iHc165DataValue & 0x00FFFFFF;        
//        if(iHc165DataValueResult != iHc165DataValue){
//          iHc165DataValueResult = iHc165DataValue;
//          SwapDataProcess(iHc165DataValueResult, buffSpiReadData);  
//          cHc165ProcessFlag = SET;
//        }
        
        eHc595TaskState = HC595_TASK_END;          
      }
    break;       

    default:
      eHc595TaskState = HC595_TASK_END;      
      break;      
  }
  wHc595TaskCount++;
      
  if(wHc595TaskCount >= 200){
    wHc595TaskCount = 0;
    cHc595WriteFlag = SET;
    readevent = 1;
  }
    
  return readevent;
  
}
