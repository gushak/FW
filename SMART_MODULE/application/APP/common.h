
#ifndef __COMMON_H__
#define __COMMON_H__

#include "mbed.h"
#include "rtos.h"
#include "platform/mbed_rtc_time.h"

#define APP_STACK_SIZE	OS_STACK_SIZE/4

#ifdef __GNUC__
typedef unsigned char	u8;
typedef signed char 	s8;
typedef unsigned short 	u16;
typedef signed short 	s16;
typedef unsigned long 	u32;
typedef unsigned long long 	u64;
typedef signed long 	s32;
typedef signed char 	int8;
typedef signed short 	int16;
typedef signed long 	int32;
typedef signed long long 	int64;
typedef u8				BOOL;
#endif

#ifdef __GNUC__
#define __PACKED_STRUCT_PRE		struct
#define __PACKED_STRUCT_LAST	__attribute__ ((packed))
#else
#define __PACKED_STRUCT_PRE 	__packed struct
#define __PACKED_STRUCT_LAST 	
#endif 	

#define TRUE	true
#define FALSE	false
#define HIGH	true
#define LOW		false
#define ENABLE	true
#define DISABLE false

#define _STX			0x02
#define _ETX			0x03
#define _ACK			0x06
#define _BS				0x08
#define _TAB			0x09
#define _LF				0x0A
#define _CR				0x0D
#define _NAK			0x15
#define _CTRL_Z			0x1A	//SUB, Ctrl+Z
#define _ESC			0x1B
#define _SPACE 			0x20

typedef __PACKED_STRUCT_PRE _DATETIME
{
	u16				year;
	u8				month;
	u8				day;
	u8				hour;
	u8				minute;
	u8				second;
	u8 				reserved;
} __PACKED_STRUCT_LAST DATETIME, *PDATETIME;
#define RTC_DATE_TIME 					DATETIME
#define PRTC_DATE_TIME 					PDATETIME

//typedef u8 IP_ADDR[4];
typedef union _IP_ADDR
{
	u32 ulAddr;
	u8  ucAddr[4];
} IP_ADDR, *PIP_ADDR;

//
// Flag Manipulation Macro
//
#define SET_FLAG(v, b)					(v |= (b))
#define CLR_FLAG(v, b)					(v &= ~(b))
#define CHK_FLAG(v, b)					((v & (b))==(b))
#define MSK_FLAG(v, b)					(v & (b))
#define BIT_FLAG(b)						(1<<(b))

//
// Nibble Manipulation Macro
//
#define LOW_NIBBLE(v) 					((v)&0xf)
#define HIGH_NIBBLE(v) 					(((v)&0xf0)>>4)
#define MEKE_HIGH_NIBBLE(v) 			(((v)&0xf)<<4)
//
// Convert Constant
//
#define KNOTS_TO_KM						(1.852)
#define MILE_TO_KM						(1.609344)

#define DEG2RAD							( 0.0174532925)		//PI/180	[0.01745329251]994329576923690768489
#define RAD2DEG							(57.2957795130)		//180/PI	[57.2957795130]82320876798154814105

//간략계산을 위한 상수
#define LAT_PER_METER					90					//0.00000901082, 미터당 위도 증감
#define LON_PER_METER					112					//0.00001123453, 미터당 경도 증감

//
// Command Handler Map
//
//typedef s16(*PFN_CMD_HANDLER)(u8* pCommand);
//typedef __PACKED_STRUCT_PRE _CMD_MAP
//{
//	u32						ulCommandHash;
//	PFN_CMD_HANDLER			pfnHandler;
//} __PACKED_STRUCT_LAST CMD_MAP;
//
//#define SET_CMD_TABLE(tbl,idx,cmd,func)		tbl[idx].ulCommandHash = STR2HASH(cmd, strlen(cmd)); \
//											tbl[idx].pfnHandler	 = func
//#define RUN_CMD_HANDLER(tbl,idx, param)		tbl[idx].pfnHandler(param)
//#define GET_CMD_HASH(tbl,idx)				tbl[idx].ulCommandHash

typedef __PACKED_STRUCT_PRE _TIMEOUT_DELAYED_T {
	u32					tick;
	u16					wait;	//ms
} __PACKED_STRUCT_LAST TIMEOUT_DELAYED, *PTIMEOUT_DELAYED;

typedef void(*pFUNC32)(u32);

//LIB
#include "LibString.h"
#include "util.h"

//Common
#include "config.h"
#include "context.h"
#include "debug.h"

//BSP/drivers
#include "UWB/UWBSpi.h"

//APP
#include "uwb.h"
#include "lora.h"
#include "led.h"
#if defined(USE_USBSERIAL)
#include "usb_serial.h"
#endif
//#include "pc_if.h"
#include "ext_flash.h"
//#include "usb_disk.h"
#include "main.h"
#include "uru_tool.h"

//main.c

//app_common.c
void JumpToSystemBootloader(void);
u32 getTickCount();
u32 getElapsedTick(u32 ulStartTick);
void changeSysClock(int mode);

//#define __CONNECT_TO_NTRIP_SERVER
//#define __WIZFI_TEST_MODE

#endif	/* __COMMON_H__ */