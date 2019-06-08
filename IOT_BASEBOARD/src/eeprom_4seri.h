#ifndef _EEPROM_4SERIAL_H_
#define _EEPROM_4SERIAL_H_

#include "stm32f4xx_hal.h"   
#include "stm32f4xx_hal_def.h"

#define MAX_EEPROM_ADDRESS  45

#define MAX_EEPROM_MENU         15

#define SW_LONG_CNT	        50

//typedef struct Config_Struct CONFIG_STRUCT;
typedef struct 
{	
    uint16_t setup;
    uint16_t Value[MAX_EEPROM_ADDRESS];
}Config_Struct;

extern void lll_eeprom_init(void);
extern void eeprom_default_value(void);
extern uint8_t load_eeprom(void);
extern void save_all_eeprom(void);
extern void save_eeprom(uint8_t adr, uint16_t data);

extern Config_Struct Config;          //s 추후 풀어야함

//#define COW_NUM	volatile uint8_t Menu = 0;  



#define CONFIG_FARM_PREFERENCE_1 		Config.Value[0]
#define CONFIG_RF_DEVICE_COUNT_2 		Config.Value[1]
#define CONFIG_RF_TYPE_3         		Config.Value[2]
#define CONFIG_PASSWORD1_4       		Config.Value[3]
#define CONFIG_PASSWORD2_5       		Config.Value[4]
#define CONFIG_LORA_SENSOR_DCODE_6 		Config.Value[5]
#define CONFIG_LORA_WATER_VALUE_DCODE_7  	Config.Value[6]
#define CONFIG_LORA_WEIGHT_DCODE_8       	Config.Value[7]
#define CONFIG_RF_FREQ_ADDR1_9     		Config.Value[8]
#define CONFIG_RF_FREQ_ADDR2_10                 Config.Value[9]
#define CONFIG_FEEDER_TIME_11     		Config.Value[10]
#define CONFIG_LIGHT_TIME_12     		Config.Value[11]
#define CONFIG_MOVING_TIME_13     		Config.Value[12]

#define CONFIG_FEEDER_TIME_DATA_POS     	15

#define CONFIG_LIGHT_TIME_ON1			Config.Value[35]
#define CONFIG_LIGHT_TIME_OFF1			Config.Value[36]
#define CONFIG_LIGHT_TIME_ON2			Config.Value[37]
#define CONFIG_LIGHT_TIME_OFF2			Config.Value[38]

#define CONFIG_HEATER_TEMP_ON1			Config.Value[39]
#define CONFIG_HEATER_TEMP_OFF1			Config.Value[40]
#define CONFIG_HEATER_TEMP_ON2			Config.Value[41]
#define CONFIG_HEATER_TEMP_OFF2			Config.Value[42]
#define CONFIG_HEATER_TEMP_ON3			Config.Value[43]
#define CONFIG_HEATER_TEMP_OFF3			Config.Value[44]
#endif
