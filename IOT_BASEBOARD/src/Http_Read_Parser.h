#ifndef _HTTP_READ_PARSER_H
#define _HTTP_READ_PARSER_H


//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif
     

#include "main.h"
#include "stm32f4xx_hal.h"
#include "parson.h"

extern uint16_t RF_ID_CAN;

extern uint8_t HTTPReadFlag;
extern uint8_t wash_http_step;

uint16_t DawoonTimeParse(char *str);     
uint8_t HttpGetTimeJsonParse(JSON_Value* json, RTC_DateTypeDef *data, RTC_TimeTypeDef *time);
void HttpGetDongParse(JSON_Value* json, uint8_t id);
void HttpGet16ChParse(JSON_Value* json, uint8_t id);


//C++ guard
#ifdef __cplusplus
   }
#endif


#endif //s DAWOON_MILK_METER_H

