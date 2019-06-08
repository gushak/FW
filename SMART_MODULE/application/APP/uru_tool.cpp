#include "common.h"

const char* URYU::OUTPUT_ENTER_KEY  = "\r";


__ALIGN_BEGIN char gUryuBuff[1024] __ALIGN_END = {0,};
URYU gUryu(URU_TX, URU_RX, 115200, gUryuBuff, sizeof(gUryuBuff));
URYU *pgUryu = NULL;

static void uryu_process();
static void uru_tx_process();
uint8_t gToolBuff[32];
uint8_t szReadData[256] = {0,};

URYU::URYU(PinName tx, PinName rx, int baud, char *rxBuff, int rxSize, char *txBuff, int txSize)
	: SerialPipe(tx, rx, baud, rxBuff, rxSize, txBuff, txSize)
{
	m_nWriteIndex = 0;
	m_nSessionIndex = 0;
	lastSendCmd = 0;

	pgUryu = this;
	_debug_trace_on = false;
	uryu_thread.start(uryu_process);
}

URYU::~URYU()
{
	shutdown_at_parser();
}

//Refer to <svn>/mbed/SmartModule/Documents/DevelRef/FromGeoPlan/TooController_Protocol_v0.1_20180607.pptx
void URYU::init()
{
	bool success = true;
	
	format(8, SerialBase::Parity::Odd, 1);

	DBGMSG(0, "[URYU] init - 1\r\n");
	
}

void uryu_process()
{
	DBGMSG(0, "[URYU] Process Started===>\r\n");

	pgUryu->bKeepRunning = true;
	pgUryu->init();
	wait_ms(100);
	pgUryu->command(URYU::eucSWITCH_TO_HKM_PROTOCOL, 1, 0);
	
	int ret = 0;

	Pipe<char> *pipe = pgUryu->getRxPipe();
	while (pgUryu->bKeepRunning)
	{
		while (BitOpMode & DOM_UART6)
		{
			wait(1);
		}
		if (!pgUryu->readable())
		{
			wait_us(10);
			
			if (pgUryu->timeoutCmd && getElapsedTick(pgUryu->getRespTick()) >= 2000)
			{
				pgUryu->command(pgUryu->timeoutCmd, pgUryu->timeoutAux, 1);
			}
			
			wait_ms(10);
			continue;
		}
		pgUryu->parser(pgUryu->getRxPipe());
		wait_ms(10);
	}
	DBGMSG(0, "[URYU] Terminate \r\n");
	pgUryu->rx_suspend();
}

void uru_tx_process()
{
	while (pgUryu->getTxPipe()->readable())
	{
		pgUryu->send();
	}
	DBGMSG(0, "[URY] Transmitted\r\n");
}

static void parser_abort(ATCmdParser *at)
{
	at->abort();
}

void URYU::setup_at_parser()
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

void URYU::shutdown_at_parser()
{
	delete _at;
	_at = NULL;
}

void URYU::send()
{
	txCopy();
}
int URYU::put(const void* buffer, int length)
{
	getTxPipe()->put((char*)buffer, length);
//	if (transmit_thread.get_state() < Thread::Ready || transmit_thread.get_state() == Thread::Deleted)
//		transmit_thread.start(uru_tx_process);
	send();
}
int URYU::put(Pipe<char> *pipe, int length, int offset)
{
	getTxPipe()->put(pipe, length, offset);
//	if (transmit_thread.get_state() < Thread::Ready || transmit_thread.get_state() == Thread::Deleted)
//		transmit_thread.start(uru_tx_process);
	send();
}

