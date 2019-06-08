#if !defined(__UWB_H) && !defined(__UWB_TEST_MODE)
#define __UWB_H

#include "UWB/UWBSpi.h"

class UWB : public UWBSpi
{
public:
	//<SVN>/mBed/SmartModule/Documents/DevelRef/FromGeoPlan/Ubisense Tag Module Accessory Communication Protocol - v1 2.pdf
	//	ID		Version		Flags	Reserved	Data	Reserved
	//	4bytes	2bytes		1byte		3bytes		1byte	17bytes
	typedef struct __attribute__((packed))
	{
		uint32_t	id;
		uint16_t	ver;
		uint8_t		flags;
		uint8_t		rsvd1[3];
		uint8_t		data;
		uint8_t		rsvd2[17];
	} uwb_out_t;
	
public:
	
	UWB(PinName mosi = UWB_MOSI, PinName miso = UWB_MISO, PinName clk = UWB_SCK, PinName cs = UWB_CS, PinName rst = UWB_RST, char *rxBuff = NULL, int rxSize = 256);
	void init();	
	bool bJustStarted;

	void resume() {	resume();	suspend_state = 0;	}
	void suspend() { suspend();	suspend_state = -1;	}
	void real_suspend()	{	suspend_state = 1;	}
	int getSuspend()	{	return suspend_state;	}
	bool bKeepRunning;
	void killProcess() {	bKeepRunning = false; }
	void parser(Pipe<char>* pipe);
	
	Thread::State getThreadState(){	return rx_thread.get_state();	}
private:
	Thread rx_thread;
	int suspend_state;
	DigitalInOut _rst;
	DigitalIn *pStatus;
};

extern UWB *gpUwb;
#endif /* __UWB_H	*/