#ifndef HC165_H
#define HC165_H

#include "main.h"
#include "stm32f1xx_hal.h"   
#include "stm32f1xx_hal_def.h"


#define HC164_INPUT_MAX 24



//s 74HC165 TASK
typedef enum
{
  HC165_TASK_END,
  HC165_TASK_LOAD_RESET,
  HC165_TASK_LOAD_SET,  
  HC165_TASK_CLK_LOW,    
  HC165_TASK_CLK_HIGH,     
  HC165_TASK_MOSI_READ,   
}HC165_TASK_STATE;



void Hc165Task(void);
void SwapDataProcess(uint32_t data32, uint8_t* buffdata);
uint32_t ChangeSw8ChData(uint32_t data);
#endif