void URYU::command(uint8_t cmd, uint16_t aux, uint8_t unVar)
{
	uint8_t cmdData[16] = { 0, };
	uint8_t nHalfCmd = 0;
	int     size = 0;
	switch (cmd)
	{
		
		case eucSWITCH_WORKNO:
		cmdData[0] = cmd;
		cmdData[1] = (uint8_t)aux;
		cmdData[2] = chkSum(cmdData, 2);
		size = 3;
		lastSendCmd = cmdData[0];
		setTimeoutCmd(lastSendCmd, aux);
		break;
		case eucREQ_BOLT_FASTENING:
		cmdData[0] = cmd;
		cmdData[1] = ((uint8_t *)&aux)[0];
		cmdData[2] = ((uint8_t *)&aux)[1];
		cmdData[3] = unVar;
		cmdData[4] = 0;
		cmdData[5] = (chkSum(cmdData, 5) + 2);
		size = 6;
		lastSendCmd = cmdData[0];
		setTimeoutCmd(lastSendCmd , aux);
		break;
		case eucCLOSE_SESSION:
		cmdData[0] = cmd;
		cmdData[1] = cmdData[0];
		size = 2;
		lastSendCmd = cmdData[0];
		setTimeoutCmd(lastSendCmd, aux);
		break;
		case eucSWITCH_TO_HKM_PROTOCOL:
		cmdData[0] = cmd;
		cmdData[1] = cmdData[0];
		size = 2;
		lastSendCmd = cmdData[0];
		setTimeoutCmd(lastSendCmd, aux);
		break;
		default:
		return;
	}
		
	put(cmdData, size);

	
	setRespTick(getTickCount());

}
void URYU::parser(Pipe<char>* pipe)
{
	uint8_t respId;
	uint8_t *respData = gToolBuff;
	
	int len = pipe->size();
	DBGMSG(0, "[len] %d\r\n ", len);
	if (len < 2)
		return;
	while (!uncompletedRespCmd && len > 0)
	{
		len--;
		respId = pipe->getc();
		DBGMSG(0, "[respId] %d [len] %d\r\n ", respId, len);
		if (lastSendCmd == (respId & 0x7F))
		{
			uncompletedRespCmd = respId;
			break;
		}
	}
	if (!len)
	{
		DBGMSG(0, "[URYU] received invalid data\r\n");
		return;
	}
	
	memset(gToolBuff, 0, sizeof(gToolBuff));
	respData[0] = respId;
	len = pipe->get_((char*)&respData[1], len);
	if (respId & 0x80)	//Handle errors
	{
		if (len < 2)
			return;
		len = pipe->get((char*)respData, 2);
		switch (respId & 0x7F)
		{
		case eucSWITCH_WORKNO:
			DBGMSG(0, "[URYU] Error SWITCH_WORKNO code = 0x%02x\r\n", respData[0]);
			pgLora->put(respData, 3);
			break;
		case eucREQ_BOLT_FASTENING:
			DBGMSG(0, "[URYU] Error REQ_BOLT_FASTENING code = 0x%02x\r\n", respData[0]);
			pgLora->put(respData, 3);
			break;
		case eucCLOSE_SESSION:
			DBGMSG(0, "[URYU] Error CLOSE_SESSION code = 0x%02x\r\n", respData[0]);
			break;
		case eucREAD_DATE_TIME:
			DBGMSG(0, "[URYU] Error READ_DATE_TIME code = 0x%02x\r\n", respData[0]);
			pgLora->put(respData, 3);
			break;
		case eucWRITE_DATE_TIME:
			DBGMSG(0, "[URYU] Error WRITE_DATE_TIME code = 0x%02x\r\n", respData[0]);
			pgLora->put(respData, 3);
			break;
		case eucSWITCH_TO_HKM_PROTOCOL:
			DBGMSG(0, "[URYU] Error SWITCH_TO_HKM_PROTOCOL code = 0x%02x\r\n", respData[0]);
			break;
		default:
			break;
		}
		uncompletedRespCmd = 0;
		return;			
	}

	uint8_t chkLen = 0;
	switch (respId & 0x7F)
	{
	case eucSWITCH_WORKNO:
		chkLen = 5;
		if (len < chkLen - 1)
			return;	
		DBGDMP(0, "SW]", respData, chkLen);
		if (chkSum(respData, chkLen - 1) == respData[chkLen - 1])
		{
			boltIndex = *(uint16_t*)&respData[1];
			
			if (m_nSessionIndex)
			{
				command(eucREQ_BOLT_FASTENING, ++boltIndex, 1);
				
			}
			else if(!m_nSessionIndex)
			{
				command(eucCLOSE_SESSION, 1, 0);
			}
		}
		if (pgLora->lastRecvCmd == eucSWITCH_WORKNO)
			pgLora->put(respData, chkLen);
		break;
	case eucREQ_BOLT_FASTENING:
		chkLen = 29;
		if (len < chkLen - 1)
			return;	

		DBGDMP(0, "RBF]", respData, chkLen);
		
		if (chkSum(respData, chkLen - 1) == respData[chkLen - 1])
		{
			command(eucCLOSE_SESSION, 1, 0);
		}
		if (pgLora->lastRecvCmd == eucREQ_BOLT_FASTENING)
		{
			respData[1] = (uint8_t)(pgLora->_bolt_fastening_index & 0xff);
			respData[2] = (uint8_t)((pgLora->_bolt_fastening_index >> 8) & 0xff);
			pgLora->put(respData, chkLen);
		}
		break;
	case eucCLOSE_SESSION:
		chkLen = 2;
		DBGDMP(0, "CS]", respData, chkLen);
		if (chkSum(respData, chkLen - 1) == respData[chkLen - 1])
		{
			command(eucREQ_BOLT_FASTENING, ++boltIndex, 1);		
			m_nSessionIndex++;
		}	
		break;
	case eucREAD_DATE_TIME:
		chkLen = 8;
		if (pgLora->lastRecvCmd == eucREAD_DATE_TIME)
			pgLora->put(respData, chkLen);
		break;
	case eucWRITE_DATE_TIME:
		chkLen = 29;
		if (pgLora->lastRecvCmd == eucWRITE_DATE_TIME)
			pgLora->put(respData, chkLen);
		break;
	case eucSWITCH_TO_HKM_PROTOCOL:
		chkLen = 2;
		DBGDMP(0, "SP]", respData, chkLen);
		if (chkSum(respData, chkLen - 1) == respData[chkLen - 1])
		{
			command(eucSWITCH_WORKNO, CTX_EXF(wn), 0);	
		}
		break;
	default:
		break;
	}
	uncompletedRespCmd = 0;
	pipe->flush(chkLen - 1);
}

uint8_t URYU::chkSum(uint8_t * buff, int len)
{
	uint8_t chk_sum = 0;
	int loop = 0;
	while (loop < len)
	{
		chk_sum ^= buff[loop++];
	}
	return chk_sum;
}