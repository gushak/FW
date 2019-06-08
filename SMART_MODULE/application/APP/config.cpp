#include "common.h"

#if defined(SAVE_CONF_DATA_INDIVIDUALLY)
Config::conf_cmd_t conf_buff[10];
Config gConf(10, &conf_buff[0]);
Config *pgConf = NULL;

static void config_process();

Config::Config(int n, conf_cmd_t* cmdBuff)
	: _pipeCmd(n, cmdBuff)
{
	pgConf = this;
	mutex_init();
	config_thread.start(config_process);
}

void config_process()
{
	gConf.lock();
	gConf.Init();
	gConf.unlock();
	wait_ms(100);
	
	while (gConf.bKeepRunning)
	{
		if (gConf.getInf()->readable())
		{
			Config::conf_cmd_t conf_cmd = gConf.getInf()->getc();
			gConf.flash_write(conf_cmd.buff, conf_cmd.addr, conf_cmd.size);
		}
	}
}

void Config::Init()
{
	DBGMSG(0, "[CONF] Load Config - start\r\n");
	LoadConfig();
	DBGMSG(0, "[CONF] Load Config - end\r\n");
	
	uint32_t temp;
	temp = *(__IO uint32_t*)(0x1FFFF7AC);
	memcpy(&pgContext->mcuId[0], (uint8_t *)&temp, 4);
	temp = *(__IO uint32_t*)(0x1FFFF7B0);
	memcpy(&pgContext->mcuId[4], (uint8_t *)&temp, 4);
	temp = *(__IO uint32_t*)(0x1FFFF7B4);
	memcpy(&pgContext->mcuId[8], (uint8_t *)&temp, 4);
}
#else
Config gConf;
Config *pgConf = NULL;

Config::Config()
{
	pgConf = this;
	mutex_init();
	
	DBGMSG(0, "[CONF] Load Config - start\r\n");
	LoadConfig();
	DBGMSG(0, "[CONF] Load Config - end\r\n");
	
	uint32_t temp;
	temp = *(__IO uint32_t*)(0x1FFFF7AC);
	memcpy(&pgContext->mcuId[0], (uint8_t *)&temp, 4);
	temp = *(__IO uint32_t*)(0x1FFFF7B0);
	memcpy(&pgContext->mcuId[4], (uint8_t *)&temp, 4);
	temp = *(__IO uint32_t*)(0x1FFFF7B4);
	memcpy(&pgContext->mcuId[8], (uint8_t *)&temp, 4);
}
#endif

BOOL Config::LoadConfig()
{
	u8 buffer[CONFIG_REC_SIZE];
	PCONFIG_INFO pConfigRec = (PCONFIG_INFO)&(buffer[0]);

	u32 uid0, uid1, uid2;
	char uid[13] = {0,};

	u8 i;

	g_Flash.init();
	//
	//Current Config Record Scan
	//
	for (i = 0; i < CONFIG_REC_COUNT_PER_SECTOR; i++)
	{
		g_Flash.read(&buffer[0], CONFIG_ADDRESS + (i*CONFIG_REC_SIZE), CONFIG_REC_SIZE);
		if(pConfigRec->hdr.ulMagic != CONFIG_MAGIC) break;
		if(pConfigRec->hdr.ulSeqID < CFG_HDR(ulSeqID)) break;

		memcpy(&CTX(Config), &buffer[0], sizeof(CONFIG_INFO));
		g_CurrentConfigRecNo = i;
	}

	if( CFG_HDR(ulMagic) != CONFIG_MAGIC )
	{
		DBGMSG(0, "[CONFIG] Configuration Not Founded! Apply Default\r\n");
		ApplyDefaultConfig(TRUE);
		
		CFG(usFwVer) = MAKE_VER16(FW_VER_MAJOR, FW_VER_MINOR);
		CFG(ulFwBuildDate) = FW_BUILD_DATE;
		CFG(usBootVer)			= 0xffff;

		SaveConfig();
	}
	else if(CFG_HDR(usVer) != CONFIG_VER)
	{
		//Major Version Different Case : Configuration Not Compatible
		if( (CFG_HDR(usVer)&0xff00) != (CONFIG_VER&0xff00) )
		{
			DBGMSG(0, "[CONFIG] Configuration Major Version Different! Apply Default\r\n");
			ApplyDefaultConfig(TRUE);
		}
		else
		{
			//Minor Version Different Case : Configuration Compatible
			DBGMSG(0, "[CONFIG] Configuration Minor Version Different! Apply Default Appended\r\n");

			CFG(usFwVer)		= CONFIG_VER;
			CFG(ulFwBuildDate)	= FW_BUILD_DATE;
			CFG_HDR(usVer)		= CFG(usFwVer);
			CFG(usBootVer)			= 0xffff;
			
			g_ConfigChanged = TRUE;
			DBGMSG(0, "[CONFIG] App Fw Version Changed\r\n");
		}
		SaveConfig();
	}

#if 0
	//< SerialNo(UID) Query & Build ... as like usb serial-no generation rule
	uid0 = *(__IO uint32_t*)(0x1FFFF7AC);
	uid1 = *(__IO uint32_t*)(0x1FFFF7B0);
	uid2 = *(__IO uint32_t*)(0x1FFFF7B4);
	sprintf(uid, "%08x%04x", uid0+uid2, (uid1>>16));
	memcpy(CFG(ucaSerialNo), uid, 12);
	//>
#endif
#if 1	//It's just for Test
	CFG(Setting).uwbId = 0;
	CFG(Setting).netId = 1;
	CFG(Setting).grpId[0] = 26;	//26 - 922.1MHz, 27 - 922.3MHz, 28 - 922.5MHz, 29 - 922.7MHz, 30 - 922.9MHz, 31 - 923.1MHz, 32 - 923.3MHz
	CFG(Setting).grpId[1] = 100;	//Default Group ID
	CFG(Setting).grpId[2] = 0;		//No Multi Gateway
#endif
//	CheckConfig();

	return 0;
}

