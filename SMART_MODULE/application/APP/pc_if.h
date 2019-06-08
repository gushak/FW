#ifndef _PCIF_H_
#define _PCIF_H_


class PCIF
{
public:
	static const int HEADER_1	= 0xAA;
	static const int HEADER_2	= 0x55;

	static const int OFFSET_HDR = 0;	// 2bytes
	static const int FFSET_LEN = 2;	// 2 bytes
	static const int OFFSET_CMD = 4;
	static const int OFFSET_SUBCMD	= 5;
	
	typedef enum
	{
		eCMD_ACK			= 0,
		eCMD_NACK			= 1,
		eCMD_DEVINFO		= 2,
		eCMD_FWUPDATE		= 3,
		eCMD_DISKVOLUME		= 4,
		eCMD_BLEINFO		= 5,
		eCMD_GNSSDISABLESVS	= 6,
		eCMD_GPS_DEFOUTPUT	= 7,
		eCMD_OUTPATH		= 8,
		eCMD_GNSSSAVE		= 9,
		eCMD_GPS			= 0x81,
		eCMD_RF				= 0x82,
		eCMD_IMU			= 0x83,
		eCMD_BT				= 0x84,
		eCMD_DISP			= 0x85,
		eCMD_MAX			= 16
	} ePCIF_COMMAND;
	typedef enum {
		ePIFSUB_INVALID,
		ePIFSUB_MODE,
		ePIFSUB_DIRECT
	} ePCIF_SUBCMD;

	typedef enum {
		ePIFDEV_GPS,
		ePIFDEV_RF,
		ePIFDEV_IMU,
		ePIFDEV_BT,
		ePIFDEV_DISP,
		ePIFDEV_COUNT
	} ePCIF_DEV;
	typedef enum
	{
		eDOM_PCIF          = 0x1,	// PC Interface mode
		eDOM_DEBUG         = 0x2,	// MCU serial debug mode
		eDOM_MCU           = 0x4,	// MCU Register debugger mode
		eDOM_GPS           = 0x8,	// GPS pass-through mode
		eDOM_DIRECT_DEVICE = 0x8000	// UART direct mode
	} eDIRECT_OPR_MODE;

	typedef enum { 
		WAIT      = -1, //!< wait for more incoming data (the start of a message was found, or no data available)
		NOT_FOUND = 0, //!< a parser concluded the the current offset of the pipe doe not contain a valid message

		UNKNOWN   = 0x000000,       //!< message type is unknown 
	} ePROTOCOL_ID;


public:
	PCIF();
	void Init();
	int send(uint8_t *buff, uint8_t size);
	int _checkPacket(Pipe<char>* pipe, int l);
	
	bool bKeepRunning;
	void killProcess() {	bKeepRunning = false; }

	uint8_t *lastPacketBuff;
	uint8_t lastPacketSize;

private:
	Thread pcif_thread;
	Timeout Timeout4waitingSend;
};

extern PCIF *gpPcif;
#endif /* _PCIF_H_ */