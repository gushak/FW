#ifndef _MAIN_H_
#define _MAIN_H_
#include <stdbool.h> 
#include "STM32CubeUSBFS/USBFS/Class/CDC/usbd_cdc.h"

#ifdef __cplusplus
extern "C" {
#endif


extern bool DevKeepRunning;
void killSystem();
void _CloseDevice();
void PowerOff(uint32_t opr);
void FactoryMode(uint32_t opr);
void ReStart();
void setUSBProcessStep(bool bMount);
bool isUSBconnected();
void resumeGPS();
void suspendGPS();
void openPCIF();
void closePCIF();

void JumpToBootloader(void);
void setDirectDevice(USBD_CDC_LineCodingTypeDef * pLineCoding);	
void sendDirectDevice(uint8_t *buff, int len);
int readableDirectDevice();
int receiveDirectDevice(uint8_t *buff);

typedef enum
{
	eDOM_PCIF	= 0x1,	// PC Interface mode
	eDOM_DEBUG	= 0x2,	// MCU serial debug mode
	eDOM_MCU    = 0x4,	// MCU Register debugger mode
	eDOM_GPS    = 0x8,	// GPS pass-through mode
	eDOM_DIRECT_DEVICE   = 0x8000	// UART direct mode
} eDIRECT_OPR_MODE;
#define DOM_UART1	(0x8000 | (0 << 8))
#define DOM_UART2	(0x8000 | (1 << 8))
#define DOM_UART3	(0x8000 | (2 << 8))
#define DOM_UART4	(0x8000 | (3 << 8))
#define DOM_UART5	(0x8000 | (4 << 8))
#define DOM_UART6	(0x8000 | (0x10 << 8))
#define DOM_I2C1	(0x8000 | (0x11 << 8))
#define DOM_I2C2	(0x8000 | (0x12 << 8))
#define DOM_I2C3	(0x8000 | (0x13 << 8))
#define DOM_SPI1	(0x8000 | (0x14 << 8))
#define DOM_SPI2	(0x8000 | (0x20 << 8))
#define DOM_SPI3	(0x8000 | (0x21 << 8))
#define DOM_CAN1	(0x8000 | (0x22 << 8))
#define DOM_CAN2	(0x8000 | (0x23 << 8))
#define DOM_SDIO	(0x8000 | (0x24 << 8))
	
extern volatile uint32_t BitOpMode;

typedef enum
{
	ePOS_KEY,	// Power-On by Power Key
	ePOS_USB,	// Power-On by USB hook-up
	ePOS_PEN,	// Power-On by External PEN signal
	ePOS_COUNT
} ePWRON_SOURCE;
	
#ifdef __cplusplus
}
#endif

#endif /* _MAIN_H_ */