BOOL Config::SaveConfig(bool freeze)
{
	u32 ulSizeWrite;

	// CRC Check Skip for Memory Reduce
#if 0	
	u16 usCRC = 0;

	usCRC = GetCommonCRC16(0, (u8*)&CTX(Config), CONFIG_REC_SIZE-4);
	if(usCRC == CFG_CRC())
	{
		return FALSE;	//
	}
#endif
	if (!freeze)
	{
		//Config Page Erase
		if (g_isZeroRecord || g_CurrentConfigRecNo == (CONFIG_REC_COUNT_PER_SECTOR - 1))
		{
			DBGMSG(0, "[CONFIG] Config Sector Erasing ... ", __func__);
			g_Flash.erase(CONFIG_ADDRESS, g_Flash.get_sector_size(FLASH_ADDR_CONFIG));
			DBGMSG(0, "Done!\r\n");

			g_CurrentConfigRecNo = 0;	//2016-02-24
			g_isZeroRecord = TRUE;
		}

		CFG_HDR(ulSeqID)++;

		if (g_isZeroRecord == FALSE)
		{
			g_CurrentConfigRecNo++;
		}
		
	}

	ulSizeWrite = (sizeof(CONFIG_INFO) > CONFIG_REC_SIZE) ? CONFIG_REC_SIZE : sizeof(CONFIG_INFO);

	g_Flash.program(&CTX(Config), CONFIG_ADDRESS + (g_CurrentConfigRecNo*CONFIG_REC_SIZE), ulSizeWrite);

	DBGMSG(0, "[CONFIG] Configuration Saved: addr=0x%x, rec#=%d, SeqNo=0x%x\r\n", 
				CONFIG_ADDRESS+(g_CurrentConfigRecNo*CONFIG_REC_SIZE),
				g_CurrentConfigRecNo, CFG_HDR(ulSeqID));

	g_ConfigChanged = FALSE;
	return TRUE;
}

#if 0
void Config::CheckConfig()
{
	u32 ulFwVer, ulFwBuildDate;

	ulFwVer = MAKE_VER(0, 0, DEF_FW_VER_MAJOR, DEF_FW_VER_MINOR);
	if(ulFwVer != CFG_TERM(ulFwVer))
	{
		CFG_TERM(ulFwVer) = ulFwVer;
	}

	ulFwBuildDate = DEF_FW_BUILD_DATE;
	if(ulFwBuildDate != CFG_TERM(ulFwBuildDate))
	{
		CFG_TERM(ulFwBuildDate) = ulFwBuildDate;
	}
}
#endif

void Config::ApplyDefaultConfig(BOOL isEmptyConfig)
{
	if(isEmptyConfig)
	{
		g_CurrentConfigRecNo = 0;
		g_isZeroRecord		 = TRUE;

		memset(&CTX(Config), 0, sizeof(CONFIG_INFO));

//		g_Flash.erase(CONFIG_ADDRESS, g_Flash.get_sector_size(CONFIG_ADDRESS));
	}

	//
	// Header
	//
	CFG_HDR(ulMagic)		= CONFIG_MAGIC;
	CFG_HDR(usVer)			= CONFIG_VER;
	CFG_HDR(usSize)			= sizeof(CONFIG_INFO);

	CFG(usFwVer)			= CONFIG_VER;
	CFG(ulFwBuildDate)		= FW_BUILD_DATE;
	CFG(usBootVer)			= 0xffff;

	memset(&CFG(fw_update), 0xff, sizeof(CFG(fw_update)));
	//
	// Setting
	//
	memset(&CFG(Setting), 0xff, sizeof(CFG(Setting)));
	CFG(Setting).uwbId = 0;
	CFG(Setting).netId = 1;
	CFG(Setting).grpId[0] = 26;	//26 - 922.1MHz, 27 - 922.3MHz, 28 - 922.5MHz, 29 - 922.7MHz, 30 - 922.9MHz, 31 - 923.1MHz, 32 - 923.3MHz
	CFG(Setting).grpId[1] = 100;	//Default Group ID
	CFG(Setting).grpId[2] = 0;		//No Multi Gateway
	
	g_ConfigChanged = TRUE;
}

