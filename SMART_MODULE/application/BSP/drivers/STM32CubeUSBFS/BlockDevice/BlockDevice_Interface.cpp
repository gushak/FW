/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <mbed.h>
#include "BlockDevice_Interface.h"
#include "usb_disk.h"


Mutex _mutex;void lock() {	_mutex.lock(); }
void unlock() {	_mutex.unlock(); }

extern "C"
{
	#if defined(USBMSC_SDIO_SD)
	#include "sdio_sd.h"
	#endif
	
	uint8_t BD_Init(void)
	{
		lock();
		#if defined(USBMSC_SDIO_SD)
		SDIO_SD_Init();
		#endif
		unlock();
	}
	uint8_t BD_DeInit(void)
	{
		lock();
		#if defined(USBMSC_SDIO_SD)
		SDIO_SD_DeInit();
		#endif
		unlock();
	}
	uint8_t BD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		//return SDIO_SD_ReadBlocks(pData, ReadAddr, NumOfBlocks, Timeout);
		ret = SDIO_SD_ReadBlocks_DMA(pData, ReadAddr, NumOfBlocks);
		#else
		ret = (uint8_t)gUsbDisk.getBD()->usbmsd_read((uint8_t*)pData, (uint64_t)ReadAddr, (uint8_t)NumOfBlocks);
		#endif
		unlock();
		return ret;
	}
	uint8_t BD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		//return SDIO_SD_WriteBlocks(pData, WriteAddr, NumOfBlocks, Timeout);
		ret = SDIO_SD_WriteBlocks_DMA(pData, WriteAddr, NumOfBlocks);
		#else
		ret = (uint8_t)gUsbDisk.getBD()->usbmsd_write((uint8_t*)pData, (uint64_t)WriteAddr, (uint8_t)NumOfBlocks);
		#endif
		unlock();
		return ret;
	}
	uint8_t BD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		ret = SDIO_SD_ReadBlocks_DMA(pData, ReadAddr, NumOfBlocks);
		#else
		ret = (uint8_t)gUsbDisk.getBD()->usbmsd_read((uint8_t*)pData, (uint64_t)ReadAddr, (uint8_t)NumOfBlocks);
		#endif
		unlock();
		return ret;
	}
	uint8_t BD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		ret = SDIO_SD_WriteBlocks_DMA(pData, WriteAddr, NumOfBlocks);
		#else
		ret = (uint8_t)gUsbDisk.getBD()->usbmsd_write((uint8_t*)pData, (uint64_t)WriteAddr, (uint8_t)NumOfBlocks);
		#endif
		unlock();
		return ret;
	}
	uint8_t BD_Erase(uint32_t StartAddr, uint32_t EndAddr)
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		ret = SDIO_SD_Erase(StartAddr, EndAddr);
		#else
		ret = (uint8_t)gUsbDisk.getBD()->usbmsd_erase(StartAddr, EndAddr);
		#endif
		unlock();
		return ret;
	}
	uint8_t BD_GetCardState(void)
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		ret = SDIO_SD_GetCardState();
		#else
		ret = SD_TRANSFER_OK;
		#endif
		unlock();
		return ret;
	}
	uint8_t BD_IsDetected()
	{
		uint8_t ret;
		lock();
		#if defined(USBMSC_SDIO_SD)
		ret = SDIO_SD_IsDetected();
		#else
		ret = SD_PRESENT;
		#endif
		unlock();
		return ret;
	}

	void BD_GetCardInfo(SD_CardInfo *pCardInfo)
	{
		lock();
		#if defined(USBMSC_SDIO_SD)
		SDIO_SD_GetCardInfo(pCardInfo);
		#else
		pCardInfo->LogBlockSize = V_SECTOR_SIZE;
		pCardInfo->CardBlockSize = V_SECTOR_SIZE;
		pCardInfo->CardCapacity = TOTAL_V_SECTORS * pCardInfo->LogBlockSize;
		pCardInfo->LogBlockNbr = TOTAL_V_SECTORS;
		#endif
		unlock();
	}
}