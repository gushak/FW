#include "common.h"

const char* LORA::OUTPUT_ENTER_KEY  = "\r";


__ALIGN_BEGIN char gLoraBuff[1024*2] __ALIGN_END = {0,};
LORA gLora(LORA_TX, LORA_RX, LORA_RST, 115200, gLoraBuff, sizeof(gLoraBuff));
LORA *pgLora = NULL;
Timeout _tLora;

static void lora_process();
static void lora_tx_process();

LORA::LORA(PinName tx, PinName rx, PinName rst, int baud, char *rxBuff, int rxSize, char *txBuff, int txSize)
	: SerialPipe(tx, rx, baud, rxBuff, rxSize, txBuff, txSize)
	, _rst(rst, HIGH)
	, _wkup(LORA_WKUP, LOW)
{
	pgLora = this;
	_debug_trace_on = false;
	_connected = false;
	lora_thread.start(lora_process);
}

LORA::~LORA()
{
	shutdown_at_parser();
}

//Refer to <svn>/mbed/SmartModule/Documents/DevelRef/FromGeoPlan/AnyConMesh_250LS051 Test Guide_2018.04.pdf
void LORA::init()
{
	bool success = true;
	_protocol_parsing_step = false;
	lastRecvCmd = 0;

	setup_at_parser();

	do
	{
		_rst = LOW;
		wait_ms(200);
		_rst = HIGH;
		wait(1);
		//_wkup = HIGH;
	
		if (_at->recv("D: UUID=%s \r\n",uuid))
			break;		
	} while (true);

	getRxPipe()->init();
	memset(gLoraBuff, 0, sizeof(gLoraBuff));
	int retry = 3, index = 0;
#if 0
	_at->send(LORA_CMD_DISP_SYS_PARAMS);
	wait(1);
#else
	while (retry > 0 && index >= 0)
	{
		switch (index)
		{
		case 0:
			if (_at->send(LORA_CMD_DISP_SYS_PARAMS))
				index++;
			else
				retry--;
			break;
		case 1:
			if (_at->recv("D: Device Type: %d (", &_Rpar.devType))
				index++;
			else
			{
				retry--;
				index = 0;
			}
			break;
		case 2:
			if (_at->recv("D: Network ID: %d \r\n", &_Rpar.netId))
				index++;
			else
			{
				retry--;
				index = 0;
			}
			break;
		case 3:
			if (_at->recv("D: Group ID1: %d(", &_Rpar.grpId1))
				index++;
			else
			{
				retry--;
				index = 0;
			}
			break;
		case 4:
			if (_at->recv("D: Group ID2: %d \r\n", &_Rpar.grpId2))
				index++;
			else
			{
				retry--;
				index = 0;
			}
			break;
		case 5:
			if (_at->recv("D: Group ID3: %d \r\n", &_Rpar.grpId3))
				index++;
			else
			{
				retry--;
				index = 0;
			}
			break;
		case 6:
			if (_at->recv("D: Spectral Factor: %d \r\n", &_Rpar.spFactor))
				index++;
			else
			{
				retry--;
				index = 0;
			}
			break;
		case 7:
			if (_at->recv("D: RF Output Power: %d \r\n", &_Rpar.rfPwr))
				index = -1;
			else
			{
				retry--;
				index = 0;
			}
			break;
		}
	}
	if (index >= 0)
	{
		return;		
	}
#endif
	//DBGMSG(0, "[LORA] init - 1\r\n");
	int changeCount = 0;
	if (_Rpar.devType != eldtRS)
	{
		_at->send(LORA_CMD_SET_DEV_TYPE, eldtRS);
		changeCount++;
	}
	if (_Rpar.netId != CFG(Setting).netId)
	{
		_at->send(LORA_CMD_SET_NID, CFG(Setting).netId);
		changeCount++;
	}
	if (_Rpar.grpId1 != CFG(Setting).grpId[0])
	{
		_at->send(LORA_CMD_SET_GID, 1, CFG(Setting).grpId[0]);
		changeCount++;
	}
	if (_Rpar.grpId2 != CFG(Setting).grpId[1])
	{
		_at->send(LORA_CMD_SET_GID, 2, CFG(Setting).grpId[1]);
		changeCount++;
	}
	if (_Rpar.grpId3 != CFG(Setting).grpId[2])
	{
		_at->send(LORA_CMD_SET_GID, 3, CFG(Setting).grpId[2]);
		changeCount++;
	}
	if (changeCount)
		_at->send(LORA_CMD_SET_SYS_PARAMS);
	wait(500);
	getRxPipe()->init();
	memset(gLoraBuff, 0, sizeof(gLoraBuff));
}

