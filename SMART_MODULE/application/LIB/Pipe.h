#pragma once 

/** pipe, this class implements a buffered pipe that can be savely 
    written and read between two context. E.g. Written from a task 
    and read from a interrupt.
*/
template <class T>
class Pipe
{
public:
    /* Constructor
        \param n size of the pipe/buffer
        \param b optional buffer that should be used. 
                 if NULL the constructor will allocate a buffer of size n. 
    */
    Pipe(int n, T* b = NULL)
    {
        _a = b ? NULL : n ? new T[n] : NULL;
        _r = 0;
        _w = 0;
        _b = b ? b : _a;
        _s = n;
	    bFull = false;
    }    
    /** Destructor 
        frees a allocated buffer.
    */
    ~Pipe(void)
    {
        if (_a) 
            delete [] _a;
    }
    
    /* This function can be used during debugging to hexdump the 
       content of a buffer to the stdout. 
    */
    void dump(void)
    {
        int o = _r;
        printf("pipe: %d/%d ", size(), _s);
        while (o != _w) {
            T t = _b[o]; 
            printf("%0*X", sizeof(T)*2, t);
            o = _inc(o); 
        }
        printf("\n");
    }
    
    // writing thread/context API
    //------------------------------------------------------------- 
	bool empty(void) const {	return (bFull ||(_w != _r)) ? false : true;	}
	bool full(void)	const {	return bFull;	}
    /** Check if buffer is writeable (=not full)
        \return true if writeable
    */
    bool writeable(void)
    {
        return free() > 0;
    }
    
    /** Return the number of free elements in the buffer 
        \return the number of free elements
    */
    int free(void)
    {
        int s = _r - _w;
	    if (!bFull && s <= 0)
            s += _s;
        return s;
    }
    
    /* Add a single element to the buffer. (blocking)
        \param c the element to add.
        \return c
    */
    T putc(T c)
    {
        int i = _w;
        int j = i;
        i = _inc(i);
        //while (i == _r);	/* nothing / just wait */
	    if (i == _r)
		    bFull = true;
	    else if (bFull)
		    _o = _r = i;
	    
        _b[j] = c;
        _w = i; 
        return c;
    }
    
    /* Add a buffer of elements to the buffer.
        \param p the elements to add
        \param n the number elements to add from p
        \param t set to true if blocking, false otherwise
        \return number elements added 
    */
    int put(const T* p, int n, bool t = false)
    {
        int c = n;
        while (c)
        {
            int f;
            for (;;) // wait for space
            {
                f = free();
                if (f > 0) break;     // data avail
                if (!t) return n - c; // no more space and not blocking
                /* nothing / just wait */;
            }
            // check free space
            if (c < f) f = c;
            int w = _w;
            int m = _s - w; 
            // check wrap
            if (f > m) f = m;
            memcpy(&_b[w], p, f);
            _w = _inc(w, f);
            c -= f;
            p += f;
        }
	    if (_w == _r)
		    bFull = true;
	    
        return n - c;
    }
    
	int put(Pipe<T> *pipe, int n = 0, int offset = 0, bool t = false)
	{
		int total = n ? n : pipe->size();
		if (!total)
			return 0;
	
		if (!n)
			n = total;
	
		char * buffs[3] = { NULL, }, *curBuff;
		int index = 0, count;
		count = pipe->getBuffer(buffs, total);
		if (offset < count)
			count -= offset;
		else
			index++;
		curBuff = buffs[index];
		while (buffs[index] && index < 2)
		{
			int written = put(curBuff + offset, count, t);
			if (written != count)
			{
				offset = written;
				count -= written;
				total -= written;
				wait_ms(1);
				continue;
			}
			index++;
			if (buffs[index]) {
				curBuff = buffs[index];
				offset = 0;
				total -= count;
				count = total;
			}
		}
		pipe->flush(n - total);
		return (n - total);
	}

    // reading thread/context API
    // --------------------------------------------------------
    
    /** Check if there are any emelemnt available (readble / not empty)
        \return true if readable/not empty
    */
    bool readable(void)
    {
        return (_r != _w);
    }
    
    /** Get the number of values available in the buffer
        return the number of element available
    */
    int size(void)
    {
        int s = _w - _r;
        if (bFull || s < 1)
            s += _s;
        return s;
    }
    
    /** get a single value from buffered pipe (this function will block if no values available)
        \return the element extracted
    */
    T getc(void)
    {
        int r = _r;
        while (!bFull && r == _w) // = !readable()
            /* nothing / just wait */;
	    if (bFull)	bFull = false;
        T t = _b[r];
        _o = _r = _inc(r);
        return t;
    }
    
