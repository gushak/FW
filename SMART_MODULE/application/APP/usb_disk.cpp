#include "common.h"

#include "usb_disk.h"
#include "pll_values.h"
#include "rtc_api.h"

#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
#include "STM32CubeUSBFS/BlockDevice/sdio_sd.h"
#endif

using namespace gnss;

//#define HOLD_ON_REAL_MODE

USB_DISK	gUsbDisk((char*)"disk", SPI_PINS, SPI_CLOCK_HZ);
uint8_t read_buff[PAGE_SIZE];
//#define FS_RD_WR_TEST
#ifdef FS_RD_WR_TEST
static uint32_t gps_offset = 0;
static uint32_t imu_offset = 0;
#endif

//	typedef enum
//	{
//		eFI_IMU,
//		eFI_UHF,
//		eFI_GPS_NMEA,
//		eFI_GPS_RAW,	//UBX or RT17
//		eFI_GPS_RTCM,
//		eFI_GPS_CMR,
//		eFI_GPS_BINEX,
//		eFI_COUNT
//	} eFILE_ID;
char filename_prefix[USB_DISK::eFI_COUNT][6] = { "imu", "uhf", "gps"/*NMEA*/, "gps"/*RAW*/, "gps"/*RTCM*/, "gps"/*CMR*/, "gps"/*BINEX*/ };

static void usbdisk_process();

USB_DISK::USB_DISK(char* name, PinName mosi, PinName miso, PinName sclk, PinName cs, int freq)
	#if defined(USBMSC_SPI_FLASH)
	: _fs(name), _bd(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS, freq)
	#endif
{
	bkConnStat = -1;
#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
	memset(SDPath, 0, sizeof(SDPath));
#endif
	for (int loop = 0; loop < eFI_COUNT; loop++)
		pFH[loop] = NULL;
	
	USBD_Device.pClass = NULL;

	pVBus = new DigitalIn(PA_9);
	usbdisk_thread.start(usbdisk_process);
}

void USB_DISK::return_error(int ret_val) {
	if (ret_val)
	{
		DBGMSG(0, "Failure. %d\r\n", ret_val);
	}
	else
	{
		DBGMSG(0, "done.\r\n");
	}
}

void USB_DISK::errno_error(void* ret_val) {
	if (ret_val == NULL)
	{
		DBGMSG(0, " Failure. %d \r\n", errno);
	}
	else
	{
		DBGMSG(0, " done.\r\n");
	}
}

