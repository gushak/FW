#pragma once

#ifndef _WATER_INTAKE_H_
#define _WATER_INTAKE_H_

#define MRAM_INTAKE_MAX_ADDRESS	200
#define RFID_UNKNOWN 9999
#define COW_WAIT 10000

#include <Lora_parse.h>

extern void Gate_open(uint16_t m_cnt);

typedef struct Water_Intake_Table_Struct WATER_ITAKE_TABLE_STRUCT;
#pragma pack(push,1)
struct Water_Intake_Table_Struct
{
	uint16_t index;				//일련번호 
	uint8_t status;				//급여조 상태		//급이 테이블이 변경되었다면 해당 스테이터스 증가 시켜 변경내용을 전송할수 있도록 합니다. 
	uint16_t rfid_num;		//개체 식별번호 RFID
	uint32_t date;				//저장된 날짜 년월일(변환) 
	uint16_t enter_time;		//입장시간
	uint16_t visit_time;		//체류시간
	uint16_t water_cnt1;		//pcnt1//qt1;				//1사료 섭취량
	uint16_t water_cnt2;		 //pcnt2//qt2;				//2사료 섭취량  
	uint16_t enter_weight;	 //weight//temp//qt3;				//3사료 섭취량
	uint16_t aqt1;					//첨가제1 섭취량
	uint16_t aqt2;					//첨가제2 섭취량
	uint16_t end_weight;			//섭취량 무게 //개체 무게
	uint8_t feeder_num;			//섭취한 급이기 번호
  	uint16_t water_temp;
};

typedef struct WATER_INFO
{
	uint8_t value[10];
	float factor;
}Water_Info_t;
#pragma pack(pop)

extern WATER_ITAKE_TABLE_STRUCT W_Intake;

extern void intake_Run(uint32_t current_time);
extern uint32_t tick;
extern uint16_t rfid;

#endif