bool Config::ChangeConfig(void* conf, void* value, int size)
{
	if (!memcmp(conf, value, size))
		return false;
	memcpy(conf, value, size);
	g_ConfigChanged = TRUE;
	return true;
}

void Config::scanExfIndices(eEXF_TYPE type)
{
	#define DEFAULT_UNIT_SIZE	4
	
	int index = 0, count, unit_size = DEFAULT_UNIT_SIZE;
	int addr = 0;
	switch (type)
	{
	case eetWP:	//write-pointer based on 32bytes unit
		addr = FLASH_CONFIG_WP_START;
		count = (FLASH_CONFIG_RP_START - FLASH_CONFIG_WP_START) / unit_size;
		break;
	case eetRP:	//read-pointer based on 32bytes unit
		addr = FLASH_CONFIG_RP_START;
		count = (FLASH_CONFIG_WN_START - FLASH_CONFIG_RP_START) / unit_size;
		break;
	case eetWN:	//work number based on 1byte unit
		addr = FLASH_CONFIG_WN_START;
		count = (FLASH_CONFIG_WN_END - FLASH_CONFIG_WN_START) / unit_size;
		break;
	default:
		return;
	}

	while (*(uint32_t*)addr != (uint32_t)(-1) && index < count)
	{
		index++;
		addr += unit_size;
	}
	
	if (index != 0)
		addr -= DEFAULT_UNIT_SIZE;
	
	switch (type)
	{
	case eetWP:	//write-pointer based on 32bytes unit
		if (index >= count)
		{
			CTX_EXF(wp) = 0;
			CTX_EXF(wp_idx) = 0;
			break;	
		}
		CTX_EXF(wp) = *(uint32_t*)addr;
		CTX_EXF(wp_idx) = index;
		break;
	case eetRP:	//read-pointer based on 32bytes unit
		if (index >= count)
		{
			CTX_EXF(rp) = 0;
			CTX_EXF(rp_idx) = 0;
			break;	
		}
		CTX_EXF(rp) = *(uint32_t*)addr;
		CTX_EXF(rp_idx) = index;
		break;
	case eetWN:	//work number
		if (index >= count)
		{
			CTX_EXF(wn) = 1;	// work number between 1 and 8
			CTX_EXF(wn_idx) = 0;
			break;	
		}
		CTX_EXF(wn) = *(uint32_t*)addr;
		CTX_EXF(wn_idx) = index;
		break;
	default:
		return;
	}	
}

void Config::updateExfIndices(eEXF_TYPE type, uint32_t value)
{
	int index = 0, count, unit_size = sizeof(value);
	int addr = 0;
	switch (type)
	{
	case eetWP:	//write-pointer based on 32bytes unit
		addr = FLASH_CONFIG_WP_START + CTX_EXF(wp_idx) * unit_size;
		count = (FLASH_CONFIG_RP_START - FLASH_CONFIG_WP_START) / unit_size;
		CTX_EXF(wp_idx) += 1;
		if (CTX_EXF(wp_idx) >= count)
		{
			CTX_EXF(wp_idx) = 0;
			pgConf->flash_erase_sector(addr);
			addr = FLASH_CONFIG_WP_START + CTX_EXF(wp_idx) * unit_size;
		}
		CTX_EXF(wp) = value;
		if (value >= EF_COUNT_MAX)
			value = 0;
		break;
	case eetRP:	//read-pointer based on 32bytes unit
		addr = FLASH_CONFIG_RP_START + CTX_EXF(rp_idx) * unit_size;
		count = (FLASH_CONFIG_WN_START - FLASH_CONFIG_RP_START) / unit_size;
		CTX_EXF(rp_idx) += 1;
		if (CTX_EXF(rp_idx) >= count)
		{
			CTX_EXF(rp_idx) = 0;
			pgConf->flash_erase_sector(addr);
			addr = FLASH_CONFIG_RP_START + CTX_EXF(rp_idx) * unit_size;
		}
		CTX_EXF(rp) = value;
		if (value >= EF_COUNT_MAX)
			value = 0;
		break;
	case eetWN:	//work number based on 1byte unit
		addr = FLASH_CONFIG_WN_START + CTX_EXF(wn_idx) * unit_size;
		count = (FLASH_CONFIG_WN_END - FLASH_CONFIG_WN_START) / unit_size;
		CTX_EXF(wn_idx) += 1;
		if (CTX_EXF(wn_idx) >= count)
		{
			CTX_EXF(wn_idx) = 0;
			pgConf->flash_erase_sector(addr);
			addr = FLASH_CONFIG_WN_START + CTX_EXF(wn_idx) * unit_size;
		}
		CTX_EXF(wn) = value;
		break;
	default:
		return;
	}
	
	pgConf->flash_write((uint8_t*)&value, addr, unit_size);
}