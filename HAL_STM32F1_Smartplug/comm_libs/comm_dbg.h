#pragma once

#include "comm_def.h"

#if _ENABLE_FILE_INFO
#define DEBUG_FILE_INFO cfileinfo(__FILE__, __LINE__, __FUNCTION__),
#else
#define DEBUG_FILE_INFO
#endif

// sync output macro
#define DSYNCOUT			csync_puts

#if _ENABLE_DEBUG_PRINT
#define DPRT(...)				DEBUG_FILE_INFO cprintf(__VA_ARGS__)
#define DUMP(title, data, len)	DEBUG_FILE_INFO chexdump(title, data, len)
#define DOUT(...)				DEBUG_FILE_INFO cprintf(__VA_ARGS__)
#else
#define DPRT(...)
#define DUMP(title, data, len)

#if !defined(_DOUT_DISABLED) || _DOUT_DISABLED == 0
#define DOUT(...)				cprintf(__VA_ARGS__)
#else
#define DOUT(...)
#endif

#endif

#if RELEASE_BUILD == 0
#define ENABLE_DEBUG_ASSERT 1
#else
#define ENABLE_DEBUG_ASSERT 0
#endif

#if RELEASE_BUILD == 0
#define DASSERT(condition) \
  {\
      if(!(condition))\
      {\
          DOUT("assertion failed at %s:%d %s", __FILE__, __LINE__, #condition);\
		  while (!(condition))\
		  	;\
      }\
  }
#else
#define DASSERT(...)
#endif

#define DPRT_STAT DPRT

#define ENABLE_DEBUG_NMS_TXRX 0

void cfileinfo(const char* file, int line, const char *tag);
void cprintf(const char *fmt, ...);
void chexdump(const char* title, const uint8_t* data, size_t len);
void csync_puts(const char *str);