#pragma once 

#include "mbed.h"
#include "Pipe.h"

#define _SerialPipeBase SerialBase //!< base class used by this class

/** Buffered serial interface (rtos capable/interrupt driven)
*/
class SerialPipe : public _SerialPipeBase, public FileHandle
{
public:
    /** Constructor
        \param tx the trasmitting pin
        \param rx the receiving pin
        \param baud the initial baudrate 
        \param rxSize the size of the receiving buffer
        \param txSize the size of the transmitting buffer
    */
	SerialPipe(PinName tx, PinName rx, int baudrate, char *rxBuff = NULL, int rxSize = 128, char *txBuff = NULL, int txSize = 128);

    /** Destructor
    */
    virtual ~SerialPipe(void);
    
	// FileHandle
    //---------------------------------------------------->
   /** Equivalent to POSIX poll(). Derived from FileHandle.
     *  Provides a mechanism to multiplex input/output over a set of file handles.
     */
	virtual short poll(short events) const;

	    /* Resolve ambiguities versus our private SerialBase
	     * (for writable, spelling differs, but just in case)
	     */
	using FileHandle::readable;
	using FileHandle::writable;

	/** Write the contents of a buffer to a file
     *
     *  @param buffer   The buffer to write from
     *  @param length   The number of bytes to write
     *  @return         The number of bytes written, negative error on failure
     */
	virtual ssize_t write(const void* buffer, size_t length);

	/** Read the contents of a file into a buffer
	 *
	 *  Follows POSIX semantics:
	 *
	 *  * if no data is available, and non-blocking set return -EAGAIN
	 *  * if no data is available, and blocking set, wait until data is available
	 *  * If any data is available, call returns immediately
	 *
	 *  @param buffer   The buffer to read in to
	 *  @param length   The number of bytes to read
	 *  @return         The number of bytes read, 0 at end of file, negative error on failure
	 */
	virtual ssize_t read(void* buffer, size_t length);

	/** Close a file
	 *
	 *  @return         0 on success, negative error code on failure
	 */
	virtual int close();

	/** Check if the file in an interactive terminal device
	 *
	 *  @return         True if the file is a terminal
	 *  @return         False if the file is not a terminal
	 *  @return         Negative error code on failure
	 */
	virtual int isatty();

	/** Move the file position to a given offset from from a given location
	 *
	 * Not valid for a device type FileHandle like UARTSerial.
	 * In case of UARTSerial, returns ESPIPE
	 *
	 *  @param offset   The offset from whence to move to
	 *  @param whence   The start of where to seek
	 *      SEEK_SET to start from beginning of file,
	 *      SEEK_CUR to start from current position in file,
	 *      SEEK_END to start from end of file
	 *  @return         The new offset of the file, negative error code on failure
	 */
	virtual off_t seek(off_t offset, int whence);

	/** Flush any buffers associated with the file
	 *
	 *  @return         0 on success, negative error code on failure
	 */
	virtual int sync();

	/** Set blocking or non-blocking mode
     *  The default is blocking.
     *
     *  @param blocking true for blocking mode, false for non-blocking mode.
     */
	virtual int set_blocking(bool blocking)	{	_blocking = blocking;	return 0;	}

	/** Register a callback on state change of the file.
     *
     *  The specified callback will be called on state changes such as when
     *  the file can be written to or read from.
     *
     *  The callback may be called in an interrupt context and should not
     *  perform expensive operations.
     *
     *  Note! This is not intended as an attach-like asynchronous api, but rather
     *  as a building block for constructing  such functionality.
     *
     *  The exact timing of when the registered function
     *  is called is not guaranteed and susceptible to change. It should be used
     *  as a cue to make read/write/poll calls to find the current state.
     *
     *  @param func     Function to call on state change
     */
	virtual void sigio(Callback<void()> func);

	// FileHandle
    //<----------------------------------------------------

    // tx channel
    //----------------------------------------------------
    
    /** check if writable 
        return the number of free bytes
    */
    int writeable(void);
    
    /** send a character (blocking)
        \param c the character to send
        \return c
    */
    int putc(int c);
    
    /** send a buffer
        \param buffer the buffer to send
        \param length the size of the buffer to send
        \param blocking, if true this function will block 
               until all bytes placed in the buffer. 
        \return the number of bytes written 
    */
    int put(const void* buffer, int length, bool blocking=false);
	int put(Pipe<char> *pipe, int length = 0, int offset=0, bool blocking = false);

	Pipe<char>* getTxPipe() {	return (Pipe<char>*)&_pipeTx; }
	uint8_t* getTxDataBuffer() {	return (uint8_t*)_pipeTx.getBuffer(); }
	int getTxSize()		{	return _pipeTx.size();	}
	void flushTx(int size) {	_pipeTx.flush(size); }
	void txOne(void);

    // rx channel
    //----------------------------------------------------
    
    /** check if readable
        \return the size available in the buffer.
    */
    int readable(void);

    /** receive one character from the serial port (blocking)
        \param the character received 
    */
    int getc(void);
    
    /** read a buffer from the serial port
        \param pointer to the buffer to read.
        \param length number of bytes to read 
        \param blocking true if all bytes shall be read. false if only the available bytes.
        \return the number of bytes read.
    */
    int get(void* buffer, int length, bool blocking=false);
	int get_(void* buffer, int length, bool blocking = false);
	Pipe<char>* getRxPipe() {	return (Pipe<char>*)&_pipeRx; }
	uint8_t* getRxDataBuffer() {	return (uint8_t*)_pipeRx.getBuffer(); }
	int getRxSize()		{	return _pipeRx.size();	}
	void reset_rxpipe(void)	{	_pipeRx.init();	}
	void flush(int size) {	_pipeRx.flush(size);	}
	void rx_suspend(void);
	void rx_resume(void);
protected:
	void rxIrqBuf(void);
    //! receive interrupt routine
    //! transmit interrupt woutine 
    void txIrqBuf(void);
    //! start transmission helper
    void txStart(void);
    //! move bytes to hardware
    void txCopy(void);
    Pipe<char> _pipeRx; //!< receive pipe
    Pipe<char> _pipeTx; //!< transmit pipe
private:
	Callback<void()> _sigio_cb;

	bool _blocking;
	    /** Device Hanged up
     *  Determines if the device hanged up on us.
     *
     *  @return True, if hanged up
     */
	bool hup() const;
	void wake(void);

};
