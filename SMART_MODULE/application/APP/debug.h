#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DISABLE_DBUG_MSG

#if defined(__USE_USB_SERIAL_DEBUG) && !defined(__TEST_USBMSD_SD__) && !defined(__TEST_USBMSD_NOR__)
#include "USBSerial.h"

#elif defined(__USE_BT_SERIAL_DEBUG)
#include "BluetoothSerial.h"

#elif defined(USE_MSC_CDC_SERIAL_DEBUG)
#include "STM32CubeUSBFS/USBFS/usbd_cdc_if.h"
#include "main.h"
#else
# define __USE_UART_SERIAL

#endif

class LOCAL_DEBUG
				#if defined(__USE_USB_SERIAL_DEBUG)
					: public USBSerial
				#elif defined(__USE_BT_SERIAL_DEBUG)
					: public BluetoothSerial
				#elif defined(__USE_UART_SERIAL)
					: public UARTSerial
				#elif defined(USE_MSC_CDC_SERIAL_DEBUG)
					
				#else
					: public Serial
				#endif
{
public:
	#define MAX_PRINTF_BUFFER			512
#ifndef u8
	typedef unsigned char	u8;
	typedef signed char 	s8;
	typedef unsigned short 	u16;
	typedef signed short 	s16;
	typedef unsigned long 	u32;
	typedef unsigned long long 	u64;
	typedef signed long 	s32;
	typedef signed char 	int8;
	typedef signed short 	int16;
	typedef signed long 	int32;
	typedef signed long long 	int64;
	typedef u8				BOOL;
#endif
	//
	// Debug Exported Variable
	//
	bool g_isDebugEnable = true;
	u8 g_ucDebugPort = 0;
	u8 g_ucDebugLevel = 0;
	u8 g_ucDebugEcho = 1;
#if defined(PASS_THROUGH_UART3_TO_DEBUG)
	u8 g_ucDebugForwardPort = DEV_UART3;
#elif defined(PASS_THROUGH_UART2_TO_DEBUG)
	u8 g_ucDebugForwardPort = DEV_UART2;
#else
	u8 g_ucDebugForwardPort = 0xff;
#endif
	
public:	
#if defined(__USE_USB_SERIAL_DEBUG)
	LOCAL_DEBUG(uint16_t vendor_id, uint16_t product_id);
#elif defined(__USE_BT_SERIAL_DEBUG)
	LOCAL_DEBUG(PinName tx, PinName rx);
#else
	LOCAL_DEBUG(PinName tx, PinName rx, char* name = (char*)"DEBUG");
#endif
	virtual ~LOCAL_DEBUG();

#if defined(USE_MSC_CDC_SERIAL_DEBUG)
	ssize_t write(const char * buffer, size_t length)
	{
		return CDC_Put((uint8_t*)buffer, length);
	}
	bool writable()
	{
		return CDC_GetTxState()==1 ? false : true;
	}
	int putc(int c)
	{
		return CDC_PutC(c);
	}
	char getc()
	{
		return CDC_GetC();
	}
#endif
	void puth(u8 c);
	void DebugEnable(bool isEnable);

	void DebugTimeStamp(u8 ucLevel);
	void DebugString(u8 ucLevel, const char* str);
	void DebugAscii(u8 ucLevel, const char* str);
	void DebugPrintf(u8 ucLevel, const char *fmt, ...);
	void DebugPrintfT(u8 ucLevel, const char *fmt, ...);
	void DebugDump(u8 ucLevel, const char *desc, u8* buffer, u32 size);
	void DebugDumpT(u8 ucLevel, const char *desc, u8* buffer, u32 size);
	void DebugDumpEx(u8 ucLevel, const char *desc, u8* buffer, u32 size, u8 col, u8 line);
	void DebugDumpAscii(u8 ucLevel, const char *desc, u8* buffer, u32 size);
};

extern LOCAL_DEBUG *pgDebug;
//
// Debug Definition
//
#if !defined(DISABLE_DBUG_MSG) && !defined(PASS_THROUGH_UART3_TO_DEBUG) && !defined(PASS_THROUGH_UART2_TO_DEBUG)
#define DBGSTR(c,m)						pgDebug->DebugString(c, m)
#define DBGASC(c,m) 					pgDebug->DebugAscii(c, m)
#define DBGMSG(c,m,...)					pgDebug->DebugPrintf(c, m, ##__VA_ARGS__)
#define DBGMSGD(c,m,...)				pgDebug->DebugDmaPrintf(c, m, ##__VA_ARGS__)
#define DBGDMP(c,d,b,s)					pgDebug->DebugDump(c,d,b,s)
#define DBGDMPE(c,d,b,s,co,ln)			pgDebug->DebugDumpEx(c,d,b,s,co,ln)
#define DBGDMPA(c,d,b,s)				pgDebug->DebugDumpAscii(c,d,b,s)
#define DBGTIME(c)						pgDebug->DebugTimeStamp(c)
#define DBGBHEX(c)						pgDebug->getInf().puth(c)

#elif defined(USE_MSC_CDC_SERIAL_DEBUG)
#define DBGSTR(c,m)						do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugString(c, m);	} while(0);
#define DBGASC(c,m) 					do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugAscii(c, m);	} while(0);
#define DBGMSG(c,m,...)					do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugPrintf(c, m, ##__VA_ARGS__);	} while(0);
#define DBGMSGD(c,m,...)				do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugDmaPrintf(c, m, ##__VA_ARGS__);	} while(0);
#define DBGDMP(c,d,b,s)					do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugDump(c,d,b,s);	} while(0);
#define DBGDMPE(c,d,b,s,co,ln)			do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugDumpEx(c,d,b,s,co,ln);	} while(0);
#define DBGDMPA(c,d,b,s)				do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugDumpAscii(c,d,b,s);	} while(0);
#define DBGTIME(c)						do { if (BitOpMode & eDOM_DEBUG) pgDebug->DebugTimeStamp(c);	} while(0);
#define DBGBHEX(c)						do { if (BitOpMode & eDOM_DEBUG) pgDebug->getInf().puth(c);	} while(0);

#else
#define DBGSTR(c,m)
#define DBGASC(c,m)
#define DBGMSG(c,m...)
#define DBGMSGD(c,m...)
#define DBGDMP(c,d,b,s)
#define DBGDMPE(c,d,b,s,co,ln)
#define DBGDMPA(c,d,b,s)
#define DBGTIME(c)
#define DBGBHEX(c)
#endif

#endif /* __DEBUG_H__ */