static void usbdisk_process()
{
	DBGMSG(0, "[DISK] Process Started===>\r\n");

	Pipe<char> *pipe = NULL;
	gUsbDisk.bKeepRunning = true;
	int timeout = 100;
	int block_size = 0;
	SDIO_SD_ITConfig();
	while (	!DevKeepRunning
			|| gGps.getThreadState() < Thread::Ready 
			|| gGps.getThreadState() == Thread::Deleted)
	{
		wait_ms(1);
	}
	gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_MOUNT;
	gUsbDisk.pending_steps = USB_DISK::eUSBDISK_INVALID;
	
	while (gUsbDisk.bKeepRunning)
	{
		int usbconnStat = gUsbDisk.isConnected();

		#if !defined(HOLD_ON_REAL_MODE)
		if (gUsbDisk.bkConnStat != usbconnStat)
		{
			gUsbDisk.clrReady();
			if (usbconnStat)
			{
				gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_DEMOUNT;
				//changeSysClock(PLL_MODE_SPEEDUP);
			}
			else
			{
				BitOpMode = 0;
				if (!CTX_MAIN(facMode) && gUsbDisk.bkConnStat != -1)
				{
					if ((CTX_MAIN(pwrOnSrc) & BIT_FLAG(ePOS_USB)))
					{
						gDisp.Comment(DISP::eDCMT_SYS_CLOSE, WAIT_FOREVER);
						killSystem();
						DBGMSG(0, "[DISK] Trigger main termination -- 1\r\n");
						break;
					}
					else
					gUsbDisk.steps = USB_DISK::eUSBDISK_INVALID;
				}
				else
					gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_MOUNT;
				//changeSysClock(PLL_MODE_NORMAL);
			}
			gUsbDisk.bkConnStat = usbconnStat;
		}
		#endif

		switch (gUsbDisk.steps)
		{
		case USB_DISK::eUSBDISK_LOCAL_DEMOUNT:
			gUsbDisk.closeFiles();
			gUsbDisk.unmountFileSystem();
			gGps.suspend();
			gImu.suspend();
			gUsbDisk.steps = USB_DISK::eUSBDISK_REMOTE_MOUNT;
			break;
		case USB_DISK::eUSBDISK_REMOTE_MOUNT:
			if (gGps.getSuspend() <= 0 || gImu.getSuspend() <= 0)
			{
				break;
			}
			if (usbconnStat)
			{
				gUsbDisk.activateUSB();
				gUsbDisk.setReady();
				gDisp.flush();
				gDisp.Comment(DISP::eDCMT_USB_READY, 4);
				gUsbDisk.steps = USB_DISK::eUSBDISK_INVALID;
			}
			else
			{
				gDisp.Comment(DISP::eDCMT_NO_MEDIA, WAIT_STATIC);
				gUsbDisk.terminateFileSave();
			}
			break;
		case USB_DISK::eUSBDISK_LOCAL_MOUNT:
			if (BitOpMode & eDOM_DEBUG == 0)
				gUsbDisk.deactivateUSB();
				
			gGps.resume();
			gImu.resume();
			DBGMSG(0, "[DISK] FS Init -->1\r\n");
				if (!BD_IsDetected())
				{
				gDisp.Comment(DISP::eDCMT_NO_MEDIA, WAIT_STATIC);
				gUsbDisk.terminateFileSave();
					break;
				}
			else
				gDisp.RemoveComment(DISP::eDCMT_NO_MEDIA);
			
			if (gUsbDisk.mountFileSystem())
			{
				gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_FORMAT;
			}
			else
			{
				gDisp.RemoveComment(DISP::eDCMT_FS_ERROR);
				gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_TIME;
			}
			timeout = 200;
			DBGMSG(0, "[DISK] FS Init -->2\r\n");
			break;
		case USB_DISK::eUSBDISK_LOCAL_TIME:
			if (gDisp.getMBatt() != .0f && gDisp.getMBatt() < BATT_LEVEL_SHUTDOWN)
			{
				gDisp.Comment(DISP::eDCMT_LOW_BATT, WAIT_STATIC);
				DBGMSG(0, "[BATT] Low Batt.------------------------------------------------------>\r\n");
				gUsbDisk.terminateFileSave();
				break;
			}
			if (gGps.getLocalTime() == 0)
			{
				if (timeout > 0)
					timeout--;
				if (!timeout)
				{
					timeout--;
					gDisp.Comment(DISP::eDCMT_NO_GPSTIME, WAIT_STATIC);
				}
				break;
			}
			timeout = 200;
			gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_FS;
			gDisp.RemoveComment(DISP::eDCMT_NO_GPSTIME);
			break;
		case USB_DISK::eUSBDISK_LOCAL_FORMAT:
			if (gDisp.getMBatt() !=.0f && gDisp.getMBatt() < BATT_LEVEL_SHUTDOWN)
			{
				gDisp.Comment(DISP::eDCMT_LOW_BATT, WAIT_STATIC);
				DBGMSG(0, "[BATT] Low Batt.------------------------------------------------------>\r\n");
				gUsbDisk.terminateFileSave();
				break;
			}
			if (BD_IsDetected())
			{
				gDisp.RemoveComment(DISP::eDCMT_NO_MEDIA);
				timeout = 300;
				DBGMSG(0, "[DISK] FS eUSBDISK_LOCAL_FORMAT -->5\r\n");
				gUsbDisk.formatFileSystem();
				gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_FS;
				DBGMSG(0, "[DISK] FS eUSBDISK_LOCAL_FORMAT -->6\r\n");
			}
			else
			{
				gDisp.Comment(DISP::eDCMT_NO_MEDIA, WAIT_STATIC);
				gUsbDisk.terminateFileSave();
			}
			break;
		case USB_DISK::eUSBDISK_LOCAL_FS:
			if (gDisp.getMBatt() != .0f && gDisp.getMBatt() < BATT_LEVEL_SHUTDOWN)
			{
				gDisp.Comment(DISP::eDCMT_LOW_BATT, WAIT_STATIC);
				DBGMSG(0, "[BATT] Low Batt.------------------------------------------------------>\r\n");
				gUsbDisk.terminateFileSave();
				break;
			}
			gDisp.RemoveComment(DISP::eDCMT_LOW_BATT);
			timeout--;
			DBGMSG(0, "[DISK] eUSBDISK_LOCAL_FS - timeout = %d\r\n", timeout);

			if (timeout > 0 && BD_IsDetected() && gUsbDisk.openFiles())
			{
				gDisp.RemoveComment(DISP::eDCMT_NO_MEDIA);
				break;
			}
			DBGMSG(0, "[DISK] FS Init -->4\r\n");
			gDisp.flush();
			gUsbDisk.steps = USB_DISK::eUSBDISK_IDLE;
			if (!gUsbDisk.isAllFilesOpen())
			{
				if (BD_IsDetected())
				{
					gDisp.RemoveComment(DISP::eDCMT_NO_MEDIA);
					if (timeout <= 0)
						gUsbDisk.steps = USB_DISK::eUSBDISK_LOCAL_FORMAT;
					gDisp.Comment(DISP::eDCMT_FS_FORMAT, WAIT_FOREVER);
				}
				else
				{
					gDisp.Comment(DISP::eDCMT_NO_MEDIA, WAIT_STATIC);
					gUsbDisk.terminateFileSave();
				}
				break;				
			}

			gUsbDisk.setReady();
			gDisp.flush();
			gDisp.Comment(DISP::eDCMT_FS_READY, 2);
			break;
		case USB_DISK::eUSBDISK_IDLE:
			if (BitOpMode & eDOM_PCIF && gpPcif == NULL)
				openPCIF();

			if (BitOpMode & 0xff00)
				break;

			for (int loop = 0; loop < USB_DISK::eFI_COUNT; loop++)
			{
				if (!(CFG(Setting).saveFiles & BIT_FLAG(loop)))	continue;
				if (!gUsbDisk.getFileHandle((USB_DISK::eFILE_ID)loop)) continue;;
				switch (loop)
				{
				case USB_DISK::eFI_IMU:
					pipe = gImu.getInf();	
					if (pipe->size() < PAGE_SIZE)	pipe = NULL;
					break;
				case USB_DISK::eFI_UHF:
					pipe = (Pipe<char>*)&gUhf;
					if (pipe->size() < PAGE_SIZE)	pipe = NULL;
					break;
				case USB_DISK::eFI_GPS_NMEA: 
					pipe = gGps.getInf(GnssParser::eGPSOUT_NMEA);	
					if (pipe->size() < PAGE_SIZE)	pipe = NULL;
					break;
				case USB_DISK::eFI_GPS_RAW:
					pipe = gGps.getInf(GnssParser::eGPSOUT_RT17);	
					if (pipe->size() < PAGE_SIZE)	pipe = NULL;
					break;
				case USB_DISK::eFI_GPS_RTCM:
				case USB_DISK::eFI_GPS_CMR:
				case USB_DISK::eFI_GPS_BINEX:
					pipe = gGps.getInf(GnssParser::eGPSOUT_COUNT);	
					if (pipe->size() < PAGE_SIZE)	pipe = NULL;
					break;
				default:
					pipe = NULL;
					break;
				}
				
				if (pipe)
				{
					gUsbDisk.write((USB_DISK::eFILE_ID)loop, pipe, PAGE_SIZE);
					pipe->flush(PAGE_SIZE);
				}
		}
		default:
			break;
		}
		
		if (gUsbDisk.pending_steps != USB_DISK::eUSBDISK_INVALID)
		{
			gUsbDisk.steps = gUsbDisk.pending_steps;
			gUsbDisk.pending_steps = USB_DISK::eUSBDISK_INVALID;
		}
		if (BitOpMode & eDOM_PCIF && gpPcif == NULL)
			openPCIF();
		
		wait_ms(10);
	}
	DBGMSG(0, "[DISK] Terminate \r\n");
	gUsbDisk.closeFiles();
	gUsbDisk.unmountFileSystem();
	DBGMSG(0, "[DISK] Terminated!! \r\n");
	wait_ms(1);
	gUsbDisk.deactivateUSB();
}


