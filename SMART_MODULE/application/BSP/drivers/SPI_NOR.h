#if !defined(SPI_NOR_H)
#define SPI_NOR_H

#include "mbed.h"
#include "BlockDevice.h"
#include <stdint.h>

#define __SPI_DMA__
//#define __TEST_FILESYSTEM_READ_WRITE
//#define __TEST_SPINOR__

//#define MT25QL02GC
#define EN25Q32

/** Access the filesystem on an EEPROM using SPI using SPI
 *
 * @code
 * #include "mbed.h"
 * #include "USBSPIFileSystem.h"
 *
 * USBSPIFileSystem e2prom("e2prom", p5, p6, p7, p12); // mosi, miso, sclk, cs
 *  
 * int main() {
 *     FILE *fp = fopen("/e2prom/myfile.txt", "w");
 *     fprintf(fp, "Hello World!\n");
 *     fclose(fp);
 * }
 */
class SPI_NOR {
	#define USBMSD_VID                        0x0483
	#define USBMSD_PID                        0x5720
	
	#define SPIF_TIMEOUT    10000

public:

	# if defined(EN25Q32)
	#define PAGE_SIZE		256
	#define TOTAL_PAGES		16384
	#define TOTAL_BYTES	(PAGE_SIZE*TOTAL_PAGES)
	
	#define SECTOR_SIZE		4096
	#define PAGES_PER_SECTOR	(SECTOR_SIZE/PAGE_SIZE)
	#define TOTAL_SECTORS	1024
	
	#define V_SECTOR_SIZE		512
	#define TOTAL_V_SECTORS	    2*1024 // 1*1024*1024 / 512 => 2*1024
	
	#define WRITE_ENABLE	0x06
	#define WRITE_DISABLE	0x04	// Write Disable or Exit OTP mode
	#define PROTECT_BLOCK	0x36	// after this, ADDR 3types(msb first)
	#define UNPROTECT_BLOCK	0x39	// after this, ADDR 3types(msb first)
	#define READ_STATUS_REG	0x05	// after this, 1 bytes read
	#define PAGE_PROGRAM	0x02	// after this, ADDR 3bytes(msb first) + data(msb first)
	#define SECTOR_ERASE4K	0x20	// after this, ADDR 3bytes(msb first)
	#define SECTOR_ERASE	0x20	// after this, ADDR 3types(msb first)
	#define SECTOR_ERASE64K		0xD8	//0x52 after this, ADDR 3types(msb first)
	#define DIE_ERASE		0xC7	//0x60 after this, ADDR 3types(msb first)
	#define READ_ID			0x9F	// after this, (M7-M0) (ID15-ID8) (ID7-ID0) (7)
	#define READ_DATA		0x03	// after this, ADDR 3bytes(msb first) + data(msb first) read
	#define FAST_READ		0x0B	// after this, ADDR 3bytes(msb first) + dummy(1byte) + data(msb first) read


	// Status register from RDSR
	// [- stuff -| wel | wip ]
	// [-   6   -|  1  |  1  ]
	#define SPIF_WEL 0x2
	#define SPIF_WIP 0x1

	# elif defined(MT25QL02GC)
	#define PAGE_SIZE		256
	#define TOTAL_PAGES		1048576	
	
	#define V_SECTOR_SIZE		512
	#define TOTAL_V_SECTORS	    512*1024 // 256*1024*1024 / 512 => 512*1024

	#define SECTOR_4K_SIZE		4096
	#define TOTAL_4K_SECTORS	65536
	
	#define SECTOR_64K_SIZE		65536
	#define TOTAL_64K_SECTORS	4096

