#include "common.h"

LOCAL_DEBUG *pgDebug;

#if defined(__USE_USB_SERIAL_DEBUG) && defined(USE_USBSERIAL)
LOCAL_DEBUG	gDebug(USBD_VID, USBD_PID);
LOCAL_DEBUG::LOCAL_DEBUG(uint16_t vendor_id, uint16_t product_id)
	: USBSerial(vendor_id, product_id, 1, false)
{
	pgDebug = this;
	g_ucDebugPort = 0xff;
	g_ucDebugLevel = 0;
	g_ucDebugEcho  = 1;
}

#elif defined(__USE_BT_SERIAL_DEBUG) && __USE_BT_SERIAL_DEBUG

LOCAL_DEBUG	gDebug(BT_TX, BT_RX);
LOCAL_DEBUG::LOCAL_DEBUG(PinName tx, PinName rx)
	: BluetoothSerial(tx, rx)
{
	g_ucDebugPort = 0xff;
	g_ucDebugLevel = 0;
	g_ucDebugEcho  = 1;
}

#elif 1//defined(__USE_UART_SERIAL) && (SERIAL_RX!=NC)
LOCAL_DEBUG gDebug(SERIAL_TX, SERIAL_RX); 
LOCAL_DEBUG::LOCAL_DEBUG(PinName tx, PinName rx, char* name)
	: UARTSerial(tx, rx)
{
	pgDebug = this;
	g_ucDebugPort = getUartIndex();
	g_ucDebugLevel = 0;
	g_ucDebugEcho  = 1;
}

#elif defined(USE_MSC_CDC_SERIAL_DEBUG)
LOCAL_DEBUG gDebug(SERIAL_TX, SERIAL_RX); 
LOCAL_DEBUG::LOCAL_DEBUG(PinName tx, PinName rx, char* name)
{
	g_ucDebugPort = 0xff;
	g_ucDebugLevel = 0;
	g_ucDebugEcho  = 1;
}

#elif (SERIAL_RX != NC)
LOCAL_DEBUG gDebug(SERIAL_TX, SERIAL_RX); 
LOCAL_DEBUG::LOCAL_DEBUG(PinName tx, PinName rx, char* name)
	: Serial(tx, rx)
{
	g_ucDebugPort = getUartIndex();
	g_ucDebugLevel = 0;
	g_ucDebugEcho  = 1;
}
#endif

LOCAL_DEBUG::~LOCAL_DEBUG()
{
}

void LOCAL_DEBUG::puth(u8 c)
{
	unsigned char nibble[2] = { (uint8_t)((c & 0xf0) >> 4), (uint8_t)(c & 0xf) };
	nibble[0] = (nibble[0] >= 10) ? (nibble[0] + 'A' - 10) : (nibble[0] + '0');
	nibble[1] = (nibble[1] >= 10) ? (nibble[1] + 'A' - 10) : (nibble[1] + '0');
	write((const char*)nibble, 2);
}

void LOCAL_DEBUG::DebugEnable(bool isEnable)
{
	g_isDebugEnable = isEnable;
}

void LOCAL_DEBUG::DebugTimeStamp(u8 ucLevel)
{
	DATETIME datetime;
	char sTimeStamp[21] = { 0, };

	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;

//#if DEVICE_RTC
//	RtcGetDateTime((PRTC_DATE_TIME)&datetime);
//#endif
	if (datetime.year >= 2016)
	{
		sprintf(sTimeStamp, "[%02d:%02d:%02d] ", datetime.hour, datetime.minute, datetime.second);
		write(sTimeStamp, strlen(sTimeStamp));
	}
}

void LOCAL_DEBUG::DebugString(u8 ucLevel, const char* str)
{
	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;

	while (*str) putc(*str++);
}

