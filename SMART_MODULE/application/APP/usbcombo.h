#ifndef _USBCOMBO_H_
#define _USBCOMBO_H_

/* FatFs includes component */
#include "FatFs/ff_gen_drv.h"
//#include "sd_diskio.h"

#include "USBFS/Core/usbd_core.h"
#include "USBFS/usbd_desc.h"
#include "USBFS/Class/MSC/usbd_msc.h"
#include "USBFS/usbd_storage.h"
#include "USBFS/Class/CDC/usbd_cdc.h" 
#include "USBFS/usbd_cdc_interface.h"

class USBCOMBO
{
public:
	USBCOMBO()
	{
	}
	void USBMSC_Init();
	void USBMSC_DeInit();
	void USBCDC_Init();
	void USBCDC_DeInit();

	bool isConnected() {	return (USBD_Device.dev_state == USBD_STATE_CONFIGURED);}
	bool isCDCConf() {	return bCDC; }
private:
	FATFS SDFatFs;   /* File system object for SD card logical drive */
	FIL MyFile;      /* File object */
	char SDPath[4];  /* SD card logical drive path */

	USBD_HandleTypeDef USBD_Device;
	bool bCDC;
};
#endif /* _USBCOMBO_H_ */