void lora_process()
{
	//DBGMSG(0, "[LORA] Process Started===>\r\n");

	pgLora->bKeepRunning = true;
	pgLora->init();
	
	int ret = 0;
	Pipe<char> *pipe = pgLora->getRxPipe();
	while (pgLora->bKeepRunning)
	{
		while (BitOpMode & DOM_UART6)
		{
			wait(1);
		}
		if (!pgLora->readable())
		{
			wait_ms(10);
			continue;
		}
		
		//if (pipe->get(0) == HKMC_STX)
			pgLora->binaryParser(pipe);
		//else
		//	pgLora->asciiParser(pipe);
		wait_ms(10);
	}
	//DBGMSG(0, "[LORA] Terminate \r\n");
	pgLora->rx_suspend();
}

void lora_tx_process()
{
	while (pgLora->getTxPipe()->readable())
	{
		pgLora->send();
	}
	DBGMSG(0, "[LORA/Transmit] Terminate \r\n");
}

static void parser_abort(ATCmdParser *at)
{
	at->abort();
}

void LORA::setup_at_parser()
{
	if (_at) {
		return;
	}

	_at = new ATCmdParser(this,
		OUTPUT_ENTER_KEY,
		AT_PARSER_BUFFER_SIZE,
		AT_PARSER_TIMEOUT,
		_debug_trace_on ? true : false);

		    /* Error cases, out of band handling  */
	_at->oob("ERROR", callback(parser_abort, _at));
}

void LORA::shutdown_at_parser()
{
	delete _at;
	_at = NULL;
}

int LORA::send()
{
	txCopy();
}
int LORA::put(const void* buffer, int length)
{
	uint8_t hdrHKMC[4] = { HKMC_STX, 0, 0, (uint8_t)length };
	uint8_t tailHKMC[3] = { 0xff, 0xff, HKMC_ETX };
	uint8_t *pNetId = (uint8_t *)&CFG(Setting).netId;

	hdrHKMC[1] = pNetId[1];
	hdrHKMC[2] = pNetId[0];
	
	getTxPipe()->put((char*)hdrHKMC, sizeof(hdrHKMC));
	getTxPipe()->put((char*)buffer, length);
	getTxPipe()->put((char*)tailHKMC, sizeof(tailHKMC));
	send();
//	if (transmit_thread.get_state() < Thread::Ready || transmit_thread.get_state() == Thread::Deleted)
//		transmit_thread.start(lora_tx_process);
}
int LORA::put(Pipe<char> *pipe, int length, int offset)
{
	uint8_t hdrHKMC[4] = { HKMC_STX, 0, 0, (uint8_t)length };
	uint8_t tailHKMC[3] = { 0xff, 0xff, HKMC_ETX };
	uint8_t *pNetId = (uint8_t *)&CFG(Setting).netId;

	hdrHKMC[1] = pNetId[1];
	hdrHKMC[2] = pNetId[0];
	
	getTxPipe()->put((char*)hdrHKMC, sizeof(hdrHKMC));
	getTxPipe()->put(pipe, length, offset);
	getTxPipe()->put((char*)tailHKMC, sizeof(tailHKMC));
	send();
//	if (transmit_thread.get_state() < Thread::Ready || transmit_thread.get_state() == Thread::Deleted)
//		transmit_thread.start(lora_tx_process);
}

bool LORA::sendMsg(const void* buffer, int length)
{
	bool success;
	rx_suspend();
	success = _at->send(LORA_CMD_SEND,buffer) && _at->recv("D: C - pktreceived.");
	rx_resume();
	return success;
}

static void protocol_timeout()
{
	pgLora->_protocol_parsing_step = false;
}