void LOCAL_DEBUG::DebugAscii(u8 ucLevel, const char* str)
{
	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;

#if 0
	while (isascii(*str)) UartWriteByte(g_ucDebugPort, *str++);
#else	
	while (*str) 
	{
		if ((*str >= 0x20) && (*str <= 0x7E))
		{
			putc(*str);
		}
		else
		{
			putc('<');
			puth(*str);
			putc('>');
		}
		str++;
	}
#endif	
}

void LOCAL_DEBUG::DebugPrintf(u8 ucLevel, const char *fmt, ...)
{
	va_list ap;
	u8 buffer[MAX_PRINTF_BUFFER] = { 0, };
	size_t len;

	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;
	if (fmt == NULL) return;

	while (!writable())
		wait(0.1);

	va_start(ap, fmt);
	len = vsnprintf((char *)buffer, MAX_PRINTF_BUFFER, fmt, ap);
	va_end(ap);

	write((const char*)buffer, len);
}

void LOCAL_DEBUG::DebugPrintfT(u8 ucLevel, const char *fmt, ...)
{
	va_list ap;
	u8 buffer[MAX_PRINTF_BUFFER] = { 0, };
	size_t len;

	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;
	if (fmt == NULL) return;

	while (!writable())
		wait(0.1);

	va_start(ap, fmt);
	len = vsnprintf((char *)buffer, MAX_PRINTF_BUFFER, fmt, ap);
	va_end(ap);

	DebugTimeStamp(ucLevel);
	write((const char*)buffer, len);
}

void LOCAL_DEBUG::DebugDump(u8 ucLevel, const char *desc, u8* buffer, u32 size)
{
	u32 i;

	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;
	if (buffer == NULL) return;

	while (!writable())
		wait(0.1);

	if (desc)
	{
		write((char*)desc, sizeof(desc));
	}

	for (i = 0; i < size; i++)
	{
		puth(buffer[i]);

		if (((i + 1) % 4) == 0) putc(' ');
		if (((i + 1) % 16) == 0) 
		{
			putc('\r');
			putc('\n');
		}
	}
	putc('\r');
	putc('\n');
}

void LOCAL_DEBUG::DebugDumpT(u8 ucLevel, const char *desc, u8* buffer, u32 size)
{
	u32 i;

	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;
	if (buffer == NULL) return;

	while (!writable())
		wait(0.1);

	if (desc)
	{
		DebugTimeStamp(ucLevel);
		write(desc, sizeof(desc));
	}

	for (i = 0; i < size; i++)
	{
		puth(buffer[i]);

		if (((i + 1) % 4) == 0) putc(' ');
		if (((i + 1) % 16) == 0) 
		{
			putc('\r');
			putc('\n');
		}
	}
	putc('\r');
	putc('\n');
}

void LOCAL_DEBUG::DebugDumpEx(u8 ucLevel, const char *desc, u8* buffer, u32 size, u8 col, u8 line)
{
//	u8 nibble_h, nibble_l, asc;
	u32 i;

	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;
	if (buffer == NULL) return;

	while (!writable())
		wait(0.1);

	if (desc) write(desc, sizeof(desc));

	for (i = 0; i < size; i++)
	{
		puth(buffer[i]);
		
		if (((i + 1) % col) == 0) putc(' ');
		if (((i + 1) % line) == 0) 
		{
			putc('\r');
			putc('\n');
		}
	}
	putc('\r');
	putc('\n');
}

void LOCAL_DEBUG::DebugDumpAscii(u8 ucLevel, const char *desc, u8* buffer, u32 size)
{
	if (!g_isDebugEnable) return;
	if (g_ucDebugPort == 0) return;
	if (ucLevel > g_ucDebugLevel) return;
	if (buffer == NULL) return;

	while (!writable())
		wait(0.1);

	if (desc) write(desc, sizeof(desc));

	write((const char*)buffer, size);

	putc('\r');
	putc('\n');
}

static void debug_rx_process()
{
	while (true)
	{
		char data = gDebug.getc();
	#if defined(__ENABLE_GPS_CONSOLE_CONTROL)
		if (gGps.console_control(data) > 0)
			continue;
	#endif

	}

}