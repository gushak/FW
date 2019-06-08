#ifndef __CONFIG_H__
#define __CONFIG_H__

//#define SAVE_CONF_DATA_INDIVIDUALLY

#include "flash_hw_map.h"
#include "Pipe.h"

#define FLASH_CONFIG_BLOCK_SIZE		512
#define FLASH_CONFIG_UPDATE_PENDING_MARK	0x55555555

#define FLASH_CONFIG_WP_START	ADDR_FLASH_SECTOR_2
#define FLASH_CONFIG_RP_START	ADDR_FLASH_SECTOR_3
#define FLASH_CONFIG_WN_START	ADDR_FLASH_SECTOR_4
#define FLASH_CONFIG_WN_END		ADDR_FLASH_SECTOR_5
typedef enum
{
	eetWP,
	eetRP,
	eetWN,
	eetSP
} eEXF_TYPE;

typedef enum
{
	MODULE_OUT_SERIAL,
	MODULE_OUT_USB,
	MODULE_OUT_BT,
	MODULE_OUT_RF,
	MODULE_OUT_GPS,
	MODULE_OUT_SD,
	MODULE_OUT_COUNT
} eMODULE_OUTPUT;
//
// Config Structure Definition
//
typedef __PACKED_STRUCT_PRE _CONFIG_HDR
{
	u32					ulMagic;				//
	u16					usVer;					//
	u16					usSize;					//
	u32					ulSeqID;				//
} __PACKED_STRUCT_LAST CONFIG_HDR, *PCONFIG_HDR;	// 12 bytes

typedef __PACKED_STRUCT_PRE _SETTING_DATA
{
	u32	uwbId;			//UWB ID
	u16 netId;			//LoRa network ID
	u8	grpId[4];		//LoRa group ID
	u8	rsvd[34];
} __PACKED_STRUCT_LAST SETTING_DATA, *PSETTING_DATA; 	// 44 bytes


typedef __PACKED_STRUCT_PRE _CONFIG_INFO
{
	// HEADER (12 bytes)
	CONFIG_HDR			hdr;					//12 bytes

				// Todo: Add Config Item
	u32 				ulFwBuildDate;			// 4 bytes
	u16 				usFwVer;				// 2 bytes	//It has the same value as hdr.usVer
	u16		 			usBootVer;				// 2 bytes	// The version number of BootLoader
												//==> 20 bytes
	SETTING_DATA 		Setting;				// 44 bytes
	u8					rsvd[384];				
	u32					fw_update[16];			//64 bytes
} __PACKED_STRUCT_LAST CONFIG_INFO, *PCONFIG_INFO;	// 128 bytes

	//
	// Export Variable
	//
extern u8			g_CurrentConfigRecNo;

#define PRODUCT_MODEL 			"AKSM"
#define CONFIG_MAGIC			*(int*)(PRODUCT_MODEL)

#define WIFI_AP_SSID 				"SK_WiFi2A0A"//"T Pocket-Fi 0118331"//"SK_WiFi2A0A"
#define WIFI_AP_PWD					"1406000102"//"T1482392"//"1406000102"
#define WIFI_PEER_DHCP 				1 	//0=Disable, 1=Enable
#define WIFI_PEER_IP 				"192.168.0.9"
#define WIFI_PEER_PORT 				5001

#define BT_PEER_NAME "RAPHAEL-MINIPC"//"\"Amp\'ed Up!\""//"\"RJoh\""//"\"RAPHAEL-LAPTOP\""//	/*"FC175948"*/
#define BT_PEER_ADDR "3052cb508cd2"

	//==============================================================
	// basic information to connect to NTrip Server
	//==============================================================
#define MODEM_HOST_IP 				"vrs3.ngii.go.kr"
#define MODEM_HOST_PORT 			2101
# define MODEM_HOST_USERNAME    "ascen8630"
# define MODEM_HOST_PASSWORD    "ngii"
	//==============================================================
	// basic information to connect to NTrip Server
	//==============================================================

	//==============================================================
	// basic information to connect to carrier's network
	//==============================================================
	// SIM pin code goes here
# define MBED_CONF_APP_SIM_PIN_CODE    "0000"
# define MBED_CONF_APP_APN         "lte-internet.sktelecom.com"
# define MBED_CONF_APP_USERNAME    NULL
# define MBED_CONF_APP_PASSWORD    NULL
	//==============================================================
	// basic information to connect to carrier's network
	//==============================================================

