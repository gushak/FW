#include "mbed.h"
#include <ctype.h>
#include "UWB/UWBSpi.h"

UWBSpi *pgUwbSpi = NULL;
static void rx_handler();
Timeout	_t;
// ----------------------------------------------------------------
// Serial Implementation 
// ----------------------------------------------------------------
UWBSpi::UWBSpi(PinName mosi,	PinName miso, PinName clk, PinName nss, char *rxBuff, int rxSize)
					: SPISlave(mosi, miso, clk, nss), _rxPipe(rxSize, rxBuff), _ext_irq(UWB_ERROR)
{
	pgUwbSpi = this;
}

UWBSpi::~UWBSpi(void)
{
	disable_interrupt();
	_ext_irq.rise(NULL);
}

static void rx_handler()
{
	// For the detail UWB protocol, 
	// Refer to <svn>/mbed/SmartModule/Documents/DevelRef/FromGeoPlan/Ubisense Tag Module Accessory Communication Protocol - v1 2.pdf
	pgUwbSpi->getPipe()->putc(pgUwbSpi->read());
}
static void process_error()
{
	*pgUwbSpi->_led = pgUwbSpi->eLED_RED_ON;
}
static void process_normal()
{
	if (pgUwbSpi->pulseCount > 100)
	{
		*pgUwbSpi->_led = pgUwbSpi->eLED_GRN_ON;
	}
	pgUwbSpi->pulseCount = 0;
	_t.attach(callback(process_error), 90);
}
static void pulse_handler()
{
	if (!pgUwbSpi->pulseCount)
		_t.attach(callback(process_normal), .5f);
	
	pgUwbSpi->pulseCount++;
}

bool UWBSpi::init(PinName req)
{
	//format(8, 0);
	interrupt_handler((uint32_t)rx_handler);
	_ext_irq.rise(&pulse_handler);
	pulseCount = 0;
	_led = new BusOut(LED_GRN, LED_RED);
	*_led = eLED_GRN_ON;
	return true;
}