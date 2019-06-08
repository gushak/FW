#pragma once

#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "rf_modules.h"
#include "eeprom_4seri.h"
#include "debug.h"

#define NX_BUFF_SIZE    100

#define NX_PAGE_LOGO    0
#define NX_PAGE_COWINFO    1
#define NX_PAGE_INFO1      2
#define NX_PAGE_INFO2      3
#define NX_PAGE_SETTING    4

#define CONFIG_FARM_PREFERENCE 3684
#define CONFIG_RF_ADDR 0x01
#define DEVICE_TYPE_8CH 0x02

#define AUTOMAN3025_RF_CONTROL 0x00

#define PAGE0_BASE_ADRESS 0x081C0000
#pragma pack(push,1)
typedef struct PARSE_NX8040
{
  uint8_t depth;
  uint8_t ui;
}PARSE_NX8040_t;

typedef struct PARSERF_DATA 
{
        uint8_t Len;                         // 데이터 총길이 
	uint16_t Fcode;			// 농장코드 
	uint8_t Id;		                //RFID CHIP Num
	uint8_t Mode;			//s 0:16ch/  1::8CH /2:계류식     
	uint8_t Cmd;			//  11 샹태 응답  22 상태 변경  33 수동상태변경      
        uint16_t Relay;                   // relay value
        uint16_t RelayTimer;              // input1 port intime        
        uint8_t Input;                     // imput3~8  check       
        uint16_t InTimer1;              // input1 port intime
        uint16_t InTimer2;              // input2 port intime
        uint16_t InTimer3;              // input3 port intime   
        uint8_t flag;
        uint8_t from;
}Parserf_Data;
#pragma pack(pop)

void NxChangeBaud(uint32_t baud);
void parse_nx8040(uint8_t* buffer, uint16_t index);
void command_data(uint8_t cmd);
void NxLcdPage(uint8_t pagenum);
void NxLcdPageString(char *String);
void NxLcdLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, char* color);
void NxLcdClear(uint32_t val);
void NxLcdText(char* Text, char* String);
void NxLcdText25(char* String);
void NxLcdText26(char* String);
void NxLcdText33(char* String);
void NxLcdText34(char* String);
void NxLcdText27(char* String);
void NxLcdText28(char* String);
void NxLcdText29(char* String);
void NxLcdText30(char* String);
void NxLcdText31(char* String);
void NxLcdText32(char* String);
void NxLcdText33(char* String);
void NxLcdText34(char* String);
void NxLcdText35(char* String);
void NxLcdText36(char* String);
void NxLcdText37(char* String);
void NxLcdText38(char* String);
void NxLcdText39(char* String);
void NxLcdText40(char* String);
void NxLcdText43(char* String);
void NxLcdText44(char* String);
void NxLcdText45(char* String);
void NxLcdText46(char* String);
void NxLcdText47(char* String);
void NxLcdText48(char* String);

void NxLcdColor(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void NxLcdclick(uint8_t cid, uint8_t event);
void NxLcdShowCom(char id, uint8_t on_off);
void NxLcdString(char* String);
void sidewindow(uint8_t button);
void ict_event(uint8_t button);
void ict_event2(uint8_t button);
void send_command(Parserf_Data *data);
void write_eeprom(uint32_t *Data, uint16_t size,uint32_t addr);
void read_eeprom(Lcdicon_st* lcd);
void swap(char* data1, char* data2);
void NxLcdSencond(uint8_t Sec);
extern void write_eeprom(uint32_t *Data, uint16_t size,uint32_t addr);
extern uint8_t Occur_Event;
extern uint32_t local_tick;
extern uint32_t global_tick;
extern uint32_t lcd_tick;
extern uint8_t Occur_Event;
extern uint16_t cLCDRxdataCount;
extern Lcdicon_st lcd_icon;
extern uint8_t event_occur;
extern void NxLcdText25(char* String);
extern void NxLcdText26(char* String);
extern void NxLcdText33(char* String);
extern void NxLcdText34(char* String);
extern void NxLcdText41(char* String);
extern void NxLcdText42(char* String);