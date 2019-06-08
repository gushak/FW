#include "main.h"

CONFIG_INFO			Config;
uint8_t				g_CurrentConfigRecNo = 0xff;
uint8_t				fw_index = 0xff;
void flash_read(uint8_t *dest, uint8_t *src, uint32_t Len)
{
	uint32_t i = 0;
	uint8_t *psrc = src;
  
	for (i = 0; i < Len; i++)
	{
		dest[i] = *psrc++;
	}
}

int flash_write(uint8_t *src, uint8_t *dest, uint32_t Len)
{
	uint32_t i = 0;
	HAL_FLASH_Unlock();
	for (i = 0; i < Len; i +=2)
	{
	  /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
	     be done by byte */ 
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, (uint32_t)(dest + i), *(uint16_t*)(src + i)) == HAL_OK)
		{
		 /* Check the written value */
			if (*(uint16_t *)(src + i) != *(uint16_t*)(dest + i))
			{
				HAL_FLASH_Lock(); 
			  /* Flash content doesn't match SRAM content */
				return 2;
			}
		}
		else
		{
			HAL_FLASH_Lock(); 
		  /* Error occurred while writing data in Flash memory */
			return 1;
		}
	}
	HAL_FLASH_Lock(); 
	return 0;
}

int isFWUpdatePending()
{
	uint8_t buffer[CONFIG_REC_SIZE];
	PCONFIG_INFO pConfigRec = (PCONFIG_INFO)&(buffer[0]);
	uint8_t sector;
	int count;

	//
	//Current Config Record Scan
	//
	for (sector = 0; sector < CONFIG_REC_COUNT_PER_SECTOR; sector++)
	{
		flash_read(&buffer[0], (uint8_t *)CONFIG_ADDRESS + (sector*CONFIG_REC_SIZE), CONFIG_REC_SIZE);
		if(pConfigRec->hdr.ulMagic != CONFIG_MAGIC)			break;
		if (pConfigRec->hdr.ulSeqID < Config.hdr.ulSeqID)	break;

		memcpy(&Config, &buffer[0], sizeof(CONFIG_INFO));
		g_CurrentConfigRecNo = sector;
	}
	if (sector >= CONFIG_REC_COUNT_PER_SECTOR)
		return 0;
	
	count = sizeof(Config.fw_update) / sizeof(Config.fw_update[0]) - 1;
	while (Config.fw_update[count] != FLASH_CONFIG_UPDATE_PENDING_MARK && count >= 0) 
	{
		count--;	
	}

	if (count >= 0)
	{
		fw_index = count;
		return 1;
	}
	return 0;
}

void clearFWUpdatePending()
{
	if (g_CurrentConfigRecNo == 0xff || fw_index == 0xff)
		return;
	
	Config.fw_update[fw_index] = 0;
	flash_write((uint8_t *)&Config.fw_update[fw_index],
		(uint8_t *)CONFIG_ADDRESS + (g_CurrentConfigRecNo*CONFIG_REC_SIZE) + ((uint8_t *)&Config.fw_update[fw_index] - (uint8_t *)&Config), 
		sizeof(Config.fw_update[fw_index]));
}
void updateBootVerNum()
{
	if (Config.usBootVer == BOOT_VER)
		return;
	
	if (Config.usBootVer == 0xffff)
	{
		Config.usBootVer = BOOT_VER;
		flash_write((uint8_t *)&Config.usBootVer,
			(uint8_t *)CONFIG_ADDRESS + (g_CurrentConfigRecNo*CONFIG_REC_SIZE) + ((uint8_t *)&Config.usBootVer - (uint8_t *)&Config), 
			sizeof(Config.usBootVer));
	}
}