#ifndef __URU_TOOL_H__
#define __URU_TOOL_H__
#include "mbed.h"
#include "SerialPipe.h"

class URYU : public SerialPipe
{
public:
	static const int DEFAULT_BAUDRATE	= 38400;
	static const int AT_PARSER_BUFFER_SIZE  = 256; //bytes
	static const int AT_PARSER_TIMEOUT	= (8 * 1000); //miliseconds
	static const char* OUTPUT_ENTER_KEY;
	enum
	{
		eucSWITCH_WORKNO      = 0x03,
		eucREQ_BOLT_FASTENING = 0x04,
		eucCLOSE_SESSION      = 0x06,
		eucREAD_DATE_TIME		= 0x07,
		eucWRITE_DATE_TIME		= 0x08,
		eucSWITCH_TO_HKM_PROTOCOL	  = 0x09
	} eURYU_CMD;

public:
	URYU(PinName tx = LORA_RX, PinName rx = LORA_RX, int baud = DEFAULT_BAUDRATE, char *rxBuff = NULL, int rxSize = 256, char *txBuff = NULL, int txSize = 128);
	~URYU();
	void init();
	int put(const void* buffer, int length);
	int put(Pipe<char> *pipe, int length, int offset);
	void send();
	void parser(Pipe<char>* pipe);
	uint8_t chkSum(uint8_t * buff, int len);
	void command(uint8_t cmd, uint16_t aux, uint8_t unVar);
	
	uint32_t getRespTick() {	return baseTickResp;	}
	void	setRespTick(uint32_t tick)	{	baseTickResp = tick; }
	void setTimeoutCmd(uint8_t cmd, uint16_t aux)	{	timeoutCmd = cmd; timeoutAux = aux;}

private:
	void setup_at_parser();
	void shutdown_at_parser();

public:
	bool bKeepRunning;
	bool bKeepRunningTransmitt;

	uint8_t timeoutCmd;
	uint16_t timeoutAux;
	
private:
	Thread uryu_thread, transmit_thread;

	ATCmdParser *_at;
	bool _debug_trace_on;
	uint8_t	lastSendCmd;
	uint8_t uncompletedRespCmd;
	uint32_t baseTickResp;
	uint32_t m_nWriteIndex;
	uint8_t m_nSessionIndex;
	uint16_t boltIndex;
};
extern URYU *pgUryu;
#endif	/* __URU_TOOL_H__ */