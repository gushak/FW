#if !defined(__TPEG_TEST_MODE)
#include "common.h"
#include "uwb.h"

__ALIGN_BEGIN char uwbBuff[1024] __ALIGN_END = "";
UWB gUWB(UWB_MOSI, UWB_MISO, UWB_SCK, UWB_CS, UWB_RST, uwbBuff, sizeof(uwbBuff));
UWB *pgUWB = NULL;
static void uwb_rx_process();

UWB::UWB(PinName mosi, PinName miso, PinName clk, PinName cs, PinName rst, char *rxBuff, int rxSize)
	: UWBSpi(mosi, miso, clk, cs, rxBuff, rxSize)
	, _rst(rst, PIN_OUTPUT, OpenDrainNoPull, LOW)
{
	pgUWB = this;
	rx_thread.start(uwb_rx_process);
}
void UWB::init()
{
	wait_ms(200);
	_rst.input();
	wait_ms(100);
	
	pStatus = new DigitalIn(UWB_STATUS);
	new DigitalOut(UWB_KEY_IN,LOW);
	
	suspend_state = 0;
	UWBSpi::init();
}
void uwb_rx_process()
{
	pgUWB->bKeepRunning = true;
	pgUWB->init();
	wait_ms(100);
	
	int ret = 0;
	while (pgUWB->bKeepRunning)
	{
		if (pgUWB->getSuspend() > 0)
		{
			if (pgUWB->getSuspend() < 0)
				pgUWB->real_suspend();
			wait(1);
			continue;
		}
		if (pgUWB->getSuspend() < 0)
			pgUWB->suspend();

		if (pgUWB->getSize() >= sizeof(UWB::uwb_out_t))
		{
			//DBGDMP(0, "[UWB]\r\n", pgUWB->getDataBuffer(), pgUWB->getSize());
			pgUWB->parser(pgUWB->getPipe());
			break;
		}
		wait_ms(10);
	}
	//DBGMSG(0, "[UWB] Terminate \r\n");
	pgUWB->suspend();
	delete pgUwbSpi;
}

void UWB::parser(Pipe<char>* pipe)
{
	uwb_out_t	uwb_data;
	pipe->get((char*)&uwb_data, sizeof(uwb_data));
	//TODO: implement action below
	//
	if (CFG(Setting).uwbId != uwb_data.id)
	{
		CFG(Setting).uwbId = uwb_data.id;
		pgConf->SaveConfig();
		//DBGMSG(0, "[UWB] ID = %08x\r\n", uwb_data.id);
	}
}
#endif /* !defined(__UWB_TEST_MODE) */