#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD) || defined(USBMSC_SPI_FLASH)
int USB_DISK::formatFileSystem()
{
#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
	f_mkfs((const TCHAR *)SDPath, 1, 0);
	if (f_mount(gUsbDisk.getFS(), (TCHAR const*)gUsbDisk.SDPath, 1) != FR_OK)
	{
		gDisp.flush();
		gDisp.Comment(DISP::eDCMT_FS_ERROR, WAIT_STATIC);
		return -1;
	}
#else
	// Reformat if we can't mount the filesystem,
    // this should only happen on the first boot
	SPIFFileSystem::format(gUsbDisk.getBD());
	if (gUsbDisk.getFS()->mount(gUsbDisk.getBD()))
	{
		gDisp.flush();
		gDisp.Comment(DISP::eDCMT_FS_ERROR, WAIT_STATIC);
		while (true)
			wait(1);
	}
#endif
	return 0;
}
int USB_DISK::mountFileSystem()
{
#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
	if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0)
	{
		gDisp.Comment(DISP::eDCMT_FS_ERROR, WAIT_STATIC);
		return -1;
	}
#endif

	gDisp.Comment(DISP::eDCMT_FS_INIT, WAIT_STATIC);
#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
	wait(1);

	/*##-4- Register the file system object to the FatFs module ################*/
	if (f_mount(gUsbDisk.getFS(), (TCHAR const*)gUsbDisk.SDPath, 1) != FR_OK)
		return -1;

	char volumeLabel[25];	//BCD format
	f_getlabel(SDPath, volumeLabel, 0);
	uint8_t label;
	label = (atoi(&volumeLabel[0]) << 4) | (atoi(&volumeLabel[1]) & 0x0f);
	if (!CFG(Setting).devId && (!strlen(volumeLabel) || !label))
	{
		uint32_t label32 = (uint32_t)&g_Context.mcuId[0] + (uint32_t)&g_Context.mcuId[4] + (uint32_t)&g_Context.mcuId[8];
		sprintf(volumeLabel, "%08x", label32);
		f_setlabel(volumeLabel);
	}
	else if (CFG(Setting).devId != label)
	{
		sprintf(volumeLabel, "%d%d", CFG(Setting).devId >> 4, CFG(Setting).devId & 0xf);
		f_setlabel(volumeLabel);
	}
	
