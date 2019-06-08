#pragma once
#include "water_intake.h"

#ifndef _LORA_PARSE_H_
#define _LORA_PARSE_H_

extern volatile uint8_t Recv_data_ok;
extern volatile u8 Lora_Status;
extern uint16_t lora_tick;
extern uint8_t init_send;
void Send_ok_data_save(void);
void parse_lora_packet(void);
void Sub_lora_process(void);
void lora_init(void);
uint16_t CheckSum(uint8_t *buffer, uint16_t size);

#pragma pack(push, 1)
typedef struct MMQT_StateMachine
{
    uint16_t dev_id;
    uint16_t rf_id;
    uint8_t hour1;
    uint8_t hour2;
    uint8_t min1;
    uint8_t min2;
    uint8_t sec1;
    uint8_t sec2;
    uint8_t year1;
    uint8_t year2;
    uint8_t mon1;
    uint8_t mon2;
    uint8_t day1;
    uint8_t day2;
}MMQT_StateMachine_t;

typedef struct ERROR_STRUCT
{
	uint16_t dev_id;
	uint16_t status;
	uint16_t rf_id;
}Error_Struct_t;
#pragma pack(pop)

extern Error_Struct_t error_st;
void Send_error_packet(Error_Struct_t *e);

#endif

