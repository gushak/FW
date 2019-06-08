#ifndef _RFID_READER_H_
#define _RFID_READER_H_

extern void parse_rfid(uint8_t c);
extern uint8_t rfid_is_empty(void);
extern u16 rfid_dequeue(void);
extern void rfid_enqueue(u16 data);
extern void parse_rfid_08IMF(uint8_t c);

#endif

