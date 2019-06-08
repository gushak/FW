#ifndef _DW_LORA_SENSOR_H_
#define _DW_LORA_SENSOR_H_


#include "stm32f4xx_hal.h"   
#include "stm32f4xx_hal_def.h"
#pragma pack(push, 1)
typedef struct LORA_DATE
{
  uint16_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hour;
  uint8_t min;
}lora_date_st;

#pragma pack(pop)

typedef enum {
    LORA_SETTING_START_WAIT,
    LORA_SETTING_WGID1,
    LORA_SETTING_WGID1_WAIT, 
    LORA_SETTING_WGID2,
    LORA_SETTING_WGID2_WAIT,     
    LORA_SETTING_WNTWID,
    LORA_SETTING_WNTWID_WAIT,     
    LORA_SETTING_WFLASH,
    LORA_SETTING_WFLASH_WAIT,
    LORA_SETTING_WRESET,
    LORA_SETTING_WRESET_WAIT,
    LORA_SETTING_END    
}LORA_SETTING_STATE;

void RfLoraRxParser(uint8_t rx_data, uint8_t* buffdata);
void RfloraLoop(void);

extern lora_date_st lora_date[10];
#endif
