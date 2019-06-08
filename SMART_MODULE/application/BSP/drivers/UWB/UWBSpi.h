#if !defined(__UWBSPI_H__)
#define __UWBSPI_H__

#include "mbed.h"
#include "Pipe.h"

#define SPI_FREQ_LOW	500000
#define SPI_FREQ_HIGH	21000000

class UWBSpi : public SPISlave
{
public:
	enum
	{
		eLED_GRN_ON = 1,
		eLED_RED_ON = 2		
	};
public:
    /** Constructor
    */
	UWBSpi(PinName mosi, PinName miso, PinName clk, PinName nss, char *rxBuff  = NULL, int rxSize    = 256);

	//! Destructor
	virtual ~UWBSpi(void);
    
	virtual bool init(PinName req = NC);
	Pipe<char>	*getPipe()	{	return &_rxPipe;	}
	bool isReadable()	{	return !_rxPipe.empty();	}
	char* getBuffer()	{	return _rxPipe.getBuffer(); }
	int	getSize()		{	return _rxPipe.size(); }
	
	int pulseCount;
	BusOut	*_led;
private:
	Pipe<char>	_rxPipe;
	InterruptIn _ext_irq;
};
extern UWBSpi *pgUwbSpi;
#endif	//__UWBSPI_H__