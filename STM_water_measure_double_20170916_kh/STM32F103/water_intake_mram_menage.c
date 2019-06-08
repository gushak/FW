#include "hw_control.h"
#include "spi_flash.h"

#define MRAM_MAX_ADDRESS	(0x8000 - 1)

#define MRAM_INTAKE_START_ADDRESS	0x100
#define MRAM_INTAKE_HEAD_ADDRESS		0x06

#define MRAM_EEPROM_ADDRESS	0x10

volatile uint16_t Mram_intake_address[MRAM_INTAKE_MAX_ADDRESS];
volatile uint8_t itemp = 0xfa, xtemp = 0;
volatile uint8_t status = 0;
volatile uint32_t wadr = 1;
volatile uint32_t radr = 2;
volatile uint8_t lp = 1;
volatile uint8_t dat = 1;
volatile uint8_t wd[10];
volatile uint8_t rd[10];

void mram_test(void)
{
    while(1)
    {		
        if(lp==1)
        {
            lp = 0;			
            status = SPI_FLASH_PageWrite((uint8_t*)&wd, wadr, 10);	Delay(1);				
            Delay(10);		          
        }
        SPI_FLASH_BufferRead((uint8_t*)&rd, radr, 10);
        Delay(10);
    }
}


uint16_t Intake_Head(void)
{
    //mram에서 값 읽어오기 
    uint16_t ret;
    //mram코드 작성예정 ~!!
    SPI_FLASH_BufferRead((uint8_t*)&ret, MRAM_INTAKE_HEAD_ADDRESS, 2);
      if(ret >= MRAM_INTAKE_MAX_ADDRESS) ret = 0;
    return ret;
}

void Intake_head_increment()
{
    uint16_t head = Intake_Head() + 1;
    
    if(head > MRAM_INTAKE_MAX_ADDRESS) head = 0;	
    SPI_FLASH_PageWrite((uint8_t*)&head, MRAM_INTAKE_HEAD_ADDRESS, 2);
}

void Intake_page_all_init()
{
    uint16_t i=0;
    WATER_ITAKE_TABLE_STRUCT w_intake_clear;
    
    memset((void *)&w_intake_clear, 0, sizeof(w_intake_clear));
    
    for(i=0;i<MRAM_INTAKE_MAX_ADDRESS;i++)
    {
        w_intake_clear.index = i;		
        SPI_FLASH_PageWrite((uint8_t *)&w_intake_clear,Mram_intake_address[i], sizeof(w_intake_clear));
    }
}

void Intake_page_all_enable()
{
    uint16_t i=0;
    WATER_ITAKE_TABLE_STRUCT w_intake_enable;
    
    for(i=0;i<MRAM_INTAKE_MAX_ADDRESS;i++)
    {
        memset((void *)&w_intake_enable, 0, sizeof(w_intake_enable));
        
        SPI_FLASH_BufferRead((uint8_t *)&w_intake_enable,Mram_intake_address[i], sizeof(w_intake_enable));
        
        if(w_intake_enable.index != 0 && w_intake_enable.date != 0 && w_intake_enable.feeder_num != 0)
        {
            w_intake_enable.status |= 0x01;		
            SPI_FLASH_PageWrite((uint8_t *)&w_intake_enable,Mram_intake_address[i], sizeof(w_intake_enable));
        }
    }
}

#ifdef MRAM_EEPROM_ENABLE

void lll_eeprom_init(void)
{
    uint16_t i=0;
    Mram_intake_address[0] = MRAM_INTAKE_START_ADDRESS;		//0x0004813C
    for(i=1;i<MRAM_INTAKE_MAX_ADDRESS;i++)
    {
        Mram_intake_address[i] = Mram_intake_address[i-1] + sizeof(WATER_ITAKE_TABLE_STRUCT);
    }	
    //mram_init
}

void eeprom_default_value(void)		
{
    CON_ADDRESS = CON_ADDRESS_DEFAULT;		
    CON_WATER_WAIT_TIME = CON_WATER_WAIT_TIME_DEFAULT;
    CON_WATER_WAIT_DIFF = CON_WATER_WAIT_DIFF_DEFAULT;
    CON_COWIN_IR_CNT = CON_COWIN_IR_CNT_DEFAULT;
    CON_COWIN_RFID_CNT = CON_COWIN_RFID_CNT_DEFAULT;
    COM_RE_ENTER_CNT = COM_RE_ENTER_CNT_DEFAULT;	
    CON_WATER_WAIT = CON_WATER_WAIT_DEFAULT;
    
    Intake_page_all_init();	//mram clear
}

void load_eeprom(void)
{
    SPI_FLASH_BufferRead((uint8_t*)&Config, MRAM_EEPROM_ADDRESS, sizeof(Config));
    
    if(Config.setup != EEPROM_CHECK_VALUE)
    {
      Config.setup = EEPROM_CHECK_VALUE;
      eeprom_default_value(); 
      save_all_eeprom(); 	
      
      TM_HD44780_Puts(0,0,"EEPROM DEFAULT  ");
      TM_HD44780_Puts(0,1," Water Weight   ");
      Delay(1000);		
	} 
	else
	{
      TM_HD44780_Puts(0,0,"EEPROM OK       ");
      TM_HD44780_Puts(0,1," Water Weight   ");
      Delay(1000);
	}
}

void save_all_eeprom(void)
{
    Config.setup = EEPROM_CHECK_VALUE;
    SPI_FLASH_PageWrite((uint8_t*)&Config, MRAM_EEPROM_ADDRESS, sizeof(Config));
}

#endif



