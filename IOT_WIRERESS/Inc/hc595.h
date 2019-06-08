#ifndef RELAY_595_H
#define RELAY_595_H

#include "main.h"
#include "stm32f1xx_hal.h"   
#include "stm32f1xx_hal_def.h"


//s uart rx part
typedef enum
{
  HC595_TASK_END,
  HC595_TASK_LATCH_RESET,
  HC595_TASK_CLK_LOW,    
  HC595_TASK_CLK_HIGH, 
  HC595_TASK_MISO_WRITE
}HC_595_STATE;


uint8_t hc595OutTask(uint16_t relayvalue);
#endif

