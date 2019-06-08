/* mbed Microcontroller Library
 * Copyright (c) 2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SPIF_BLOCK_DEVICE_H
#define MBED_SPIF_BLOCK_DEVICE_H

#include "BlockDevice.h"
#include "spi_dma/spi_dma.h"
//#define __TEST_FILESYSTEM_READ_WRITE
/** BlockDevice for SPI based flash devices
 *  such as the MX25R or SST26F016B
 *
 *  @code
 *  // Here's an example using the MX25R SPI flash device on the K82F
 *  #include "mbed.h"
 *  #include "SPIFBlockDevice.h"
 *  
 *  // Create flash device on SPI bus with PTE5 as chip select
 *  SPIFBlockDevice spif(PTE2, PTE4, PTE1, PTE5);
 *  
 *  int main() {
 *      printf("spif test\n");
 *  
 *      // Initialize the SPI flash device and print the memory layout
 *      spif.init();
 *      printf("spif size: %llu\n",         spif.size());
 *      printf("spif read size: %llu\n",    spif.get_read_size());
 *      printf("spif program size: %llu\n", spif.get_program_size());
 *      printf("spif erase size: %llu\n",   spif.get_erase_size());
 *  
 *      // Write "Hello World!" to the first block
 *      char *buffer = (char*)malloc(spif.get_erase_size());
 *      sprintf(buffer, "Hello World!\n");
 *      spif.erase(0, spif.get_erase_size());
 *      spif.program(buffer, 0, spif.get_erase_size());
 *  
 *      // Read back what was stored
 *      spif.read(buffer, 0, spif.get_erase_size());
 *      printf("%s", buffer);
 *  
 *      // Deinitialize the device
 *      spif.deinit();
 *  }
 *  @endcode
 */

class SPIFBlockDevice : public BlockDevice {
	#define __SPI_DMA__
	
	#define	ADDR_LEN	4	//4-Bytes Addressing
	
	#define PAGE_SIZE		256
	#define TOTAL_PAGES		1048576	
	
	#define V_SECTOR_SIZE		512
	#define TOTAL_V_SECTORS	    512*1024 // 256*1024*1024 / 512 => 512*1024

	#define SECTOR_4K_SIZE		4096
	#define TOTAL_4K_SECTORS	65536
	
	#define SECTOR_64K_SIZE		65536
	#define TOTAL_64K_SECTORS	4096

	#define DIE_SIZE			128*1024*10424
	#define TOTAL_DIES			2

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
public:
    /** Check if locally files are opened for write
    *
    * @return - true if none are opened for write, false otherwise
    */
	bool localSafe(void);
    
	/** Check if via USB files files are being written
	*
	* @return - true if none are being written, false otherwise
	*/
	bool usbSafe(void);
    
	/** Sets the USB mode
	*
	* Argument = 0: USB is write protected when not available.
	* Argument = 1: USB is disconnected when not available (default).
	*
	* @param mode - USB safety mode
	*/
	void usbMode(int mode);

	int disk_erase(uint64_t sector, uint8_t count);
	int disk_erase();

public:
	virtual int disk_initialize();
	virtual int _disk_status();
	virtual int disk_read(uint8_t * buffer, uint64_t sector, uint8_t count);
	virtual int disk_write(const uint8_t * buffer, uint64_t sector, uint8_t count);
	virtual int disk_sync();
	virtual uint64_t disk_sectors();

	virtual uint64_t disk_size(){return TOTAL_V_SECTORS * V_SECTOR_SIZE;}

public:
	uint8_t usbmsd_erase(uint32_t start_addr, uint32_t end_addr)
	{
		if (!start_addr && !end_addr)
			return usbmsd_erase();
		return erase(start_addr, end_addr - start_addr + 1);
	}
	uint8_t usbmsd_erase()	{	return _erase();	}
	uint8_t usbmsd_initialize();
	uint8_t usbmsd_status();
	uint8_t usbmsd_read(uint8_t * buffer, uint32_t addr, uint32_t count)
	{
		return read(buffer, addr, count * V_SECTOR_SIZE);
	}
	uint8_t usbmsd_write(uint8_t * buffer, uint32_t addr, uint32_t count)
	{
		return program(buffer, addr, count * V_SECTOR_SIZE);
	}

protected:
	//virtual int disk_status_fat(void);
	virtual int disk_status_msd(void);

	int initialise_card();
    
	uint64_t _sectors;
    
	uint8_t _status;

	int cdv;
private:
	int local_count;
	int usbmode;
	Timeout usb_write;
	bool usbfree;


public:
    /** Creates a SPIFBlockDevice on a SPI bus specified by pins
     *
     *  @param mosi     SPI master out, slave in pin
     *  @param miso     SPI master in, slave out pin
     *  @param sclk     SPI clock pin
     *  @param csel     SPI chip select pin
     *  @param freq     Clock speed of the SPI bus (defaults to 40MHz)
     */
    SPIFBlockDevice(PinName mosi, PinName miso, PinName sclk, PinName csel, int freq=40000000);

    /** Initialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int init();

    /** Deinitialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int deinit();

    /** Read blocks from a block device
     *
     *  @param buffer   Buffer to write blocks to
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int program(const void *buffer, bd_addr_t addr, bd_size_t size);

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int erase(bd_addr_t addr, bd_size_t size);

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const;

    /** Get the size of a programable block
     *
     *  @return         Size of a programable block in bytes
     *  @note Must be a multiple of the read size
     */
    virtual bd_size_t get_program_size() const;

    /** Get the size of a eraseable block
     *
     *  @return         Size of a eraseable block in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size() const;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const;
    
private:
    // Master side hardware
    SPI _spi;
    DigitalOut _cs;

    // Device configuration discovered through sfdp
    bd_size_t _size;

    // Internal functions
    int _wren();
    int _sync();
    void _cmdread(uint8_t op, uint32_t addrc, uint32_t retc,
            uint32_t addr, uint8_t *rets);
    void _cmdwrite(uint8_t op, uint32_t addrc, uint32_t argc,
            uint32_t addr, const uint8_t *args);
	
	uint8_t _statusflag();
	void _clear();
	int _erase();
	int _erase64k(bd_addr_t addr, bd_size_t size);
#if defined(__SPI_DMA__)
	SPI_TypeDef *_spi_id;
#endif
};


#endif  /* MBED_SPIF_BLOCK_DEVICE_H */