	#define RESET_ENABLE	0x66
	#define RESET_MEMORY	0x99
	#define READ_ID			0x9E	// after this, data(msb first, 1~20bytes) read
	#define READ_DATA		0x03	// after this, ADDR 3bytes(msb first) + data(msb first) read
	#define FAST_READ		0x0B	// after this, ADDR 3bytes(msb first) + dummy(1byte) + data(msb first) read
	#define READ_4DATA		0x13	// after this, ADDR 4bytes(msb first) + data(msb first) read
	#define FAST_4READ		0x0C	// after this, ADDR 4bytes(msb first) + dummy(1byte) + data(msb first) read
	#define WRITE_ENABLE	0x06
	#define WRITE_DISABLE	0x04	// Write Disable

	#define WRITE_STATUS_REG			0x01	//
	#define READ_STATUS_REG				0x05	// after this, 1 bytes read
	// Status Register
	//7		Status register			0 = Enabled/1 = Disabled(default)	Nonvolatile control bit : Used with W # to enable or
	//		write enable / disable										disable writing to the status register.
	//5		Top/bottom				0 = Top/1 = Bottom(default)			Nonvolatile control bit : Determines whether the protected
	//																	memory area defined by the block protect bits
	//																	starts from the top or bottom of the memory array.
	//6, 4:2 BP[3:0]				See Protected Area tables			Nonvolatile control bit : Defines memory to be software
	//																	protected against PROGRAM or ERASE operations.
	//																	When one or more block protect bits is set to 1, a designated
	//																	memory area is protected from PROGRAM and
	//																	ERASE operations.
	//1		Write enable			latch 0=Clear(default)/1=Set		Volatile control bit : The device always powers up with
	//																	this bit cleared to prevent inadvertent WRITE, PROGRAM,
	//																	or ERASE operations.To enable these operations,
	//																	the WRITE ENABLE operation must be executed first to
	//																	set this bit.
	//0		Write in progress		0 = Ready/1 = Busy					Status bit : Indicates if one of the following command
	//																	cycles is in progress :
	//																	WRITE STATUS REGISTER
	//																	WRITE NONVOLATILE CONFIGURATION REGISTER
	//																	PROGRAM
	//																	ERASE
		#define SPIF_WEL				0x02
		#define SPIF_WIP				0x01
	
