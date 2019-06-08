/* a copy of the SDFileSystem library...
 * but adapted for class 'USBSPIFileSystem'!
 *
 * WILL BE TESTED ONLY WITH 25LC1024(Microchip)
 */


#include "SPI_NOR.h"
#include "mbed_debug.h"

#if defined(__SPI_DMA__)
#include "spi_dma/spi_dma.h"
#endif

#if defined(__TEST_SPINOR__)
#include "debug.h"
#endif

#define SD_COMMAND_TIMEOUT 5000

#define SD_DBG             0

SPI_NOR::SPI_NOR(PinName mosi, PinName miso, PinName sclk, PinName cs, int hz)
	:  _spi(mosi, miso, sclk), _cs(cs),  _transfer_sck(hz) 
{
    _cs = 1;
    //no init
    _status = 0x01;
//    wait (1);
#if defined(__SPI_DMA__)
	_spi_id = spi_get_id(mosi, miso, sclk, NC);
	spi_dma_init(_spi_id);
#endif
}
/*
#define R1_IDLE_STATE           (1 << 0)
#define R1_ERASE_RESET          (1 << 1)
#define R1_ILLEGAL_COMMAND      (1 << 2)
#define R1_COM_CRC_ERROR        (1 << 3)
#define R1_ERASE_SEQUENCE_ERROR (1 << 4)
#define R1_ADDRESS_ERROR        (1 << 5)
#define R1_PARAMETER_ERROR      (1 << 6)

// Types
//  - v1.x Standard Capacity
//  - v2.x Standard Capacity
//  - v2.x High Capacity
//  - Not recognised as an SD Card
#define SDCARD_FAIL 0
#define SDCARD_V1   1
#define SDCARD_V2   2
#define SDCARD_V2HC 3
*/
int SPI_NOR::disk_initialize() {
	_spi.frequency(_transfer_sck); // Set to 24MHz for data transfer

	_sectors = TOTAL_V_SECTORS;

    // OK
    _status = 0x00;

    return 0;
}

int SPI_NOR::disk_write(uint8_t * buffer, uint64_t pages, uint8_t count = 1) {
	uint64_t _pages = pages << 8;


#if defined(MT25QL02GC)
	if (!count)
	{
		_cs = 0;
		_spi.write(WRITE_ENABLE);
		_cs = 1;
		_cs = 0;
		_spi.write(WRITE_VOL_CONF_REG);
		_spi.write(0xeb);
		_cs = 1;
		
		_cs = 0;
		_spi.write(CLEAR_FLAG_STATUS_REG);
		_cs = 1;
		
		return BD_ERROR_OK;
	}
#endif
	// send the data block
	for (int i = 0; i < count; i++) {
		_cs = 0;
	    _spi.write(WRITE_ENABLE);
		_cs = 1;
		_cs = 0;
		#if defined(ADDRESSING_4BYTES)
		_spi.write(PAGE_4PROGRAM); //PAGE_PROGRAM
		_spi.write(0xff & (_pages >> 24)); //MSB
		#else
		_spi.write(PAGE_PROGRAM); //PAGE_PROGRAM
		#endif
		_spi.write(0xff & (_pages >> 16)); //
		_spi.write(0xff & (_pages >> 8));	//
		_spi.write(0xff & (_pages));		//LSB
		#if defined( __SPI_DMA__)
		spi_dma_write(_spi_id, (uint8_t *)&buffer[PAGE_SIZE*i], &dummyByte, PAGE_SIZE);
		#else
        for(int j = (PAGE_SIZE*i); j < (PAGE_SIZE<<i); j++){
            _spi.write(buffer[j]);
        }
		#endif
		_cs = 1;
		if (disk_sync() != BD_ERROR_OK)
			return BD_ERROR_DEVICE_ERROR;
		_pages += PAGE_SIZE;
	}

	return BD_ERROR_OK;
}

