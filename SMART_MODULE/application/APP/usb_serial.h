#if !defined(USB_SERIAL_H)// && !defined(__TEST_USBMSD_SD__) && !defined(__TEST_USBMSD_NOR__) && !defined(__TEST_USBFILESYSTEM_SD) && !defined(__TEST_SD_FILESYSTEM) && !defined(__TEST_RAMDISK)
#define USB_SERIAL_H

#include <USBSerial.h>

class USB_SERIAL
{
public:
	#define USBD_VID                        0x0483
	#define USBD_PID                        0x5740

    /** Constructor
        \param tx the trasmitting pin
        \param rx the receiving pin
        \param baud the initial baudrate 
        \param rxSize the size of the receiving buffer
        \param txSize the size of the transmitting buffer
    */
	USB_SERIAL(uint16_t vendor_id, uint16_t product_id);
	/** Destructor
	*/
	virtual ~USB_SERIAL(void);
    
	USBSerial* getInf()	{	return &_USBSerial;	}
private:
	USBSerial _USBSerial;
	Thread usbserial_thread;
};

extern USB_SERIAL	gUsbSerial;
#endif	/* !defined(USB_SERIAL_H) */