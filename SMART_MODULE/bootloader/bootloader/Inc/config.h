#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "usbd_dfu_flash.h"

#ifdef __GNUC__
#define __PACKED_STRUCT_PRE		struct
#define __PACKED_STRUCT_LAST	__attribute__ ((packed))
#else
#define __PACKED_STRUCT_PRE 	__packed struct
#define __PACKED_STRUCT_LAST 	
#endif 	

#define FLASH_SIZE      (uint32_t) 0x100000

#define FLASH_CONFIG_BLOCK_SIZE		512
#define FLASH_CONFIG_UPDATE_PENDING_MARK	0x55555555

//
// Config Structure Definition
//
typedef __PACKED_STRUCT_PRE _CONFIG_HDR
{
	uint32_t					ulMagic;				//	4 bytes
	uint16_t					usVer;					//	2 bytes
	uint16_t					usSize;					//	2 bytes
	uint32_t					ulSeqID;				//	4 bytes
} __PACKED_STRUCT_LAST CONFIG_HDR, *PCONFIG_HDR;	// 12 bytes

typedef __PACKED_STRUCT_PRE _CONFIG_INFO
{
	// HEADER (12 bytes)
	CONFIG_HDR			hdr;					//12 bytes

				// Todo: Add Config Item
	uint32_t 			ulFwBuildDate;			// 4 bytes
	uint16_t 			usFwVer;				// 2 bytes	//It has the same value as hdr.usVer
	uint16_t 			usBootVer;				// 2 bytes
												//==> 20 bytes
	uint8_t				etc[44];				//44 bytes
	uint8_t				rsvd[384];				//384 bytes
	uint32_t			fw_update[16];			//64 bytes
} __PACKED_STRUCT_LAST CONFIG_INFO, *PCONFIG_INFO;	// 512 bytes


	//
	// Config Default Value Definition
	//
#define BOOT_VER_MAJOR					1				// (Major Version Change -> Not Compatible Change)
#define BOOT_VER_MINOR					1
#define BOOT_BUILD_DATE					20180306

#define MAKE_VER16(v1,v2)				(uint16_t)(((v1)<<8)|(v2))

#define BOOT_VER						MAKE_VER16(BOOT_VER_MAJOR,BOOT_VER_MINOR)
/*
	Program Memory(Flash) Area Definition:

	Address				Description
	0x08000000		+-----------------------------------------------------------+
					|	Boot Area										        |  64K
	0x08010000		+-----------------------------------------------------------+
					|	Config Area												|  64K
	0x08020000		+-----------------------------------------------------------+
					|	Application Area										| 256K
	0x08060000		+-----------------------------------------------------------+
					|	Application Download Area								| 256K
	0x080A0000		+-----------------------------------------------------------+
					|	Unused Area												| 384K
	0x08100000		+-----------------------------------------------------------+
*/

#define FLASH_ADDR_BOOT	 				FLASH_ADDR_START
#define FLASH_ADDR_CONFIG 				ADDR_FLASH_SECTOR_11
#define FLASH_ADDR_APP	 				ADDR_FLASH_SECTOR_0
#define FLASH_ADDR_APP_DOWN 			FLASH_ADDR_SECT_7


//
// Config Data Definition
//

#define CONFIG_MAGIC					0x46543138			// 'FT18' means FitTogether 2018

#define CONFIG_REC_SIZE					((sizeof(CONFIG_INFO)+FLASH_CONFIG_BLOCK_SIZE-1)/FLASH_CONFIG_BLOCK_SIZE*FLASH_CONFIG_BLOCK_SIZE)					// max size of config record
#define CONFIG_REC_COUNT_PER_SECTOR		32					// config record count per sector

#define CONFIG_ADDRESS 					FLASH_ADDR_CONFIG


#define INVALID_CONFIG_SEQ_ID			0xffff

int isFWUpdatePending();
void clearFWUpdatePending();
void updateBootVerNum();

#endif //__CONFIG_H__
