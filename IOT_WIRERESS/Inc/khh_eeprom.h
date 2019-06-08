#ifndef _KHH_EEPROM_H_
#define _KHH_EEPROM_H_

#include "stm32f1xx_hal.h"   
#include "stm32f1xx_hal_def.h"

#define MAX_EEPROM_ADDRESS	40

//typedef struct Config_Struct CONFIG_STRUCT;
typedef struct 
{	
  uint16_t setup;
  uint16_t Value[MAX_EEPROM_ADDRESS];
}Config_Struct;

uint8_t load_eeprom(void);
extern void lll_eeprom_init(void);
extern void eeprom_default_value(void);
void eeprom_pend_default_value(void);
void eeprom_8ch_limit_default_value(void);
extern void save_all_eeprom(void);
extern void save_eeprom(uint8_t adr, uint16_t data);

extern Config_Struct Config; 



#define CONFIG_FARM_PREFERENCE					Config.Value[0]	//s 농장 설정 정보
#define CONFIG_DEVIEC_TPYE                             		Config.Value[1]	//s 1:16ch/  2::8CH /3:계류식
#define CONFIG_RF_ADDR                                                Config.Value[2]
#define CONFIG_RF_TYPE                                                Config.Value[3]
#define CONFIG_RELAY_TIME1                                            Config.Value[4]
#define CONFIG_RELAY_TIME2                                            Config.Value[5]
#define CONFIG_RELAY_TIME3                                            Config.Value[6]
#define CONFIG_RELAY_TIME4                                            Config.Value[7]
#define CONFIG_RELAY_TIME5                                            Config.Value[8]
#define CONFIG_RELAY_TIME6                                            Config.Value[9]
#define CONFIG_RELAY_TIME7                                            Config.Value[10]
#define CONFIG_RELAY_TIME8                                            Config.Value[11]
#define CONFIG_RELAY_TIME9                                            Config.Value[12]
#define CONFIG_RELAY_TIME10                                           Config.Value[13]
#define CONFIG_RELAY_TIME11                                           Config.Value[14]
#define CONFIG_RELAY_TIME12                                           Config.Value[15]
#define CONFIG_RELAY_TIME13                                           Config.Value[16]
#define CONFIG_RELAY_TIME14                                           Config.Value[17]
#define CONFIG_RELAY_TIME15                                           Config.Value[18]
#define CONFIG_RELAY_TIME16                                           Config.Value[19]
#define CONFIG_MOVING_COUNT                                           Config.Value[20]
#define CONFIG_8CH_REVERSE                                           Config.Value[21]
#define CONFIG_RESET_PROCESS                                           Config.Value[22]
#define CONFIG_RF_FREQ_ADDR1                                           Config.Value[23]
#define CONFIG_RF_FREQ_ADDR2                                           Config.Value[24]
#define CONFIG_RESET_SAVE                                           Config.Value[30]
#define CONFIG_RELAY_SAVE                                             Config.Value[31]
#define CONFIG_PANDING_SAVE                                           Config.Value[32]
#define CONFIG_PANDING_TIME1                                           Config.Value[33]
#define CONFIG_PANDING_TIME2                                           Config.Value[34]
#define CONFIG_PANDING_TIME3                                           Config.Value[35]
#endif
