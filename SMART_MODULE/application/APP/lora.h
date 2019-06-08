#ifndef __LORA_H__
#define __LORA_H__
#include "mbed.h"
#include "SerialPipe.h"

class LORA : public SerialPipe
{
public:
	static const int DEFAULT_BAUDRATE	= 9600;
	static const int AT_PARSER_BUFFER_SIZE  = 256; //bytes
	static const int AT_PARSER_TIMEOUT	= (8 * 1000); //miliseconds
	static const char* OUTPUT_ENTER_KEY;
	typedef enum
	{
		elcDISP_SYS_PARAMS,
		elcSET_GID,
		elcSET_DEV_TYPE,
		elcSET_NID,
		elcSET_RF_PWR,
		elcSET_SPEC_FACTOR,
		elcGET_SYS_PARAMS,
		elcSET_SYS_PARAMS,
		elcSET_ALIVE_PERIOD
	} eLORA_CMD;
	#define LORA_CMD_DISP_SYS_PARAMS	"$Rpar=\r\n"
#define LORA_CMD_DISP_SYS_PARAMS_RESP_DEV_TYPE	""
	#define LORA_CMD_SET_GID			"$Wgid=%d:%d\r\n"
	#define LORA_CMD_SET_DEV_TYPE		"$Wdvt=%d\r\n"
	#define LORA_CMD_SET_NID			"$Wntwid=%d\r\n"
	#define LORA_CMD_SET_RF_PWR			"$Wrfpower=%d\r\n"
	#define LORA_CMD_SET_SPEC_FACTOR	"$Wsf=%d\r\n"
	#define LORA_CMD_GET_SYS_PARAMS		"$Rflash=\r\n"
	#define LORA_CMD_SET_SYS_PARAMS		"$Wflash=\r\n"
	#define LORA_CMD_SET_ALIVE_PERIOD	"$Warup=%d\r\n"
	#define LORA_CMD_SEND				"$Trup=%s\r\n"
	#define LORA_CMD_RECV_COMMON		"Urdp:"
	#define LORA_CMD_RECV				"Urdp:Seq:%d:"
	#define LORA_CMD_RECV_W_CMD			"Urdp:Cmd:Seq:%d:%d:"
	enum
	{
		eldtBS,
		eldtRS
	} eLORA_DEVICE_TYPE;
	typedef struct
	{
		uint8_t	devType;
		uint32_t sn;
		uint16_t netId;
		uint8_t	grpId1;
		uint8_t	grpId2;
		uint8_t	grpId3;
		uint8_t	grpId4;
		uint8_t	spFactor;
		uint8_t	rfPwr;
		uint8_t	bd1_prd;
		uint8_t	bds_prd;
		uint16_t baud;
		uint8_t upFormat;
		char	version[128];
	} stLORA_BS_RPAR;
	
	#define HKMC_STX	0x02
	#define HKMC_FIXED_CRC	0xffff
	#define HKMC_ETX	0x03
	#define HKMC_PROTOCOL_LEN_MAX	113 //Data[110] + CRC[2] + ETX(1)	
	#define HKMC_PROTOCOL_URYU	0x10
public:
	LORA(PinName tx = LORA_RX, PinName rx = LORA_RX, PinName rst = LORA_RST, int baud = DEFAULT_BAUDRATE, char *rxBuff = NULL, int rxSize = 256, char *txBuff = NULL, int txSize = 128);
	~LORA();
	void init();
	int put(const void* buffer, int length);
	int put(Pipe<char> *pipe, int length, int offset);
	int send();
	bool isConnected()	{	return _connected;	}
	void asciiParser(Pipe<char>* pipe);
	void binaryParser(Pipe<char>* pipe);
	
	uint32_t getRespTick() {	return baseTickResp;	}
	void	setRespTick(uint32_t tick)	{	baseTickResp = tick; }
	void setTimeoutCmd(uint8_t cmd, uint16_t aux)	{	timeoutCmd = cmd; timeoutAux = aux; }

	bool sendMsg(const void* buffer, int length);
private:
	void setup_at_parser();
	void shutdown_at_parser();
	int16_t getNumberBySeparator(uint8_t *buff, char sep, uint8_t *offset);
public:
	bool bKeepRunning;
	bool bKeepRunningTransmitt;
	bool _protocol_parsing_step;
	uint8_t	_protocol_len;
	uint8_t	lastRecvCmd;
	uint16_t _bolt_fastening_index;

	uint8_t timeoutCmd;
	uint16_t timeoutAux;

private:
	Thread lora_thread, transmit_thread;
	DigitalOut	_rst, _wkup;

	ATCmdParser *_at;
	bool _debug_trace_on;
	bool _connected;
	//char resp[28];
	
	uint8_t uncompletedRespCmd;
	uint32_t baseTickResp;
	stLORA_BS_RPAR _Rpar;
	char uuid[64];
};
extern LORA *pgLora;
#endif	/* __LORA_H__ */