class Config
{
	//
	// Config Default Value Definition
	//
	static const int FW_VER_MAJOR		=	1;				// (Major Version Change -> Not Compatible Change)
	static const int FW_VER_MINOR		=	4;
	static const int FW_BUILD_DATE		=	20180306;

	#define MAKE_VER16(v1,v2)				(uint16_t)(((v1)<<8)|(v2))
	#define MAKE_VER32(v1,v2,v3,v4)			(uint32_t)(((v1)<<24)|((v2)<<16)|((v3)<<8)|(v4))

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

	static const int FLASH_ADDR_BOOT	 		=	FLASH_ADDR_START;
	static const int FLASH_ADDR_CONFIG 			=	ADDR_FLASH_SECTOR_11;
	static const int FLASH_ADDR_APP	 			=	ADDR_FLASH_SECTOR_0;
	static const int FLASH_ADDR_APP_DOWN 		=	ADDR_FLASH_SECTOR_7;


	//
	// Config Data Definition
	//

	static const int CONFIG_VER					=	MAKE_VER16(FW_VER_MAJOR, FW_VER_MINOR);

	static const int CONFIG_REC_SIZE			=	((sizeof(CONFIG_INFO) + FLASH_CONFIG_BLOCK_SIZE - 1) / FLASH_CONFIG_BLOCK_SIZE*FLASH_CONFIG_BLOCK_SIZE);					// max size of config record
	static const int CONFIG_REC_COUNT_PER_SECTOR	=	32;					// config record count per sector

	static const int CONFIG_ADDRESS 			=	FLASH_ADDR_CONFIG;


	static const int INVALID_CONFIG_SEQ_ID		=	0xffff;

public:
	typedef struct
	{
		const void *buff;
		uint32_t addr;
		uint32_t size;
	} conf_cmd_t;

public:
#if defined(SAVE_CONF_DATA_INDIVIDUALLY)
	Config(int n, conf_cmd_t* cmdBuff);
#else
	Config();
#endif
	//
	// Config Data API
	//
	BOOL LoadConfig();
	BOOL SaveConfig(bool freeze = false);
	//void CheckConfig();
	bool ChangeConfigU8(u8* conf, u8 value)
	{
		if (*conf == value)
			return false;
		*conf = value;
		g_ConfigChanged = TRUE;
		return true;
	}
	bool ChangeConfig(void* conf, void* value, int size);
	void scanExfIndices(eEXF_TYPE type);
	void updateExfIndices(eEXF_TYPE type, uint32_t value);
	BOOL isConfChanged() { return g_ConfigChanged;	}

	int flash_write(const void *buff, uint32_t addr, uint32_t size)
	{
		int ret;
		lock();
		ret = g_Flash.program(buff, addr, size);
		unlock();
		return ret;
	}
	int flash_erase_sector(uint32_t addr)
	{
		int ret;
		lock();
		ret = g_Flash.erase(addr, g_Flash.get_sector_size(addr));
		unlock();
		return ret;
	}
	void mutex_init()	{	_mutex = new Mutex; }
	void lock() {	_mutex->lock(); }
	void unlock() {	_mutex->unlock(); }
	void mutex_reset() {	delete _mutex; mutex_init(); }

#if defined(SAVE_CONF_DATA_INDIVIDUALLY)	
	void Init();
	bool bKeepRunning;
	void killProcess() {	bKeepRunning = false; }
	Pipe <conf_cmd_t> *getInf() {	return &_pipeCmd;	}
private:
	Thread config_thread;
	Pipe <conf_cmd_t> _pipeCmd;
#else
private:
#endif
	Mutex *_mutex;

	u8				g_CurrentConfigRecNo = 0;	//zero index (0~31)
	BOOL			g_isZeroRecord = FALSE;
	u8	facMode = 0;
	FlashIAP g_Flash;
	BOOL			g_ConfigChanged = FALSE;
	void ApplyDefaultConfig(BOOL isEmptyConfig);
	void ApplyDefaultApended();
	void setDefOutPath(u8 mode);
};

extern Config *pgConf;
#endif //__CONFIG_H__