	#define CLEAR_FLAG_STATUS_REG		0x50
	#define READ_FLAG_STAT_REG			0x70	// after this, 1 bytes read
	//7		Program or erase		0 = Busy / 1 = Ready				Status bit : Indicates whether one of the following
	//		controller													command cycles is in progress : WRITE STATUS
	//																	REGISTER, WRITE NONVOLATILE CONFIGURATION
	//																	REGISTER, PROGRAM, or ERASE.
	//6		Erase suspend			0 = Clear / 1 = Suspend				Status bit : Indicates whether an ERASE operation has been
	//																	or is going to be suspended.
	//5		Erase					0 = Clear							Error bit : Indicates whether an ERASE operation has succeeded
	//								1 = Failure or protection error		or failed.
	//4		Program					0 = Clear							Error bit : Indicates whether a PROGRAM operation has succeeded
	//								1 = Failure or protection error		or failed.
	//3		Reserved				0									Reserved
	//2		Program suspend			0 = Clear /	1 = Suspend				Status bit : Indicates whether a PROGRAM operation has
	//																	been or is going to be suspended.
	//1		Protection				0 = Clear							Error bit : Indicates whether an ERASE or PROGRAM operation
	//								1 = Failure or protection error		has attempted to modify the protected array sector, or
	//																	whether a PROGRAM operation has attempted to access the
	//																	locked OTP space.
	//0		Addressing				0 = 3 - byte addressing				Status bit : Indicates whether 3 - byte or 4 - byte address
	//								1 = 4 - byte addressing				mode is enabled.
	
		
	#define WRITE_NONVOL_CONF_REG		0xB1
	#define READ_NONVOL_CONF_REG		0xB5
	//15:12	Number of				0000 = Default						Sets the number of dummy clock cycles subsequent
	//		dummy clock cycles		0001 = 1							to all FAST READ commands
	//								0010 = 2							(See the Command Set Table for default setting
	//								.values).
	//								.
	//								1101 = 13
	//								1110 = 14
	//								1111 = Default
	//11:9	XIP mode at				000 = XIP : Fast read				Enables the device to operate in the selected XIP
	//		power - on reset		001 = XIP : Dual output fast read	mode immediately after power - on reset.
	//								010 = XIP : Dual I / O fast read
	//								011 = XIP : Quad output fast read
	//								100 = XIP : Quad I / O fast read
	//								101 = Reserved
	//								110 = Reserved
	//								111 = Disabled(Default)
	//8:6		Output driver			000 = Reserved						Optimizes the impedance at VCC / 2 output voltage.
	//		strength				001 = 90 Ohms
	//								010 = Reserved
	//								011 = 45 Ohms
	//								100 = Reserved
	//								101 = 20 Ohms
	//								110 = Reserved
	//								111 = 30 Ohms(Default)
	//5		Double transfer			0 = Enabled							Set DTR protocol as current one.Once enabled,
	//		rate protocol			1 = Disabled(Default)				all commands will work in DTR.
	//4		Reset / hold			0 = Disabled						Enables or disables HOLD # or RESET # on DQ3.
	//								1 = Enabled(Default)
	//3		Quad I / O				0 = Enabled							Enables or disables quad I / O command input
	//		protocol				1 = Disabled(Default)				(4 - 4 - 4 mode).
	//2		Dual I / O				0 = Enabled							Enables or disables dual I / O command input
	//		protocol				1 = Disabled(Default)				(2 - 2 - 2 mode).
	//1		128Mb					0 = Highest 128Mb segment			Selects the power - on default 128Mb segment for
	//		segment select			1 = Lowest 128Mb segment(Default)	3 - byte address operations.See also the extended
	//																	address register.
	#define WRITE_VOL_CONF_REG			0x81
	#define READ_VOL_CONF_REG			0x85
	//7:4	Number of				0000 = Default						Sets the number of dummy clock cycles subsequent to all
	//		dummy clock				0001 = 1							FAST READ commands
	//		cycles					0010 = 2							(See the Command Set Table for default setting values).
	//								.
	//								.
	//								1101 = 13
	//								1110 = 14
	//								1111 = Default
	//3		XIP						0 = Enable							Enables or disables XIP.
	//								1 = Disable(default)
	//2		Reserved				0									0b = Fixed value.
	//1:0	Wrap					00 = 16 - byte boundary				16 - byte wrap : Output data wraps within an aligned 16 - byte
	//								aligned								boundary starting from the 3 - byte address issued after the
	//																	command code.
	//								01 = 32 - byte boundary				32 - byte wrap : Output data wraps within an aligned 32 - byte
	//								aligned								boundary starting from the 3 - byte address issued after the
	//																	command code.
	//								10 = 64 - byte boundary				64 - byte wrap : Output data wraps within an aligned 64 - byte
	//								aligned								boundary starting from the 3 - byte address issued after the
	//																	command code.
	//								11 = Continuous(default)			Continuously sequences addresses through the entire array.
	#define WRITE_ENH_VOL_CONF_REG		0x61
	#define READ_ENH_VOL_CONF_REG		0x65
	//7		Quad I / O protocol		0 = Enabled							Enables or disables quad I / O command input
	//								1 = Disabled(Default)				(4 - 4 - 4 mode).
	//6		Dual I / O protocol		0 = Enabled							Enables or disables dual I / O command input
	//								1 = Disabled(Default)				(2 - 2 - 2 mode).
	//5		Double transfer rate	0 = Enabled							Set DTR protocol as current one.Once enabled,
	//		protocol				1 = Disabled(Default,				all commands will work in DTR
	//								single transfer rate)				
	//4		Reset / hold			0 = Disabled						Enables or disables HOLD # or RESET # on DQ3.
	//								1 = Enabled(Default)				(Available only on specified part numbers.)
	//3		Reserved				0
	//2:0	Output driver strength	000 = Reserved						Optimizes the impedance at VCC / 2 output voltage.
	//								001 = 90 Ohms
	//								010 = Reserved
	//								011 = 45 Ohms
	//								100 = Reserved
	//								101 = 20 Ohms
	//								110 = Reserved
	//								111 = 30 Ohms(Default)
	#define WRITE_EXT_ADDR_REG			0xC5
	#define READ_EXT_ADDR_REG			0xC8
	//7:4		A[31 : 24]			000000								Reserved
	//3:0		A[27 : 24]			1111 = Highest 128Mb segment		Enables specified 128Mb memory segment.The default
	//								1110 = 15th 128Mb segment			setting(lowest) can be changed to the highest
	//								1101 = 14th 128Mb segment			128Mb segment using bit 1 of the nonvolatile
	//								1100 = 13th 128Mb segment			configuration register.	
	//								1011 = 12th 128Mb segment
	//								1010 = 11th 128Mb segment
	//								1001 = 10th 128Mb segment
	//								1000 = 9th 128Mb segment
	//								0111 = 8th 128Mb segment
	//								0110 = 7th 128Mb segment
	//								0101 = 6th 128Mb segment
	//								0100 = 5th 128Mb segment
	//								0011 = 4th 128Mb segment
	//								0010 = 3rd 128Mb segment
	//								0001 = 2nd 128Mb segment
	//								0000 = Lowest 128Mb segment


