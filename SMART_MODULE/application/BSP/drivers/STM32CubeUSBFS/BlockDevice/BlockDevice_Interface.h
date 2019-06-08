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
#ifndef MBED_SPIF_BLOCK_DEVICE_INTERFACE_H
#define MBED_SPIF_BLOCK_DEVICE_INTERFACE_H

#include "rtx_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USBMSC_SDIO_SD)

#elif defined(USBMSC_SPI_SD)

#elif defined(USBMSC_SPI_FLASH)
#define SPIF_TIMEOUT    100
	 
	 
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

	 
#define __SPI_DMA__
#define SD_DUMMY_BYTE	0xaa

#endif
	enum {    
		BD_OK      = 0x00,      
		MSD_OK         = 0x00,
		BD_ERROR   = 0x01,
		MSD_ERROR   = 0x01,
		BD_TIMEOUT = 0x02,
		MSD_TIMEOUT = 0x02
	};

	typedef struct              
	{
		uint8_t  Reserved1 : 2;               /* Reserved */
		uint16_t DeviceSize : 12;             /* Device Size */
		uint8_t  MaxRdCurrentVDDMin : 3;      /* Max. read current @ VDD min */
		uint8_t  MaxRdCurrentVDDMax : 3;      /* Max. read current @ VDD max */
		uint8_t  MaxWrCurrentVDDMin : 3;      /* Max. write current @ VDD min */
		uint8_t  MaxWrCurrentVDDMax : 3;      /* Max. write current @ VDD max */
		uint8_t  DeviceSizeMul : 3;           /* Device size multiplier */
	} struct_v1;


	typedef struct              
	{
		uint8_t  Reserved1 : 6;               /* Reserved */
		uint32_t DeviceSize : 22;             /* Device Size */
		uint8_t  Reserved2 : 1;               /* Reserved */
	} struct_v2;

	/** 
	  * @brief  Card Specific Data: CSD Register
	  */ 
	typedef struct
	{
	/* Header part */
		uint8_t  CSDStruct : 2;            /* CSD structure */
		uint8_t  Reserved1 : 6;            /* Reserved */
		uint8_t  TAAC : 8;                 /* Data read access-time 1 */
		uint8_t  NSAC : 8;                 /* Data read access-time 2 in CLK cycles */
		uint8_t  MaxBusClkFrec : 8;        /* Max. bus clock frequency */
		uint16_t CardComdClasses : 12;      /* Card command classes */
		uint8_t  RdBlockLen : 4;           /* Max. read data block length */
		uint8_t  PartBlockRead : 1;        /* Partial blocks for read allowed */
		uint8_t  WrBlockMisalign : 1;      /* Write block misalignment */
		uint8_t  RdBlockMisalign : 1;      /* Read block misalignment */
		uint8_t  DSRImpl : 1;              /* DSR implemented */
  
		/* v1 or v2 struct */
		union csd_version {
			struct_v1 v1;
			struct_v2 v2;
		} version;
  
		uint8_t  EraseSingleBlockEnable : 1;  /* Erase single block enable */
		uint8_t  EraseSectorSize : 7;         /* Erase group size multiplier */
		uint8_t  WrProtectGrSize : 7;         /* Write protect group size */
		uint8_t  WrProtectGrEnable : 1;       /* Write protect group enable */
		uint8_t  Reserved2 : 2;               /* Reserved */
		uint8_t  WrSpeedFact : 3;             /* Write speed factor */
		uint8_t  MaxWrBlockLen : 4;           /* Max. write data block length */
		uint8_t  WriteBlockPartial : 1;       /* Partial blocks for write allowed */
		uint8_t  Reserved3 : 5;               /* Reserved */
		uint8_t  FileFormatGrouop : 1;        /* File format group */
		uint8_t  CopyFlag : 1;                /* Copy flag (OTP) */
		uint8_t  PermWrProtect : 1;           /* Permanent write protection */
		uint8_t  TempWrProtect : 1;           /* Temporary write protection */
		uint8_t  FileFormat : 2;              /* File Format */
		uint8_t  Reserved4 : 2;               /* Reserved */
		uint8_t  crc : 7;                     /* Reserved */
		uint8_t  Reserved5 : 1;               /* always 1*/
  
	} SD_CSD;

	/** 
	* @brief  Card Identification Data: CID Register   
	*/
	typedef struct
	{
		__IO uint8_t  ManufacturerID;       /* ManufacturerID */
		__IO uint16_t OEM_AppliID;          /* OEM/Application ID */
		__IO uint32_t ProdName1;            /* Product Name part1 */
		__IO uint8_t  ProdName2;            /* Product Name part2*/
		__IO uint8_t  ProdRev;              /* Product Revision */
		__IO uint32_t ProdSN;               /* Product Serial Number */
		__IO uint8_t  Reserved1;            /* Reserved1 */
		__IO uint16_t ManufactDate;         /* Manufacturing Date */
		__IO uint8_t  CID_CRC;              /* CID CRC */
		__IO uint8_t  Reserved2;            /* always 1 */
	} SD_CID;

	/** 
	* @brief SD Card information 
	*/
	typedef struct
	{
		SD_CSD Csd;
		SD_CID Cid;
		uint32_t CardCapacity;              /*!< Card Capacity */
		uint32_t CardBlockSize;             /*!< Card Block Size */
		uint32_t LogBlockNbr;               /*!< Specifies the Card logical Capacity in blocks   */
		uint32_t LogBlockSize;              /*!< Specifies logical block size in bytes           */
	} SD_CardInfo;
	
	/**
  * @brief  SD detection on its memory slot
  */
	#define SD_PRESENT               ((uint8_t)0x01)
	#define SD_NOT_PRESENT           ((uint8_t)0x00)

	#define SD_DATATIMEOUT           ((uint32_t)100000000)

	/** 
	  * @brief SD Card information structure 
	  */   
	#define BD_CardInfo SD_CardInfo

	/** 
	  * @brief  SD transfer state definition  
	  */     
	#define   SD_TRANSFER_OK                ((uint8_t)0x00)
	#define   SD_TRANSFER_BUSY              ((uint8_t)0x01)


	uint8_t BD_Init(void);
	uint8_t BD_DeInit(void);
	uint8_t BD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
	uint8_t BD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
	uint8_t BD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
	uint8_t BD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
	uint8_t BD_Erase(uint32_t StartAddr, uint32_t EndAddr);
	uint8_t BD_GetCardState(void);
	void	BD_GetCardInfo(SD_CardInfo *pCardInfo);
	uint8_t BD_IsDetected();
	
#ifdef __cplusplus
}
#endif
#endif  /* MBED_SPIF_BLOCK_DEVICE_INTERFACE_H */
