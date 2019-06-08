#pragma once

#include "comm_def.h"

#define CFG_DEF_MTU             72

#define CFG_DEF_SYS_MODE        0x00

#define CFG_DEF_ALIVE_PERIOD	0xFF

#define CFG_MIN_ALIVE_PERIOD	0x01
#define CFG_MAX_ALIVE_PERIOD	0xFF

#define CFG_DEF_REG_PERIOD      0xFF

#define CFG_MIN_REG_PERIOD    1
#define CFG_MAX_REG_PERIOD    0xFF

#define CFG_DEV_TYPE            0x02
#define CFG_DEV_ID                              {'S','M','A','R','T','P','L','U','G','0','3'}
#define CFG_P_VER               0x01

#define CFG_DEF_YEAR            0x18
#define CFG_DEF_MONTH           0x12
#define CFG_DEF_WEEKDAY         0x31        
#define CFG_DEF_DAY             0x31
#define CFG_DEF_HOUR            0x23
#define CFG_DEF_MIN             0x59
#define CFG_DEF_SEC             0x59

#define RTC_BUFF_SIZE_MAX       20

#pragma pack(push, 1)

typedef struct Smartplug_Parameter
{
      uint8_t   sys_mode;
      uint8_t   alive_period;
      uint8_t   reg_period;
      uint8_t   mtu;
      uint8_t   dev_type;
      uint8_t   dev_id[11];
      uint8_t   p_ver[1];
      
} Smartplug_Parameter_t;

typedef struct Smartplug_BaseConfig
{
      Smartplug_Parameter_t def;
      Smartplug_Parameter_t act;
} Smartplug_BaseConfig_t;

typedef struct KEPCO_SmartplugConfig
{
      Smartplug_BaseConfig_t bc;
} KEPCO_SmartplugConfig_t;

typedef struct Rtc_Parameter
{
      uint8_t year;
      uint8_t month;
      uint8_t weekday;
      uint8_t day;
      uint8_t hour;
      uint8_t min;
      uint8_t sec;    
        
} Rtc_Parameter_t;

typedef struct Rtc_BaseConfig
{
      Rtc_Parameter_t RtcBase;
      Rtc_Parameter_t RtcAct;
} Rtc_BaseConfig_t;

typedef struct INFO_RTC 
{
      uint32_t sec;
      uint8_t cmd;
}INFO_RTC_t;

typedef struct QUE_RTC
{
      INFO_RTC_t rtc_buff[RTC_BUFF_SIZE_MAX + 1];
      uint8_t front_index;
      uint8_t rear_index;
} QUE_RTC_t;

#pragma pack(pop)
