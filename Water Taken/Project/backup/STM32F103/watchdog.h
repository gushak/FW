#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

extern void Wdt_reset(void);
extern uint8_t Wdt_init(uint16_t timeout_sec);


#endif
