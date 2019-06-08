#ifndef __LED_H__
#define __LED_H__
#include "rtos.h"

class LED
{
public:
enum {
	eLED_OPR_TYPE_TRG,
	eLED_OPR_TYPE_PRD,
	eLED_OPR_TYPE_CUST,
	eLED_OPR_TYPE_COUNT
};

enum {
	eLED_OPR_RPT_IDLE,
	eLED_OPR_RPT_TRG,		// initiated to set the value of state variable in the value of delay variable
	eLED_OPR_RPT_TRG_TOGG,	// toggle from base state like HIGH -> LOW -> HIGH or LOW -> HIGH -> LOW
	eLED_OPR_RPT_REPEAT,
	eLED_OPR_RPT_COUNT
};

// It LED works high active, Please change the order between eLED_OPR_STATE_ON and eLED_OPR_STATE_OFF
enum {
	eLED_OPR_STATE_ON,
	eLED_OPR_STATE_OFF,
	eLED_OPR_STATE_COUNT
};

// GPIO Pin Definition Struction for Named GPIO
	typedef struct _LED_OPR_PATT_DEF
{
	DigitalOut*	led;
		uint8_t		action;		// 0 : idle, 1 : triggered, 2 : triggered toggle, 3 : repeat
		uint8_t		nstate;		// next state 0 : off, 1 : on
		uint8_t		delay;		// time delay before setting to the next state. (100ms unit)
		uint32_t	utick;		// timer ticks stored for comparison
	void*	pattern;	// time pattern for on/off/....
		uint8_t		patt_len;	// the length of periodic pattern elements
		uint8_t		patt_idx;	//
	} LED_OPR_PATT_DEF;


enum
{
	eLED_GPS_BLU,
	eLED_GPS_RED,
	eLED_GPS_GRN,
	eLED_ODIN_GRN,
	eLED_ODIN_RED,
	eLED_ODIN_BLU,
	eLED_UWB_GRN,
	eLED_UWB_RED,
	eLED_UWB_BLU
};
#define LED_GPS_YLW ((u32)1<<eLED_GPS_RED | 1<<eLED_GPS_GRN)
#define LED_ODIN_YLW ((u32)1<<eLED_ODIN_RED | 1<<eLED_ODIN_GRN)
#define LED_UWB_YLW ((u32)1<<eLED_UWB_RED | 1<<eLED_UWB_GRN)
#define LED_GPS_WHT ((u32)1<<eLED_GPS_BLU | 1<<eLED_GPS_RED | 1<<eLED_GPS_GRN)
#define LED_ODIN_WHT ((u32)1<<eLED_ODIN_BLU | 1<<eLED_ODIN_RED | 1<<eLED_ODIN_GRN)
#define LED_UWB_WHT ((u32)1<<eLED_UWB_BLU | 1<<eLED_UWB_RED | 1<<eLED_UWB_GRN)
#define LED_ALL_RED	((u32)1<<eLED_GPS_RED | 1<<eLED_ODIN_RED | 1<<eLED_UWB_RED)
#define LED_ALL_GRN	((u32)1<<eLED_GPS_GRN | 1<<eLED_ODIN_GRN | 1<<eLED_UWB_GRN)
#define LED_ALL_BLU	((u32)1<<eLED_GPS_BLU | 1<<eLED_ODIN_BLU | 1<<eLED_UWB_BLU)
#define LED_ALL		(LED_ALL_GPS | LED_ALL_ODIN | LED_ALL_UWB)

	LED();
	virtual ~LED();

	void SetPatt(u8 ledEnum, u8 action, u8 nstate, u8 delay, u8* pattSeq, u8 pattLen);
	void SetGrpPatt(u32 ledMsk, u8 action, u8 nstate, u8 delay, u8* pattSeq, u8 pattLen);
	
	bool bKeepRunning;
private:
	Thread led_thread;
};
extern LED gLed;
#endif	/* __LED_H__ */