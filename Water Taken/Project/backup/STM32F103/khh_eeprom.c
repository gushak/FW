#include "hw_control.h"

CONFIG_STRUCT Config, Config_Old;		// Holds configuration (from eeProm)


uint16_t VirtAddVarTab[NumbOfVar];	


#ifndef MRAM_EEPROM_ENABLE

void lll_eeprom_init(void)
{
	FLASH_Unlock();
	EE_Init();
	
	uint16_t i = 0;
	for(i=0;i<NumbOfVar;i++)
	{
		VirtAddVarTab[i] = i;
	}
}

void eeprom_default_value(void)		
{

	//Menu_test = 101;
	CON_ADDRESS = CON_ADDRESS_DEFAULT;				//기기주소

}

void load_eeprom(void)
{
	int i = 0;
	Read_eeprom(0, Config.setup);
	if(Config.setup != EEPROM_CHECK_VALUE)
	{
		Config.setup = EEPROM_CHECK_VALUE;
		eeprom_default_value(); 
		save_all_eeprom(); 	
	} else {
		for(i=0;i<MAX_EEPROM_ADDRESS; i++)
		{
			Read_eeprom(i+1, Config.Value[i]);
		}
	}
}

void save_all_eeprom(void)
{
	int i = 0;
	Write_eeprom(0, EEPROM_CHECK_VALUE);	//Config.setup
	delay_ms(EEPROM_WRITE_DELAY);
	
	for(i=0;i<MAX_EEPROM_ADDRESS;i++)
	{
		Config_Old.Value[i] = Config.Value[i];	//flush 영역이므로 데이터값이 잘못 될 경우를 대비하여 백업
		Write_eeprom(i+1, Config.Value[i]);
		delay_ms(EEPROM_WRITE_DELAY);
	}
}

void save_eeprom(uint8_t adr, uint16_t data)
{
	if(adr>MAX_EEPROM_ADDRESS)  adr = MAX_EEPROM_ADDRESS;
	Write_eeprom(adr+1, data);
}



#endif











