#include <string.h>
#include <stdlib.h>
#include "comm_sys.h"

static uint32_t milli_tick_ = 0; //Tick count on each 100 millis
static uint32_t uptime_seconds_ = 0; // uptime seconds

void delay_1ms()
{
        uint16_t empty_loop;
  
  for(empty_loop = 0; empty_loop < 0x2800; empty_loop++)
  {
#if !defined(WIN32)
          asm(" nop ");
#endif
  }
}
/*
* Millisecond delay
*/
void delay_ms(uint32_t millis)
{
        for(; millis != 0; millis--)
            delay_1ms();
}
/*
* set initial tick value
*/
void sys_tick_set(uint32_t tick_value)
{
        milli_tick_ = tick_value;
}

/*
** increase system tick
*/

void sys_tick_inc()
{
	static uint16_t sec_count = 0;

	++milli_tick_;
	if (++sec_count == 1000)
	{
		sec_count = 0;
		++uptime_seconds_;
	}
}

/*
**get system milli second tick
*/
uint32_t sys_tick_count()
{
        return milli_tick_;
}

/*
**compute defference of tick
*/
uint32_t sys_tick_diff(uint32_t test_tick, uint32_t prev_tick)
{
	if (test_tick < prev_tick)	// tick overflow
        {
		return (0xFFFFFFFF - prev_tick) + 1 + test_tick;
        }
	else
        {
		return test_tick - prev_tick;

        }
}

/*
** get uptime in seconds
*/
uint32_t sys_uptime()
{
	return uptime_seconds_;
}

