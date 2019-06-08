#include "SerialPipe.h"
#include "common.h"

SerialPipe::SerialPipe(PinName tx, PinName rx, int baudrate, char *rxBuff, int rxSize, char *txBuff, int txSize)
	: _SerialPipeBase(tx, rx, baudrate)
	, _pipeRx((rx != NC) ? rxSize : 0, rxBuff)
	, _pipeTx((tx != NC) ? txSize : 0, txBuff)
{
	if (rx != NC)
		wait_us(10);
		attach(callback(this, &SerialPipe::rxIrqBuf), RxIrq);
}

SerialPipe::~SerialPipe(void)
{
    attach(NULL, RxIrq);
    attach(NULL, TxIrq);
}

int SerialPipe::close()
{
    /* Does not let us pass a file descriptor. So how to close ?
     * Also, does it make sense to close a device type file descriptor*/
	return 0;
}

int SerialPipe::isatty()
{
	return 1;

}

off_t SerialPipe::seek(off_t offset, int whence)
{
    /*XXX lseek can be done theoratically, but is it sane to mark positions on a dynamically growing/shrinking
     * buffer system (from an interrupt context) */
	return -1;
}

int SerialPipe::sync()
{
	while (!_pipeTx.empty()) {
		// Doing better than wait would require TxIRQ to also do wake() when becoming empty. Worth it?
		wait_ms(1);

	}
	return 0;
}

void SerialPipe::sigio(Callback<void()> func) {
	core_util_critical_section_enter();
	_sigio_cb = func;
	if (_sigio_cb) {
		short current_events = poll(0x7FFF);
		if (current_events) {
			_sigio_cb();
		}
	}
	core_util_critical_section_exit();
}

ssize_t SerialPipe::write(const void* buffer, size_t length)
{
	return put(buffer, length, _blocking);
}

ssize_t SerialPipe::read(void* buffer, size_t length)
{
	return get(buffer, length);
}

bool SerialPipe::hup() const
{
	return false;
}

void SerialPipe::wake()
{
	if (_sigio_cb) {
		_sigio_cb();
	}
}

short SerialPipe::poll(short events) const 
{

	short revents = 0;
	/* Check the Circular Buffer if space available for writing out */

	if (!_pipeRx.empty()) {
		revents |= POLLIN;
	}

    /* POLLHUP and POLLOUT are mutually exclusive */
	if (hup()) {
		revents |= POLLHUP;
	}
	else if (!_pipeTx.full()) {
		revents |= POLLOUT;
	}

	    /*TODO Handle other event types */

	return revents;
}

// tx channel
int SerialPipe::writeable(void)    
{
    return _pipeTx.free();
}

int SerialPipe::putc(int c)    
{
    c = _pipeTx.putc(c);
    txStart();
    return c;
}

int SerialPipe::put(const void* buffer, int length, bool blocking)    
{ 
    int count = length;
    const char* ptr = (const char*)buffer;
	if (!count)
		return 0;
    do
    {
        int written = _pipeTx.put(ptr, count, false);
        if (written) {
            ptr += written;
            count -= written;
            txStart();
        }
        else if (!blocking)
            break;
        /* nothing / just wait */;
    }
    while (count);

	return (length - count);
}

int SerialPipe::put(Pipe<char> *pipe, int length, int offset, bool blocking)
{
	int count = length;
	if (!count)
		return 0;
	do
	{
		int written = _pipeTx.put(pipe, count, offset, false);
		if (written) {
			offset = 0;
			count -= written;
			txStart();
		}
		else if (!blocking)
			break;
		/* nothing / just wait */
		;
	} while (count);

	return (length - count);
}

void SerialPipe::txOne(void)
{
	if (_SerialPipeBase::writeable() && _pipeTx.readable())
	{
		char c = _pipeTx.getc();
		printf("[txOne]%c,", c);
		_SerialPipeBase::_base_putc(c);
	}
}
#include "debug.h"
void SerialPipe::txCopy(void)
{
	pgDebug->DebugPrintf(0, "\r\n[");
	while (_SerialPipeBase::writeable() && _pipeTx.readable())
    {
        char c = _pipeTx.getc();
        _SerialPipeBase::_base_putc(c);
	    pgDebug->DebugPrintf(0, " %02x ", c);
    }
	pgDebug->DebugPrintf(0, "]\r\n");
}

void SerialPipe::txIrqBuf(void)
{
    txCopy();
    // detach tx isr if we are done 
    if (!_pipeTx.readable())
        attach(NULL, TxIrq);
}

void SerialPipe::txStart(void)
{
    // disable the tx isr to avoid interruption
    attach(NULL, TxIrq);
    txCopy();
    // attach the tx isr to handle the remaining data
    if (_pipeTx.readable())
        attach(callback(this, &SerialPipe::txIrqBuf), TxIrq);
}

// rx channel
int SerialPipe::readable(void)                      
{ 
    return _pipeRx.readable(); 
} 

int SerialPipe::getc(void)                          
{ 
    if (!_pipeRx.readable())
        return EOF;
    return _pipeRx.getc(); 
} 

int SerialPipe::get(void* buffer, int length, bool blocking) 
{ 
    return _pipeRx.get((char*)buffer,length,blocking); 
}
int SerialPipe::get_(void* buffer, int length, bool blocking) 
{ 
	return _pipeRx.get_((char*)buffer, length, blocking); 
}

void SerialPipe::rxIrqBuf(void)
{ 
    while (_SerialPipeBase::readable())
    {
        char c = _SerialPipeBase::_base_getc();
	    if (_pipeRx.writeable())
	    {
		    _pipeRx.putc(c);   
	    }
	    else 
            /* overflow */;
    }
}
void SerialPipe::rx_suspend(void)
{
	attach(NULL, RxIrq);
}
void SerialPipe::rx_resume(void)
{
	attach(callback(this, &SerialPipe::rxIrqBuf), RxIrq);
}