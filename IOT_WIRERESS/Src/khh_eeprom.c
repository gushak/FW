#include "main.h"
//#include "lwip/netif.h"
//#include "lwip/tcpip.h"
//#include "cmsis_os.h"
//#include "ethernetif.h"
//#include "app_ethernet.h"
//#include "httpserver-netconn.h"
//#include "tm_stm32_hd44780.h"
//#include "driver_GPIO.h"
#include "menu.h"
#include "khh_eeprom.h"
#include "eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EEPROM_DATA_START_POS 	0	// allows moving (not really necessary)
#define EEPROM_CHECK_VALUE		0x1119 //>checkValue


Config_Struct Config;
Config_Struct Config_Old;		// Holds configuration (from eeProm)
#define EEPROM_WRITE_DELAY	50

#define Read_eeprom(x, y)		EE_ReadVariable(x, y)
#define Write_eeprom(x, y)		EE_WriteVariable(x, y)

uint16_t VirtAddVarTab[NB_OF_VAR] = {0,1,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39};

void lll_eeprom_init(void)
{

	HAL_FLASH_Unlock();
	EE_Init();
}

//> setting mode 
/*
#define FARM_PREFERENCE				Config.Value[0]	//농장 설정 정보
#define FARM_MANAGE_OR_SHANCHION	Config.Value[1]	//0:농장관리/ 1:계류식
#define TRANSPORT_OR_AUTO_RUN		Config.Value[2]	//이송라인 동작/자동시작
#define FEED_DEV_TIME				Config.Value[3]	//급이장치 시간?
#define STANCHION_DEV_TIME			Config.Value[4]	//계류장치 시간?
*/

void eeprom_default_value(void)		//<<<<<<<<<<Config.value
{
	uint8_t i;
        memset((void *)&Config, 0, sizeof(Config));
        
	CONFIG_FARM_PREFERENCE = 3350;					//농장 설정 정보!!
        CONFIG_DEVIEC_TPYE = 1;
        CONFIG_RF_TYPE = 1;
        CONFIG_RF_ADDR = 1;

        for(i = 4; i < 20; i++)
        {
            Config.Value[i] = 3;
        }

        CONFIG_MOVING_COUNT = 2;
        CONFIG_8CH_REVERSE = 0;
        CONFIG_RESET_PROCESS = 0;

}

void eeprom_pend_default_value(void)		//<<<<<<<<<<Config.value
{
	uint8_t i;
        for(i = 4; i < 20; i++)
        {
          Config.Value[i] = 3;                //s 3초
        }
}


void eeprom_8ch_limit_default_value(void)		//<<<<<<<<<<Config.value
{
	uint8_t i;
        for(i = 4; i < 20; i++)
        {
            Config.Value[i] = 300;              //s 5분
        }
}

uint8_t load_eeprom(void)
{
	uint8_t ret = 0;
	
	int i = 0;
	Read_eeprom(0, (uint16_t *)&Config.setup);//> 0:in setup , ~1: value //> save하기전에 Read했을데 이미 넣은 고유값이 들어있는지 확인 > s
	if(Config.setup != EEPROM_CHECK_VALUE) //>임의의 수를 넣고 그값이 들어있는지 확인.
	{
		eeprom_default_value(); //>eeprom에 저장된 변수들의 함수
		Config.setup = EEPROM_CHECK_VALUE;
		save_all_eeprom(); 	
		ret= 0;
	} 
        else {
		for(i=0;i<MAX_EEPROM_ADDRESS; i++)
		{
			Read_eeprom(i+1, (uint16_t *)&Config.Value[i]);//> 0:in setup , ~1: value
		}
		
		ret = 1;
	}
	
	return ret;
}

void save_all_eeprom(void)
{
	int i = 0;
	Write_eeprom(0, EEPROM_CHECK_VALUE);	//Config.setup
	//delay_ms(EEPROM_WRITE_DELAY);
	
	for(i=0;i<MAX_EEPROM_ADDRESS;i++)
	{
		Config_Old.Value[i] = Config.Value[i];	//flush 영역이므로 데이터값이 잘못 될 경우를 대비하여 백업
		Write_eeprom(i+1, Config.Value[i]);
		//delay_ms(EEPROM_WRITE_DELAY);
	}
}

void save_eeprom(uint8_t adr, uint16_t data)
{
	if(adr>MAX_EEPROM_ADDRESS)  adr = MAX_EEPROM_ADDRESS;
	Write_eeprom(adr+1, data);
}