#elif defined(USBMSC_SPI_FLASH)
	if (gUsbDisk.getFS()->mount(gUsbDisk.getBD()))
		return -1;
#endif
	
	return 0;
}

int USB_DISK::unmountFileSystem()
{
	#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
	if (getFS()->fs_type >= 2)
	{
		f_mount(0, SDPath, 0);
		FATFS_UnLinkDriver(SDPath);
		SDPath[0] = 0;
		getFS()->fs_type = 0;
		BD_DeInit();
	}
	#elif defined(USBMSC_SPI_FLASH)
    // Release any resources we were using
	_fs.unmount();
	#endif
	return 0;
}

bool USB_DISK::isAllFilesOpen()
{
	for (int loop = 0; loop < eFI_COUNT; loop++)
	{
		if (!(CFG(Setting).saveFiles & BIT_FLAG(loop)))	continue;
		if (!getFileHandle((eFILE_ID)loop)) return false;
	}
	return true;
}

int USB_DISK::openFiles()
{
	int ret = 0;
	for (int loop = 0; loop < eFI_COUNT; loop++)
	{
		if (!(CFG(Setting).saveFiles & BIT_FLAG(loop))) 
		{	
			if (getFileHandle((eFILE_ID)loop))
				closeFile((eFILE_ID)loop);
			continue;
		}
		if (getFileHandle((eFILE_ID)loop)) continue;
		
		ret |= openFile((eFILE_ID)loop, filename_prefix[loop]);
	}
	
	return ret;
}
int USB_DISK::closeFiles()
{
    // Remember that storage may not be updated until the file
    // is closed successfully
	for (int loop = 0; loop < eFI_COUNT; loop++)
		closeFile((eFILE_ID)loop);

	return 0;
}

