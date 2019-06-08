#include "common.h"
#include "debug.h"

__ALIGN_BEGIN char gExfBuff[256] __ALIGN_END = { 0, };
EXT_FLASH gExtFlash((char*)"fs", EF_PINS, EF_CLOCK_HZ);
EXT_FLASH *pgExtFlash;

static void exf_process();

EXT_FLASH::EXT_FLASH(char* name, PinName mosi, PinName miso, PinName sclk, PinName cs, int freq)
	: SPI_NOR(mosi, miso, sclk, cs, freq)
{
	pgExtFlash = this;
	mutex_init();
	flash_thread.start(exf_process);
}
EXT_FLASH::~EXT_FLASH()
{
}

void exf_process()
{
	while (pgConf == NULL)
	{
		wait_ms(100);
	}
	pgConf->scanExfIndices(eEXF_TYPE::eetWP);
	pgConf->scanExfIndices(eEXF_TYPE::eetRP);
	pgConf->scanExfIndices(eEXF_TYPE::eetWN);
	
	if (CTX_EXF(wp) == (uint32_t)(-1))
	{
		CTX_EXF(wp) = 0;
		CTX_EXF(wp_idx) = 0;		
	}
	if (CTX_EXF(rp) == (uint32_t)(-1))
	{
		CTX_EXF(rp) = 0;
		CTX_EXF(rp_idx) = 0;
	}

	if (CTX_EXF(wn) == (uint32_t)(-1))
	{
		CTX_EXF(wn) = 1;
		CTX_EXF(wn_idx) = 0;
	}	
}

int EXT_FLASH::write_tool_data(uint8_t *buffer)
{
	if (CTX_EXF(wp) == (uint32_t)(-1))
		return -1;
	
	if (CTX_EXF(wp) >= EF_COUNT_MAX)
		CTX_EXF(wp) = 0;

	uint32_t page = CTX_EXF(wp) / EF_TOOL_DATA_PER_PAGE;
	uint32_t index = CTX_EXF(wp) % EF_TOOL_DATA_PER_PAGE;

	lock();
	disk_read((uint8_t *)gExfBuff, (uint64_t)page, 1);
	unlock();

	if (!index && *(uint32_t *)gExfBuff != (-1))
	{
		if (CTX_EXF(rp) / EF_TOOL_DATA_PER_4KSECTOR == CTX_EXF(wp) / EF_TOOL_DATA_PER_4KSECTOR)
		{
			CTX_EXF(rp) = CTX_EXF(rp) / EF_TOOL_DATA_PER_4KSECTOR * EF_TOOL_DATA_PER_4KSECTOR + EF_TOOL_DATA_PER_4KSECTOR;
			pgConf->updateExfIndices(eEXF_TYPE::eetRP, 0);
			if (CTX_EXF(rp) >= EF_COUNT_MAX)
				CTX_EXF(rp) = 0;
		}
		else if ((CTX_EXF(rp) / EF_TOOL_DATA_PER_4KSECTOR == 0) && (CTX_EXF(wp) / EF_TOOL_DATA_PER_4KSECTOR == 0))
		{
			//no handle
		}
		lock();
		disk_erase(CTX_EXF(wp) / EF_TOOL_DATA_PER_4KSECTOR, 1);
		unlock();
	}
	
	memcpy(gExfBuff + index*EF_TOOL_DATA_SIZE, buffer, EF_TOOL_DATA_SIZE);
	lock();
	disk_write((uint8_t *)gExfBuff, (uint64_t)page, 1);
	unlock();
	
	pgConf->updateExfIndices(eEXF_TYPE::eetWP, CTX_EXF(wp) + 1);
	
	return index;
}

int EXT_FLASH::read_tool_data(uint8_t *buffer)
{
 
    if (CTX_EXF(rp) == (uint32_t)(-1))
		return -1;

	if (CTX_EXF(rp) >= EF_COUNT_MAX)
		CTX_EXF(rp) = 0;

	uint32_t page = CTX_EXF(rp) / EF_TOOL_DATA_PER_PAGE;
	uint32_t index = CTX_EXF(rp) % EF_TOOL_DATA_PER_PAGE;
	
	
	lock();
	disk_read((uint8_t *)gExfBuff, (uint64_t)page, 1);
	unlock();
	
	memcpy(buffer, gExfBuff + index*EF_TOOL_DATA_SIZE, EF_TOOL_DATA_SIZE);
	
	return index;
}

int EXT_FLASH::flush_tool_data()
{
	uint16_t bkRP = CTX_EXF(rp);
	pgConf->updateExfIndices(eEXF_TYPE::eetRP, CTX_EXF(rp) + 1);
	if (!(CTX_EXF(rp) % EF_TOOL_DATA_PER_4KSECTOR))
	{
		lock();
		disk_erase(bkRP / EF_TOOL_DATA_PER_4KSECTOR, 1);
		unlock();
	}
	return 0;
}