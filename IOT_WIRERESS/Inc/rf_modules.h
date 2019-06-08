#ifndef _DW_DATA_RF_H_
#define _DW_DATA_RF_H_


#include "stm32f1xx_hal.h"   
#include "stm32f1xx_hal_def.h"
#include "menu.h"
//---------------------------------------------------------------
#define RX_RF_RECV_FAIL		0
#define RX_RF_RECV_OK		1
#define RX_RF_RECV_ING		2
#define RX_RF_RECV_END		3

#define FEEDER_TIMEOUT		0x51

#define RECV_RF_TIMEOUT		2000		// 1,000,000 = 1초 
#define RECV_RF_ID_CHECK		1000				// ID번호의 기계가 유지되고 있는가?


#define SUB_FEEDER		0x80
#define SUB_FIM				0x40
#define SUB_CALF			0x20
#define SUB_PIG				0x10

#define RX_RF_IDLE		0
#define RX_RF_DATA	1
#define RX_RF_NEXT	2
#define RX_RF_WAIT	3

//------------------------------------------------------------------------------
// 패킷구조체 및 명령어
#define BOARD_ID						0x01
#define BRODCAST_ID					0xFF

#define PACKET_STX1					0xAA
#define PACKET_STX2					0xBB
#define PACKET_STX3025			                0xAB
#define PACKET_ETX						0x0D

#define AUTOMAN_SEND_OP_CODE    0x01
//#define AUTOMAN_REG_OP_CODE      0x02

#define RF_DEVIEC_TPYE_ASK                  100

#define AUTOMAN_DATA_SIZE				0x13    
#define AUTOMAN_EX_SIZE				0x10
#define AUTOMAN_INFO_SIZE				0x01

#define RF_DATA_POS_CMD                   4
#define RF_DATA_POS_RELAY                5
#define RF_DATA_POS_RELAYTIME1       7
#define RF_DATA_POS_RELAYTIME2       8
#define RF_DATA_POS_INTIMER1L            10
#define RF_DATA_POS_INTIMER1H            11
#define RF_DATA_POS_INTIMER2L            12
#define RF_DATA_POS_INTIMER2H            13
#define RF_DATA_POS_FLAG                    16

#define RF_DATA_NONE        0
#define RF_MAIN_CMD_ALIVE            0x21
#define RF_MAIN_CMD_REQ              0x01
#define RF_MAIN_CMD_CHANGE           0x02
#define RF_MAIN_CMD_CONTROL         0x03
#define RF_MAIN_CMD_FAN_CONTROL  0x04
#define RF_MAIN_CMD_ACK              0x13
#define RF_MAIN_PENDING_AUTO_START   0x04
#define RF_MAIN_PENDING_AUTO_STOP    0x05
#define RF_MAIN_CMD_LCD 0x06

#define RF_SUB_CMD_ALIVE             0x31
#define RF_SUB_CMD_REQ               0x11
#define RF_SUB_CMD_CHANGE            0x12
#define RF_SUB_CMD_CONTROL            0x13
#define RF_SUB_CMD_FAN_CONTROL            0x14
#define RF_SUB_CMD_INPUT             0x3
#define RF_SUB_PENDING_AUTO_START    0x14
#define RF_SUB_PENDING_AUTO_STOP   	 0x15
#define RF_SUB_CMD_LCD                          0x16

#define RF_AUTOMAN_FREQ_GR0_MAX         20
#define RF_AUTOMAN_FREQ_GR1_MAX         24
#define RF_AUTOMAN_FREQ_GR2_MAX         20
#define RF_AUTOMAN_FREQ_GR3_MAX         10


typedef enum
{
    RXSTATUS_IDLE = 0,		
    RXSTATUS_FARMCODE1,	    
    RXSTATUS_FARMCODE2,	
    RXSTATUS_ID,			
    RXSTATUS_MODE,	
    RXSTATUS_CMD,	     
    RXSTATUS_DATA,	    
    RXSTATUS_CRC1,						
    RXSTATUS_CRC2, 	
    RXSTATUS_ETX	      
}AUTOMAN_READ_STATE;


#define MAX_DATA_SIZE					0xff

#pragma pack(push,1)
typedef struct 
{
	AUTOMAN_READ_STATE status;
	uint16_t fcode;
	uint8_t idx;
	uint8_t mode;
	uint8_t cmd;
	uint8_t size;
	uint8_t data[MAX_DATA_SIZE];
        uint8_t crc_h;
        uint8_t crc_l;        
}strAutomanPacket;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct 
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
}RfDataStruct;
#pragma pack(pop)


typedef enum {
    RF_SETTING_START_WAIT,
    RF_SETTING_FREQ_CHECK,
    RF_SETTING_FREQ_CHECK_WAIT,    
    RF_SETTING_FREQ_SET,
    RF_SETTING_FREQ_SET_WAIT, 
    RF_SETTING_FREQ_CHECK2,
    RF_SETTING_FREQ_CHECK2_WAIT,      
    RF_SETTING_END    
}RF_SETTING_STATE;


unsigned char AsciiExtract(unsigned char* string, unsigned char size);
unsigned char LoraDataCheck(void);
unsigned char LoraTrupRxDataCheck(void);
uint8_t LoraTrdpcDataCheck(uint8_t* buffdata);
void AutomanSetCommand(uint8_t cmd, uint8_t mode);

extern void SendPacket_Rf(RfDataStruct* strdata);
extern uint8_t PorcessPacket_Rf_check(uint8_t rx_data, uint8_t* buffdata);
void PorcessPacket_Rf_Clear(void);
uint16_t PacketCRC16(unsigned char * data, uint16_t size);
void RfAutomanSetProcess(uint8_t mode);
#endif