Timeout *tm_charger_checker;
static void check_charger()
{
	if (!gUsbDisk.isConfigured())
	{
		gDisp.flush();
		gDisp.Comment(DISP::eDCMT_CHG_CONN, WAIT_FOREVER);
		killSystem();
		DBGMSG(0, "[DISK] Trigger main termination -- 3\r\n");
	}
	delete tm_charger_checker;
	tm_charger_checker = NULL;
}

int USB_DISK::activateUSB()
{
	if (pVBus != NULL)
	{
		free(pVBus);
		pVBus = NULL;
	}
	/* Init MSC Application */
	USBD_Init(&USBD_Device, &MSC_CDC_Desc, 0);
  
	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_MSC_CDC_CLASS);
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);
	/* Add Storage callbacks for MSC Class */
	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);
  
	/* Start Device Process */
	USBD_Start(&USBD_Device);
//	tm_charger_checker = new Timeout();
//	tm_charger_checker->attach(check_charger, 10.0f);

	return 0;
}

int USB_DISK::deactivateUSB()
{
	if (USBD_Device.pClass != NULL)
	{
		/* Disconnect the USB device */
		USBD_DeInit(&USBD_Device);
		USBD_Device.pClass = NULL;
		BD_DeInit();
	}
	pVBus = new DigitalIn(PA_9);
	closePCIF();
	BitOpMode = 0;
	if (CFG(Setting).gnssOutputs != CFG(Setting).gnssDefOut)
	{
		gConf.ChangeConfigU8(&CFG(Setting).gnssOutputs, CFG(Setting).gnssDefOut);
		gGps.changeOutputFormat(CFG(Setting).gnssDefOut, CFG(Setting).gnssRates);
	}
	return 0;
}
char * FILEID2EXT_TBL[] = {(char *)"dat", (char *)"dat", (char *)"nmea", (char *)"raw", (char *)"rtcm", (char *)"cmr", (char *)"binex"};
int USB_DISK::openFile(eFILE_ID id, char *pDevName)
{
	if (getFileHandle(id) != NULL)
		return 0;
	int ret = 0;
	char name[32] = { 0, };
	char ext[5] = { 0, };
	//sprintf(&name[0], "%s.dat", pDevName);
	// Convert the time into a tm
	struct tm ti;
	rtc_read_struct(&ti);
	if (id < eFI_GPS_NMEA)
		strcpy(ext, "dat");
	else
		strcpy(ext, FILEID2EXT_TBL[id]);

	sprintf(&name[0], "%s_%02d%02d%02d%02d.%s", pDevName, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec, ext);
	ret = f_open(&FileHandle[id], (const char*)name, FA_CREATE_ALWAYS | FA_WRITE);
	DBGMSG(0, "[DISK] FS - %s is created\r\n", name);

	setFileHandle(id, &FileHandle[id]);
	return ret;
}

void USB_DISK::closeFile(eFILE_ID id)
{
	if (getFileHandle(id) == NULL)
		return;
	f_close(&FileHandle[id]);
	setFileHandle(id, NULL);
}

int USB_DISK::seek(eFILE_ID id, uint32_t offset)
{
	if (!bReady)
		return -1;
	
	int ret = -1;
#if defined(USBMSC_SDIO_SD) || defined(USBMSC_SPI_SD)
	if (getFileHandle(id) == NULL)
		return -1;
	ret = f_lseek(getFileHandle(id), offset);
#else
//	lock();
//	if (id == eFI_GPS && fGps != NULL)
//		ret = fseek(fGps, offset) ? 0 : (-1);
//	else if (id == eFI_IMU && fImu != NULL)
//		ret = fseek(fImu, offset) ? 0 : (-1);
//	unlock();
#endif

	return ret;
}

