#if defined(USE_USBSERIAL)
#include "common.h"

#if !defined(__USE_USB_SERIAL_DEBUG) || defined(__GPS_TEST_MODE)
USB_SERIAL	gUsbSerial(USBD_VID, USBD_PID);
#endif

static void usbserial_process();

#if defined(__WIZFI_TEST_MODE) 
UARTSerial gTargetUart(WIZNET_TX, WIZNET_RX, MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE);
DigitalIn gTargetCts(WIZNET_CTS);
DigitalOut gTargetRts(WIZNET_RTS,LOW);
DigitalOut gTargetReset(WIZNET_RST_, HIGH);
DigitalOut gTargetFunction(WIZNET_FUNC_, HIGH);
#elif defined(__GPS_TEST_MODE)
UARTSerial gTargetUart(GPS_TX, GPS_RX, 9600);
#endif

USB_SERIAL::USB_SERIAL(uint16_t vendor_id, uint16_t product_id)
	: _USBSerial(vendor_id, product_id, 1, false)
{
	usbserial_thread.start(usbserial_process);
}

USB_SERIAL::~USB_SERIAL()
{
}

static void usbserial_process()
{
#if defined(__WIZFI_TEST_MODE)
	wait_ms(200);
	gTargetReset = LOW;
	wait_ms(200);
	gTargetReset = HIGH;
	wait_ms(3000);
//	//Entering Function mode	
//	gTargetFunction = LOW;
//	wait_ms(150);
//	gTargetFunction = HIGH;
//	wait_ms(150);
//	//Now, It's in AP mode
//	gTargetFunction = LOW;
//	wait_ms(150);
//	gTargetFunction = HIGH;
//	wait_ms(150);
	//Now, It's in OTA mode
//	gTargetFunction = LOW;
//	wait_ms(150);
//	gTargetFunction = HIGH;
//	wait_ms(150);
	//Now, It's going to Factory Default settings
#elif defined(__GPS_TEST_MODE)
	char *pTxB, txBuff[512] = { 0, };
	pTxB = &txBuff[0];
#endif	
	for (;;)
	{
		while (!gUsbSerial.getInf()->configured())
			;
		#if defined(__WIZFI_TEST_MODE) || defined(__GPS_TEST_MODE)
		while (gUsbSerial.getInf()->readable())
		{
			char ch;
			if (gTargetUart.writable())
			{
				ch = gUsbSerial.getInf()->getc();
				gTargetUart.write(&ch, 1);
				*pTxB++ = ch;
			}
		}
		while (gTargetUart.readable())
		{
			char ch;
			if (gTargetUart.read(&ch, 1) <= 0)
				continue;
			if (gUsbSerial.getInf()->writeable())
				gUsbSerial.getInf()->](ch);
		}
		#else
 
		uint8_t	*unArray; 

		if (gUsbSerial.getInf()->readable())
		{
			int			nReadIndex;
			int			nCheckSumIndex;
			uint8_t		szCheckSum[1] = {0,};
		
			for (nReadIndex = 0; gUsbSerial.getInf()->readable(); nReadIndex++)
			{
				char ch;
				ch = gUsbSerial.getInf()->getc();
				unArray[nReadIndex] = (int)strtol(&ch, NULL, 16);	
			}
			
			for (nCheckSumIndex = 0; nCheckSumIndex < nReadIndex; nCheckSumIndex++)
			{
				szCheckSum[0] ^= unArray[nCheckSumIndex];
			}
			
			unArray[nReadIndex] = szCheckSum[0];
	
			if (unArray[0] == 0x04)
			{
				unArray[1] = 0xff&(unArray[1] << 4);
				unArray[1] = unArray[1] | unArray[2];
				
				for (int i = 2; i < nReadIndex - 1; i++)
				{
					unArray[i] = unArray[i + 1];
				}
				pgUryu->put(unArray, nReadIndex);
			}
			else
				pgUryu->put(unArray, nReadIndex + 1);	
		}
			
		
		Pipe<char> *pipe = pgUryu->getRxPipe();
		
		if (pgUryu->readable())
		{
			char ch = pipe->getc();
			gUsbSerial.getInf()->putc(ch);
	    }
	
		#endif
	}
}
#endif /*  */