	#define PAGE_PROGRAM	0x02	// after this, ADDR 3bytes(msb first) + data(msb first)
	#define PAGE_4PROGRAM	0x12	// after this, ADDR 4bytes(msb first) + data(msb first)
	#define SECTOR_ERASE4K	0x20	// after this, ADDR 3bytes(msb first)
	#define SECTOR_ERASE64K		0xD8	//0x52 after this, ADDR 3bytes(msb first)
	#define SECTOR_4ERASE4K	0x21	// after this, ADDR 4bytes(msb first)
	#define SECTOR_4ERASE64K	0xDC	//0x52 after this, ADDR 3bytes(msb first)
	#define DIE_ERASE		0xC4	//0x60 after this, ADDR 3bytes(msb first)

	#define PROGRAM_ERASE_SUSPEND		0x75
	#define PROGRAM_ERASE_RESUME		0x7A
	
	#define ENTER_4ADDR_MODE	0xB7
	#define EXIT_4ADDR_MODE		0xE9
	
	#define ADDRESSING_4BYTES
	# endif
	
    /** Create the File System for accessing an EEPROM using SPI
     *
     * @param mosi SPI mosi pin connected to SPI EEPROM
     * @param miso SPI miso pin conencted to SPI EEPROM
     * @param sclk SPI sclk pin connected to SPI EEPROM
     * @param cs   DigitalOut pin used as SPI EEPROM chip select
     * @param name The name used to access the virtual filesystem
     */
    SPI_NOR(PinName mosi, PinName miso, PinName sclk, PinName cs, int hz);
    virtual int disk_initialize();
    virtual int disk_status();
    virtual int disk_read(uint8_t * buffer, uint64_t pages, uint8_t count);
	virtual int disk_write(uint8_t * buffer, uint64_t pages, uint8_t count);
    virtual int disk_sync();
    virtual uint64_t disk_sectors();

    virtual uint64_t disk_size(){return TOTAL_V_SECTORS * V_SECTOR_SIZE;};

	int disk_erase(uint64_t sector, uint8_t count);
	int disk_erase();

#if defined(__TEST_SPINOR__)
	void test_write_flash();
	void test_read_flash();
private:
	int8_t		mBuff[512];
#endif
	
protected:

    int initialise_card();
    
    uint64_t _sectors;
    
    uint8_t _status;

    SPI _spi;
    DigitalOut _cs;
    int cdv;
#if defined(__SPI_DMA__)
	SPI_TypeDef *_spi_id;
	int _transfer_sck;
	uint8_t	dummyByte;
#else 
	int _transfer_sck;
#endif

};

#endif /* !defined(SPI_NOR_H) */
