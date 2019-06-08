#if !defined(__EXT_FLASH_H) && !defined(__EXT_FLASH_TEST_MODE)
#define __EXT_FLASH_H
#include "mbed.h"
#include "SPI_NOR.h"

class EXT_FLASH : public SPI_NOR
{
public:
	#define EF_TOOL_DATA_SIZE		32
	#define EF_TOOL_DATA_PER_PAGE	(PAGE_SIZE/EF_TOOL_DATA_SIZE)	//256/32 => 8
	#define EF_TOOL_DATA_PER_4KSECTOR (EF_TOOL_DATA_PER_PAGE*(SECTOR_SIZE/PAGE_SIZE))	//8*(4096/256) => 128
	#define EF_COUNT_MAX			(1024*1024/EF_TOOL_DATA_SIZE)
	
	#define	EF_PINS		EF_MOSI, EF_MISO, EF_SCK, EF_CS
	#define EF_CLOCK_HZ    ( 24 * 1000000 )
	typedef enum
	{
		eFI_GPS,
		eFI_IMU
	} eFILE_ID;

	EXT_FLASH(char* name, PinName mosi, PinName miso, PinName sclk, PinName cs, int freq) ;
	virtual ~EXT_FLASH();
	
	void mutex_init()	{	_mutex = new Mutex; }
	void lock() {	_mutex->lock(); }
	void unlock() {	_mutex->unlock(); }
	void mutex_reset() {	delete _mutex; mutex_init(); }

	int write_tool_data(uint8_t *buffer);
	int read_tool_data(uint8_t *buffer);
	int flush_tool_data();
private:
	Thread flash_thread;
	Mutex *_mutex;
};

extern EXT_FLASH *pgExtFlash;
#endif /* __EXT_FLASH_H	*/