void LORA::binaryParser(Pipe<char>* pipe)
{
	uint8_t respData[42] = { 0, };
	int len = pipe->size();
	int index = 0;

	if (!uncompletedRespCmd)
	{
		uint8_t	hdrHKMC[3] = { HKMC_STX, };
		uint8_t *pNetId = (uint8_t *)&CFG(Setting).netId;

		hdrHKMC[1] = pNetId[1];
		hdrHKMC[2] = pNetId[0];
		while (!uncompletedRespCmd && len > 0)
		{
			len--;
			if (pipe->getc() == hdrHKMC[index++] && index >= sizeof(hdrHKMC))
			{
				_protocol_len = 0;
				break;
			}
			else
				index = 0;
		}
		if (!index && !len)
		{
			DBGMSG(0, "[LORA/BIN] received invalid data\r\n");
			return;
		}
		else if (!pipe->size())
			wait(1);
		
		if (!pipe->size())
			return;
		_protocol_len = pipe->getc() + 3;
	}
	
	if (_protocol_len > pipe->size())
	{
		uncompletedRespCmd = 1;
		return;
	}

	pipe->get((char*)&respData[0], _protocol_len);
	if (respData[0] != HKMC_PROTOCOL_URYU || respData[_protocol_len - 3] != 0xff || respData[_protocol_len - 2] != 0xff || respData[_protocol_len - 1] != HKMC_ETX)
	{
		_protocol_parsing_step = false;
		return;
	}
	
	switch (respData[1])
	{
	case URYU::eucSWITCH_WORKNO:
		pgConf->updateExfIndices(eetWN, respData[2]);
		pgUryu->put(&respData[1], _protocol_len - 4);
		break;
	case URYU::eucREQ_BOLT_FASTENING:
		{
			uint16_t index = 	(uint16_t)respData[3] | respData[2];
			_bolt_fastening_index = index;
			if(index > 10000)
			{
				index = 10000;
				respData[2] = (uint8_t)(index & 0xff);
				respData[3] = (uint8_t)((index >> 8) & 0xff);
			}
		}
		pgUryu->put(&respData[1], _protocol_len - 4);
		break;
	case URYU::eucREAD_DATE_TIME:
		pgUryu->put(&respData[1], _protocol_len - 4);
		break;
	case URYU::eucWRITE_DATE_TIME:
		pgUryu->put(&respData[1], _protocol_len - 4);
		break;
	}
	lastRecvCmd = respData[1];
	_protocol_parsing_step = false;
}

int16_t LORA::getNumberBySeparator(uint8_t *buff, char sep, uint8_t *offset)
{
	uint16_t num = -1;
	char seqChar;
	int cnt = 0;
	while (cnt < 4 && ((seqChar = buff[cnt]) != sep))
	{
		if (cnt)
			num |= (seqChar - '0') * 10*cnt;
		else
			num = seqChar - '0';
		cnt++;
	}
	if (seqChar == sep)
	{
		*offset = cnt;
		return num;
	}
	return num;
}

static const char	recvHdr[5] = { 'U', 'r', 'd', 'p', ':' };
void LORA::asciiParser(Pipe<char>* pipe)
{
	uint8_t respData[42] = { 0, };
	uint8_t *pNetId = (uint8_t *)&CFG(Setting).netId;
	
	int len = pipe->size();
	int index = 0;
	while (!uncompletedRespCmd && len > 0)
	{
		len--;
		if (pipe->getc() == recvHdr[index++] && index >= 5)
		{
			_protocol_len = 0;
			break;
		}
	}
	if (!len)
	{
		DBGMSG(0, "[LORA/ASCII] received invalid data\r\n");
		return;
	}
	if (pipe->size() < 8)
	{
		uncompletedRespCmd = 1;
		return;
	}
	
	if (pipe->get_((char*)&respData[0], 4) != 4)
	{
		return;
	}
	
	uint16_t seq = 0, cmd = 0;
	if (!memcmp(respData, "Seq:", 4))
	{
		uint8_t offset = 0;
		pipe->get_((char*)&respData[0], (pipe->size() >= sizeof(respData)) ? sizeof(respData) : pipe->size());
		seq = getNumberBySeparator(&respData[4], ':', &offset);
		if (!offset)
		{
			uncompletedRespCmd = 0;
			return;
		}
		pipe->flush(4 + offset + 4 + 1);
	}
	else if (!memcmp(respData, "Cmd:", 4))
	{
		uint8_t offset1 = 0, offset2 = 0;
		pipe->get_((char*)&respData[0], 8);
		if (memcmp(respData, "Cmd:Seq:", 8))
		{
			uncompletedRespCmd = 0;
			return;
		}
		pipe->get_((char*)&respData[0], (pipe->size() >= sizeof(respData)) ? sizeof(respData) : pipe->size());
		cmd = getNumberBySeparator(&respData[8], ':', &offset1);
		if (!offset1)
		{
			uncompletedRespCmd = 0;
			return;
		}
		offset1 = 8 + offset1 + 1;
		if (pipe->size() < (8 + offset1 + 1))
			return;
		seq = getNumberBySeparator(&respData[offset1], ':', &offset2);
		if (!offset2 || respData[offset1 + offset2 + 1] != HKMC_STX || respData[offset1 + offset2 + 2] != pNetId[0] || respData[offset1 + offset2 + 3] != pNetId[1])
		{
			uncompletedRespCmd = 0;
			return;
		}
		if (pipe->size() < (offset1 + offset2 + 4 + 1))
			return;
		_protocol_len = respData[offset1 + offset2 + 4] + 3;
		pipe->flush(offset1 + offset2 + 4 + 1);
	}
	else
	{
		uncompletedRespCmd = 0;
		return;
	}
	_tLora.attach(callback(protocol_timeout), .1f);
	_protocol_parsing_step = true;
	uncompletedRespCmd = 0;
}
