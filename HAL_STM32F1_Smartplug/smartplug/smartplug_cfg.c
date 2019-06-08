/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: smartplug configuration
*
* History
* Date			Description
* 2018/09/28	file created
*****************************************************************************/

#include "smartplug_cfg.h"
#include "comm_dbg.h"
#include "smartplug_hw.h"

#pragma pack(push,1)

static const Smartplug_Parameter_t device_default_param = 
{
      CFG_DEF_SYS_MODE,
      CFG_DEF_ALIVE_PERIOD,
      CFG_DEF_REG_PERIOD,
      CFG_DEF_MTU,
      CFG_DEV_TYPE,
      CFG_DEV_ID,
      CFG_P_VER
};

static const Rtc_Parameter_t rtc_default_param = 
{
      CFG_DEF_YEAR,
      CFG_DEF_MONTH,
      CFG_DEF_WEEKDAY,
      CFG_DEF_DAY,
      CFG_DEF_HOUR,
      CFG_DEF_MIN,
      CFG_DEF_SEC
};

#pragma pack(pop)

uint16_t cfg_alive_period(const CurConfig_t* cfg)
{
      uint16_t alive_period = cfg->bc.act.alive_period;
      
      if(alive_period < 10)
            alive_period = 10;
      
//      LOCAL_DPRT("alive_period=%d", alive_period);
      
      return alive_period;
}

void cfg_set_device_default(CurConfig_t* cfg, Rtc_BaseConfig_t* RtcCfg)
{
	cfg->bc.def = device_default_param;
	cfg->bc.act = device_default_param; 
        RtcCfg->RtcBase = rtc_default_param;
        RtcCfg->RtcAct = rtc_default_param;

}

uint8_t load_flash_info(uint32_t addr, uint16_t size)
{
      if(flash_empty(addr))
      {
            flash_read(addr, &g_FlashInfo, size);
      
            g_config.bc.def.reg_period = g_FlashInfo.RegiPeriod;
            g_config.bc.def.alive_period = g_FlashInfo.AlivePeriod;
            memcpy(&g_config.bc.def.dev_id[0], &g_FlashInfo.szDevid[0], sizeof(g_FlashInfo.szDevid));
            
            return kTrue;
      }
      return kFalse;
}