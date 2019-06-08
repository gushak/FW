#ifndef USB_DISK_H
#define USB_DISK_H


#include "stm32f4xx_hal.h"

#include "STM32CubeUSBFS/USBFS/Core/usbd_def.h"
#include "STM32CubeUSBFS/USBFS/Core/usbd_core.h"
#include "STM32CubeUSBFS/USBFS/usbd_desc.h"
#include "STM32CubeUSBFS/USBFS/Class/MSC/usbd_msc.h"
#include "STM32CubeUSBFS/USBFS/usbd_storage.h"
#include "STM32CubeUSBFS/USBFS/usbd_cdc_if.h"
//#include "STM32CubeUSBFS/USBFS/Class/DFU/usbd_dfu.h"
//#include "STM32CubeUSBFS/USBFS/usbd_dfu_flash.h"
#include "STM32CubeUSBFS/USBFS/Class/CDC/usbd_cdc.h"
#include "STM32CubeUSBFS/USBFS/Class/usbd_msc_cdc.h"


#include "STM32CubeUSBFS/SPIFFileSystem.h"
#include "STM32CubeUSBFS/BlockDevice/SPIFBlockDevice.h"
#include "Pipe.h"

class USB_DISK
{
public:
	#define	SPI_PINS		SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS
	#define SPI_CLOCK_HZ    ( 16 * 1000000 )
	typedef enum
	{
		eFI_IMU,
		eFI_UHF,
		eFI_GPS_NMEA,
		eFI_GPS_RAW,	//UBX or RT17
		eFI_GPS_RTCM,
		eFI_GPS_CMR,
		eFI_GPS_BINEX,
		eFI_COUNT
	} eFILE_ID;

	USB_DISK(char* name, PinName mosi, PinName miso, PinName sclk, PinName cs, int freq);
	
	int activateUSB();
	int deactivateUSB();
	
	void return_error(int ret_val);
	void errno_error(void* ret_val);
	int openFile(eFILE_ID id, char *pDevName);
	void closeFile(eFILE_ID id);
	bool isAllFilesOpen();
	FIL *getFileHandle(eFILE_ID id)	{	return pFH[id];}
	void setFileHandle(eFILE_ID id, FIL *f)	{	pFH[id] = f; }
	int mount() {	return _fs.mount(&_bd);	}

	bool bKeepRunning;
	void killProcess() {	bKeepRunning = false;	}
	int seek(eFILE_ID id, uint32_t offset);
	int read(eFILE_ID id, uint8_t *buffer, uint32_t size);
	int write(eFILE_ID id, uint8_t *buffer, uint32_t size);
	int write(eFILE_ID id, Pipe<char> *pipe, uint32_t length);
	
	int formatFileSystem();
	int openFiles();
	int closeFiles();
	void terminateFileSave();
	int mountFileSystem();
	int unmountFileSystem();
	int isConnected() { 
		if (pVBus != NULL)
			return *pVBus;
		return USBD_Device.dev_connection_status?1:0;
	}
	bool isConfigured() { return (USBD_Device.dev_state == USBD_STATE_CONFIGURED);}
	int bkConnStat;
	void setReady() {	bReady = true;	}
	void clrReady() {	bReady = false; }
	bool isReady()	{	return bReady;	}
	
	void lock() {	_mutex.lock(); }
	void unlock() {	_mutex.unlock(); }
	
	Thread::State getThreadState(){	return usbdisk_thread.get_state();	}
	typedef enum {
		eUSBDISK_LOCAL_DEMOUNT,
		eUSBDISK_REMOTE_MOUNT,
		eUSBDISK_LOCAL_MOUNT,
		eUSBDISK_LOCAL_TIME,
		eUSBDISK_LOCAL_FORMAT,
		eUSBDISK_LOCAL_FS,
		eUSBDISK_IDLE,
		eUSBDISK_INVALID
	} eUSBDISK_STEPS;
	eUSBDISK_STEPS steps, pending_steps;
	
	uint8_t *cdc_getRxBuffer(void);
	void cdc_flushRx(int size);
	int cdc_getRxSize(void);
	bool isCDC_readable();
	bool isCDC_writable();
	char cdc_getc();
	void cdc_putc(char data);
	int cdc_put(uint8_t* data, uint16_t len);
	int cdc_put(Pipe<char>* pipe, uint16_t len);
	USBD_HandleTypeDef *getInf() {	return &USBD_Device;	}

	SPIFFileSystem *getFS() {	return &_fs;	}
	SPIFBlockDevice *getBD() {	return &_bd;	}
private:
	SPIFFileSystem _fs;
	SPIFBlockDevice _bd;
	FIL *pFH[2];
	Thread usbdisk_thread;
	FIL FileHandle[eFI_COUNT], *pFH[eFI_COUNT];
	USBD_HandleTypeDef USBD_Device;
	Mutex _mutex;

	bool bReady;
	DigitalIn * pVBus;
};

extern USB_DISK gUsbDisk;
#endif