int SPI_NOR::disk_read(uint8_t * buffer, uint64_t pages, uint8_t count = 1) {
	uint64_t _pages = pages << 8;

#if defined(MT25QL02GC)
	if (!count)
	{
		_cs = 0;
		// Use it to check the chip configuration.
		_spi.write(READ_STATUS_REG); // read Status Register
		buffer[0] = _spi.write(0xAA);
		_cs = 1;
		
		_cs = 0;
		_spi.write(READ_FLAG_STAT_REG); // read Flag Status Register
		buffer[1] = _spi.write(0xAA);	// default value : 0x80
		_cs = 1;

		_cs = 0;
		_spi.write(READ_NONVOL_CONF_REG); // read non-volatile configuration Register
		buffer[2] = _spi.write(0xAA);	// default value : 0xff
		buffer[3] = _spi.write(0xAA);	// default value : 0xff
		_cs = 1;

		_cs = 0;
		_spi.write(READ_VOL_CONF_REG); // read volatile configuration Register
		buffer[4] = _spi.write(0xAA);		// default value : 0xfb
		_cs = 1;

		_cs = 0;
		_spi.write(READ_ENH_VOL_CONF_REG); // read enhanced volatile configuration Register
		buffer[5] = _spi.write(0xAA);		// default value : 0xfF
		_cs = 1;
		
		_cs = 0;
		_spi.write(READ_EXT_ADDR_REG); // read extended address Register
		buffer[6] = _spi.write(0xAA); 
		_cs = 1;

		_cs = 0;
		_spi.write(READ_ID); // read extended address Register
		for(int j = 0; j < 20; j++)
			buffer[7+j] = _spi.write(0xAA); 
		_cs = 1;

		return BD_ERROR_OK;
	}
#endif

	for (int i = 0; i < count; i++) {
		_cs = 0;
		#if defined(ADDRESSING_4BYTES)
		_spi.write(READ_4DATA); // READ
		_spi.write(0xff & (_pages >> 24)); //MSB
		#else
		_spi.write(READ_DATA); // READ
		#endif
		_spi.write(0xff & (_pages >> 16));	//
		_spi.write(0xff & (_pages >> 8));	//
		_spi.write(0xff & (_pages));		//LSB
		#if defined( __SPI_DMA__)
		spi_dma_read(_spi_id, &dummyByte, &buffer[PAGE_SIZE*i], PAGE_SIZE);
		#else
        for(int j = (PAGE_SIZE*i); j < (PAGE_SIZE<<i); j++){
            buffer[j] = _spi.write(0xAA);
        }
		#endif
		_cs = 1;
		_pages += PAGE_SIZE;	// increase 1 page at once
	}

	return BD_ERROR_OK;
}

int SPI_NOR::disk_status() { return _status; }
int SPI_NOR::disk_sync() { 

	    // Read status register until write not-in-progress
		uint8_t status;
	for (int i = 0; i < SPIF_TIMEOUT; i++) {
		_cs = 0;
		// Use it to check the chip configuration.
		_spi.write(READ_STATUS_REG); // read Status Register
		status = _spi.write(0xAA);
		_cs = 1;
		// Check WIP bit
		if (!(status & SPIF_WIP)) {
			return BD_ERROR_OK;
		}
		wait_ms(1);
	}

	return BD_ERROR_DEVICE_ERROR;
}
uint64_t SPI_NOR::disk_sectors() { return _sectors; }

int SPI_NOR::disk_erase(uint64_t sector, uint8_t count=1) {
	uint64_t _sector = sector << 12;	//4K
	
	for (int i = 0; i < count; i++) { //0,1
		_cs = 0;
		_spi.write(WRITE_ENABLE);
		_cs = 1;
		_cs = 0;
		#if defined(ADDRESSING_4BYTES)
		_spi.write(SECTOR_4ERASE4K);
		_spi.write(0xff & (_sector >> 24)); //MSB
		#else
		_spi.write(SECTOR_ERASE4K);
		#endif
		_spi.write(0xff & (_sector >> 16)); //
		_spi.write(0xff & (_sector >> 8)); //
		_spi.write(0xff & (_sector)); //LSB
		_cs = 1;
		if (disk_sync() != BD_ERROR_OK)
			return BD_ERROR_DEVICE_ERROR;
		_sector += 4096;	// increase 4K at once.
	}

	return BD_ERROR_OK;
}

int SPI_NOR::disk_erase() {
#if defined(MT25QL02GC)
	_cs = 0;
	_spi.write(WRITE_ENABLE);
	_cs = 1;
	_cs = 0;
	_spi.write(WRITE_EXT_ADDR_REG);
	_spi.write(0);
	_cs = 1;
#endif
	_cs = 0;
	_spi.write(WRITE_ENABLE);
	_cs = 1;
	_cs = 0;
	_spi.write(DIE_ERASE);
	_cs = 1;

	if (disk_sync() != BD_ERROR_OK)
		return BD_ERROR_DEVICE_ERROR;

#if defined(MT25QL02GC)
	_cs = 0;
	_spi.write(WRITE_ENABLE);
	_cs = 1;
	_cs = 0;
	_spi.write(WRITE_EXT_ADDR_REG);
	_spi.write(1);
	_cs = 1;
	_cs = 0;
	_spi.write(WRITE_ENABLE);
	_cs = 1;
	_cs = 0;
	_spi.write(DIE_ERASE);
	_cs = 1;

	if (disk_sync() != BD_ERROR_OK)
		return BD_ERROR_DEVICE_ERROR;
#endif

	return BD_ERROR_OK;
}

# if defined(__TEST_SPINOR__)
void SPI_NOR::test_write_flash()
{
#if 1	
	memset((uint8_t *)mBuff, 0x08, sizeof(mBuff));
	disk_write((uint8_t *)mBuff, 0, 1);
#elif 1
	//Write configuration
	memset((uint8_t *)mBuff, 0xff, 2);
	disk_write((uint8_t *)mBuff, 0, 0);
#endif
}

void SPI_NOR::test_read_flash()
{
	memset(mBuff, 0x00, sizeof(mBuff));
#if 1
	//Read data
	disk_read((uint8_t *)mBuff, 0, 1);
	//DBGDMP(0, "\r\n[SerialFlash]read \r\n", mBuff, 512);
#elif 1
	//Read status
	disk_read((uint8_t *)mBuff, 0, 0);
	DBGMSG(0, "\r\n[Flash Status Register] %02x\r\n", mBuff[0]);
#endif
}
#endif