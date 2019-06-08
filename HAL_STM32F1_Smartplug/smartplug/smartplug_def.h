#pragma once

/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: modem configuration
*
* History
* Date			Description
* 2018/09/20	file created
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "platform_env.h"
#include "comm_def.h"
#include "comm_uart.h"
#include "comm_sys.h"
#include "comm_dbg.h"
#include "crc16.h"
#include "comm_str.h"
#include "smartplug_struct.h"
#include "KEPCO_struct.h"
#include "NMS_format.h"
#include "V9871_format.h"
#include "smartplug_flash.h"        
#include "comm_relay.h"

#define BOOTUP_DELAY 3300               // dealy time for PLC chip ready

// Milliseconds time between MODEM BOOT packet during boot up
#define NMS_BOOT_REPORT_DURATION        15000
#define NMS_READY_REPORT_DURATION       15000
#define NMS_NUM_READY_RETRY             15

#define NMS_NUM_RUN_RETRY               3

#define NMS_ALIVE_DURATION_SEC (60 * 60) // 3 hours

#define NMS_MULTI_PACKET_TX_DURATION 350
#define RY_MCU_DURATION 1000

typedef struct QUE_RTC RtcQue_t;
extern RtcQue_t g_RtcQue;
typedef struct KEPCO_SmartplugConfig CurConfig_t;
extern CurConfig_t g_config;
typedef struct Rtc_BaseConfig RtcBase_t;
extern RtcBase_t g_RtcConfig;
typedef struct Flash_Parameter FlashInfo_t;
extern FlashInfo_t g_FlashInfo;
typedef struct Relay_Parameter RelayInfo_t;
extern RelayInfo_t g_RelayInfo;

