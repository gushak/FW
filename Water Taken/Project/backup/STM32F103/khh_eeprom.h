#ifndef _KHH_EEPROM_H_
#define _KHH_EEPROM_H_

#define MAX_EEPROM_ADDRESS	11

#define EEPROM_DATA_START_POS 	0	// allows moving (not really necessary)
#define EEPROM_CHECK_VALUE	0x1127 
#define EEPROM_WRITE_DELAY	50

typedef struct Config_Struct CONFIG_STRUCT;
struct Config_Struct
{	
	uint16_t setup;
	uint16_t Value[MAX_EEPROM_ADDRESS];
};

extern void lll_eeprom_init(void);
extern void eeprom_default_value(void);
extern void load_eeprom(void);
extern void save_all_eeprom(void);
extern void save_eeprom(uint8_t adr, uint16_t data);

extern CONFIG_STRUCT Config;



#endif