    /*! get elements from the buffered pipe
        \param p the elements extracted
        \param n the maximum number elements to extract
        \param t set to true if blocking, false otherwise
        \return number elements extracted
    */
    int get(T* p, int n, bool t = false)
    {
        int c = n;
        while (c)
        {
            int f;
            for (;;) // wait for data
            {
                f = size();
                if (f)  break;        // free space
                if (!t) return n - c; // no space and not blocking
                /* nothing / just wait */;
            }
	        
	        if (bFull)	bFull = false;
            // check available data
            if (c < f) f = c;
            int r = _r;
            int m = _s - r; 
            // check wrap
            if (f > m) f = m;
            memcpy(p, &_b[r], f);
	        _o = _r = _inc(r, f);
            c -= f;
            p += f;
        }
        return n - c;
    }
    /*! get elements from the buffered pipe without increasing read pointer
        \param p the elements extracted
        \param n the maximum number elements to extract
        \param t set to true if blocking, false otherwise
        \return number elements extracted
    */
	int get_(T* p, int n, bool t = false)
	{
		int c = n;
		int r = _r;
		while (c)
		{
			int f;
			for (;;) // wait for data
			{
				f = size();
				if (f)  break;        // free space
				if (!t) return n - c; // no space and not blocking
				/* nothing / just wait */
				;
			}
			// check available data
			if (c < f) f = c;
			int m = _s - r; 
			// check wrap
			if (f > m) f = m;
			memcpy(p, &_b[r], f);
			r = _inc(r, f);
			c -= f;
			p += f;
		}
		return n - c;
	}

	T* getBuffer() {	return &_b[_r];	}
	int getBuffer(T** buff, int size)
	{	
		if (_r < _w || (_r + size) < _s)
		{
			buff[0] = &_b[_r % _s];
			return 0;			
		}
		else // _w < _r
		{
			buff[0] = &_b[_r % _s];
			buff[1] = &_b[0];
			return _s - _r;	
		}
	}
	//==================================================================================
	// You should check its feasibility with isOneshotReadable/isOneshotWritable() or forward_size(), 
	// Before buffer pointer is used directly without copying buffer to the local area.
	//==================================================================================
	bool isOneshotReadable(int size) {	return _s > (_r + size) ? true : false;	}
	bool isOneshotWritable(int size) {	return _s > (_w + size) ? true : false;	}
	int forward_size(int * size_buff)
	{
		if (_r < _w)
		{
			size_buff[0] = _w - _r;
			return 1;
		}
		else	// _w < _r
		{	
			size_buff[0] = _s - _r;
			size_buff[1] = _w;
			return 2;
		}
	}
	int getRdPointer()	{	return _r;	}
	int getWrPointer()	{	return _w;	}
    // the following functions are useful if you like to inspect 
    // or parse the buffer in the reading thread/context
    // --------------------------------------------------------
    
    /** set the parsing index and return the number of available 
        elments starting this position.
        \param ix the index to set.
        \return the number of elements starting at this position 
    */
    int set(int ix) 
    {
        int sz = size();
        ix = (ix > sz) ? sz : ix;
        _o = _inc(_r, ix); 
        return sz - ix;
    }
	void reset()
	{
		_o = _r;
	}
	int packet_size()
	{
		if (_r < _o)
			return _o - _r;
		return _s - _r + _o;
	}
    
    /** get the next element from parsing position and increment parsing index
        \return the extracted element.
    */
    T next(void)
    {
        int o = _o;
        T t = _b[o]; 
        _o = _inc(o); 
        return t; 
    }
	T get(int ix)
	{
		int o = _r + ix;
		T t = _b[o]; 
		return t; 
	}
	
    /** get the next element from parsing position and increment parsing index
		\param n : the size of data to get.
		\param data : the pointer of data buffer.
        \return the extracted element.
    */
	void next(int n, T* data)
	{
		int o = _o;
		int count = 0;
		while (count < n)
		{
			data[count++] = _b[o]; 
			o = _inc(o);			
		}
	}
    
    /** commit the index, mark the current parsing index as consumed data.
    */
    void done(void) 
    {
        _r = _o; 
    }
    // <--------------------------------------------------------
    // the above functions are useful if you like to inspect 
    // or parse the buffer in the reading thread/context
	
	
	void init(void)
	{
		_r = 0;
		_w = 0;
		_o = 0;
		bFull = false;
	}
	void flush() {	
		_o = _r = _w; 
		bFull = false;
	}
	void flush(int length) {	
		if (!length)
			return;
		if (size() < length)
			length = size();
		int r = _r;
		_r = _inc(r, length); 
		_o = _r;
		bFull = false;
	}
private:
    /** increment the index
        \param i index to increment
        \param n the step to increment
        \return the incremented index.
    */
    inline int _inc(int i, int n = 1)
    {
        i += n;
	    i %= _s;
        return i;
    }

    T*            _b; //!< buffer
    T*            _a; //!< allocated buffer
    int           _s; //!< size of buffer (s - 1) elements can be stored
    volatile int  _w; //!< write index 
    volatile int  _r; //!< read index 
    int           _o; //!< offest index used by parsing functions
	bool			bFull;
};