int USB_DISK::read(eFILE_ID id, uint8_t *buffer, uint32_t size)
{
	if (!bReady)
		return -1;
	
	int ret = -1;
#if defined(USBMSC_SDIO_SD)
	UINT rdCount = 0;
	if (getFileHandle(id) == NULL)
		return -1;
	ret = f_read(getFileHandle(id), buffer, size, &rdCount);
#else
	lock();
	if (id == eFI_GPS && fGps != NULL)
		ret = fread(buffer, size, 1, fGps) ? 0 : (-1);
	else if (id == eFI_IMU && fImu != NULL)
		ret = fread(buffer, size, 1, fImu) ? 0 : (-1);
	unlock();
#endif

	return ret;
}

int USB_DISK::write(eFILE_ID id, uint8_t *buffer, uint32_t size)
{
	if (!bReady)
		return -1;
	
	int ret = -1;
#if defined(USBMSC_SDIO_SD)
	UINT wrCount = 0;
	if (getFileHandle(id) == NULL)
		return -1;
	DBGMSG(0, "[FS] %d Start-->", id);
	ret = f_write(getFileHandle(id), buffer, size, &wrCount);
#else
	lock();
	if (id == eFI_GPS && fGps != NULL)
		ret = fwrite(buffer, size, 1, fGps) ? 0 : (-1);
	else if (id == eFI_IMU && fImu != NULL)
		ret = fwrite(buffer, size, 1, fImu) ? 0 : (-1);
	unlock();
#endif
	DBGMSG(0, "<--[FS] %d End\r\n", id);
	return ret;
}

int USB_DISK::write(eFILE_ID id, Pipe<char> *pipe, uint32_t length)
{
	int ret = -1;
	int total = length ? length : pipe->size();
	if (!total)
		return ret;
	DBGMSG(0, "[FS] %d Start-->", id);

	if (!length)
		length = total;
	
	char * buffs[3] = { NULL, }, *curBuff;
	int index = 0, count = length, offset = 0;
	count = pipe->getBuffer(buffs, count);
	curBuff = buffs[index];
	while (buffs[index] && index < 2)
	{
		UINT written;
		ret = f_write(getFileHandle(id), curBuff + offset, count, &written);
		if (written != count)
		{
			offset = written;
			count -= written;
			total -= written;
			wait_ms(1);
			continue;
		}
		index++;
		if (index < 2 && buffs[index]) {
			curBuff = buffs[index];
			offset = 0;
			total -= count;
			count = total;
		}
	}
	DBGMSG(0, "<--[FS] %d End\r\n", id);

	return ret;
}
void USB_DISK::terminateFileSave()
{
	if (gUsbDisk.isConnected())
		return;
	closeFiles();
	gUsbDisk.unmountFileSystem();
	steps = eUSBDISK_INVALID;
}

uint8_t *USB_DISK::cdc_getRxBuffer(void)
{
	return CDC_GetRxBuffer();
}
void USB_DISK::cdc_flushRx(int size)
{
	CDC_FlushRx(size);
}
int USB_DISK::cdc_getRxSize(void)
{
	return CDC_RxDataSize();
}
bool USB_DISK::isCDC_readable()
{
	return CDC_RxDataSize() ? true : false;
}
bool USB_DISK::isCDC_writable()
{
	return CDC_GetTxState() ? false : true;
}
char USB_DISK::cdc_getc()
{
	return CDC_GetC();
}
void USB_DISK::cdc_putc(char data)
{
	CDC_PutC(data);
}
int USB_DISK::cdc_put(uint8_t* data, uint16_t len)
{
	return CDC_Put(data, len);
}
int USB_DISK::cdc_put(Pipe<char>* pipe, uint16_t len)
{
	int total = len ? len : pipe->size();
	if (!total)
		return 0;
	
	if (!len)
		len = total;

	char * buffs[3] = { NULL, };
	uint8_t *curBuff;
	int index = 0, count = len, offset = 0;
	count = pipe->getBuffer(buffs, count);
	curBuff = (uint8_t*)buffs[index];
	while (buffs[index] && index < 2)
	{
		int written = CDC_Put(curBuff + offset, count);
		if (written != count)
		{
			offset = written;
			count -= written;
			total -= written;
			wait_ms(1);
			continue;
		}
		index++;
		if (index < 2 && buffs[index]) {
			curBuff = (uint8_t*)buffs[index];
			offset = 0;
			total -= count;
			count = total;
		}
	}

	return (len - total);
}
#endif
