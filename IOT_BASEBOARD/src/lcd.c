#include "lcd.h"
#include "main.h"

#define DATA_EEPROM_END_ADDR ((uint32_t)0x080097F0

PARSE_NX8040_t parse;
Parserf_Data rfdata;
char seg_buffer[30] = {0,};
int seg_index = 0;
uint16_t parse_index = 0;
uint8_t packet[128] = {0,};
uint8_t ch;
uint8_t size_valuedee;

void NxChangeBaud(uint32_t baud)
{
  char strtemp[50];
  uint8_t size;
  
  size = sprintf(strtemp, "baud=%d",baud);
  size_valuedee = size;

  strtemp[size] = 0xff;
  strtemp[size+1] = 0xff;
  strtemp[size+2] = 0xff;          
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp, 3+size, 10000);  
}

void parse_nx8040(uint8_t* buffer, uint16_t index)
{
  for(uint8_t i = 0; i < index; i++)
  {
    uint8_t ch = buffer[i];
    
    if(ch == 0x65 && parse_index == 0)
    {
        packet[parse_index++] = ch;
    }
    else if(ch == 0x65 && parse_index == 1)
    {
         packet[--parse_index] = ch;
    }
    else if(ch != 0x65 && parse_index == 1 && ch!= 0xFF)
    {
        parse.depth = ch;
        packet[parse_index++] = ch;
    }
    else if(ch != 0x65 && parse_index == 2 && ch!= 0xFF)
    {
        parse.ui = ch; 
        packet[parse_index++] = ch;
    }
    else if(ch == 0x00 && parse_index == 3)
    {
        packet[parse_index++] = ch;
    }
    else if(ch == 0xFF && parse_index >= 4)
    {
        packet[parse_index++] = ch;
        if(parse_index == 7)
        {
          command_data(parse.depth);
          memset(packet, 0x00, 128);
          memset(buffer, 0x00, cLCDRxdataCount);
          cLCDRxdataCount = 0;
          parse.ui = 0;
          parse.depth = 0;
          
        }
    }
    else
    {
          memset(packet, 0x00, 128);
          parse_index = 0;
    }
  }
}

void write_eeprom(uint32_t *Data, uint16_t size, uint32_t addr)
{
  HAL_FLASH_Unlock();

__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );

  FLASH_Erase_Sector(FLASH_SECTOR_7, VOLTAGE_RANGE_3);
  
  HAL_FLASH_Program(TYPEPROGRAM_WORD,addr, lcd_icon.current_page);
  HAL_FLASH_Program(TYPEPROGRAM_WORD,addr+4, lcd_icon.iconbuffer[0]);
  HAL_FLASH_Program(TYPEPROGRAM_WORD,addr+8, lcd_icon.iconbuffer[1]);
  HAL_FLASH_Program(TYPEPROGRAM_WORD,addr+12, lcd_icon.iconbuffer[2]);
  HAL_FLASH_Program(TYPEPROGRAM_WORD,addr+16, lcd_icon.iconbuffer[3]);
  HAL_FLASH_Program(TYPEPROGRAM_WORD,addr+20, lcd_icon.iconbuffer[4]);
  HAL_FLASH_Lock();
}

void read_eeprom(Lcdicon_st* lcd)
{
  lcd->current_page = *(uint32_t*)0x08060000; 
  lcd->iconbuffer[0]=*(uint32_t*)0x08060004;
  lcd->iconbuffer[1]=*(uint32_t*)0x08060008;
  lcd->iconbuffer[2]=*(uint32_t*)0x0806000C;
  lcd->iconbuffer[3]=*(uint32_t*)0x08060010;
  lcd->iconbuffer[4]=*(uint32_t*)0x08060014;
}
void command_data(uint8_t cmd)
{
  switch(cmd)
  {
    case 0x00:  
      if(parse.ui == 0x00)
      {
        lcd_icon.current_page = 1;
        NxLcdPage(1);
      }
      write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      break;
    case 1: 
      if(parse.ui == 0x08)
      {
        lcd_icon.current_page = 3;
        NxLcdPage(3);
      }
      else if(parse.ui == 0x04)
      {
        lcd_icon.current_page = 5;
        NxLcdPage(5);
      }
      else if(parse.ui == 0x05)
      {
        lcd_icon.current_page = 2;
        NxLcdPage(2);
      }
      else if(parse.ui == 0x07)
      {
        lcd_icon.current_page = 2;
        NxLcdPage(2);
      }
      else if(parse.ui == 0x09)
      {
        lcd_icon.current_page = 3;
        NxLcdPage(3);
      }
      write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      break;
    case 2:
      
      if(parse.ui == 14)
      {
        lcd_icon.current_page = 3;
        NxLcdPage(3);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 13)
      {
        lcd_icon.current_page = 1;
        NxLcdPage(1);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 15)
      {
        lcd_icon.current_page = 1;
        NxLcdPage(1);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else
        sidewindow(parse.ui);

      break;
    case 3:      
      if(parse.ui == 0x1A)
      {
        lcd_icon.current_page = 2;
        NxLcdPage(2);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 0x1B)
      {
        lcd_icon.current_page = 4;
        NxLcdPage(4);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 0x1C)
      {
        lcd_icon.current_page = 1;
        NxLcdPage(1);  
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 0x1D)
      {
        lcd_icon.current_page = 1;
        NxLcdPage(1);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else
      {
        ict_event(parse.ui);
      }
      break;
    case 4:      
      if(parse.ui == 0x1A)
      {
        lcd_icon.current_page = 3;
        NxLcdPage(3);
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 0x1C)
      {
        lcd_icon.current_page = 1;
        NxLcdPage(1); 
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else if(parse.ui == 0x1B)
      {
        lcd_icon.current_page = 5;
        NxLcdPage(5);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
      }
      else
      {
        ict_event2(parse.ui);
      }
      break;
 
      case 5:

      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[0] = data;
        cBuzzerCount = BUZZER_TIME_COUNT; 
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 4);
          seg_index = 4;
          NxLcdText("t2", seg_buffer);
          cBuzzerCount = BUZZER_TIME_COUNT;                         
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 4);
        seg_index = 4;
        NxLcdText("t2",seg_buffer);
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 4;
        NxLcdPage(4);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 6;
        NxLcdPage(6);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x10)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 2)
          swap(&seg_buffer[3], &seg_buffer[2]);
      else if(seg_index == 1)
      {
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }
      else if(seg_index == 0)
      {
          swap(&seg_buffer[1], &seg_buffer[0]);
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }

      if(lcd_icon.current_page == 5)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;
    
    case 6:
      
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[1] = data;
        cBuzzerCount = BUZZER_TIME_COUNT; 
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
        NxLcdText("t2",seg_buffer);
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 5;
        NxLcdPage(5);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 7;
        NxLcdPage(7);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x0A)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 0)
      {
          swap(&seg_buffer[0], &seg_buffer[1]);
      }

      if(lcd_icon.current_page == 6)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;
    
    case 7:
     
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[2] = data;
        cBuzzerCount = BUZZER_TIME_COUNT; 
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
       if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
      }
     
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 6;
        NxLcdPage(6);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 8;
        NxLcdPage(8);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x0A)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 0)
      {
          swap(&seg_buffer[0], &seg_buffer[1]);
      }

      if(lcd_icon.current_page == 7)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;
    
    case 8://hhlee

      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[3] = data;
        cBuzzerCount = BUZZER_TIME_COUNT; 
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 4);
          seg_index = 4;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 4);
        seg_index = 4;
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 7;
        NxLcdPage(7);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 9;
        NxLcdPage(9);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x10)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 2)
          swap(&seg_buffer[3], &seg_buffer[2]);
      else if(seg_index == 1)
      {
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }
      else if(seg_index == 0)
      {
          swap(&seg_buffer[1], &seg_buffer[0]);
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }

      if(lcd_icon.current_page == 8)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;
    
    case 9:
      
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[4] = data;
        cBuzzerCount = BUZZER_TIME_COUNT; 
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 4);
          seg_index = 4;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
     if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 4);
        seg_index = 4;
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 8;
        NxLcdPage(8);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 10;
        NxLcdPageString("10");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x10)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 2)
          swap(&seg_buffer[3], &seg_buffer[2]);
      else if(seg_index == 1)
      {
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }
      else if(seg_index == 0)
      {
          swap(&seg_buffer[1], &seg_buffer[0]);
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }

      if(lcd_icon.current_page == 9)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;
    
    case 10:
      
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[5] = data;
        cBuzzerCount = BUZZER_TIME_COUNT; 
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
     if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 9;
        NxLcdPage(9);      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 11;
        NxLcdPageString("11");       
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x10)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 2)
          swap(&seg_buffer[3], &seg_buffer[2]);
      else if(seg_index == 1)
      {
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }
      else if(seg_index == 0)
      {
          swap(&seg_buffer[1], &seg_buffer[0]);
          swap(&seg_buffer[2], &seg_buffer[1]);
          swap(&seg_buffer[3], &seg_buffer[2]);
      }

      if(lcd_icon.current_page == 10)
      {
          NxLcdText("t2",seg_buffer);
      }

      break;
    
      case 11:
      
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[6] = data;
        cBuzzerCount = BUZZER_TIME_COUNT;
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 10;
        NxLcdPageString("10");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 12;
        NxLcdPageString("12");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x0A)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 0)
      {
          swap(&seg_buffer[0], &seg_buffer[1]);
      }

      if(lcd_icon.current_page == 11)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;

    case 12:
      
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[7] = data;
        cBuzzerCount = BUZZER_TIME_COUNT;
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 11;
        NxLcdPageString("11");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {
        lcd_icon.current_page = 13;
        NxLcdPageString("13");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x0A)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 0)
      {
          swap(&seg_buffer[0], &seg_buffer[1]);
      }

      if(lcd_icon.current_page == 12)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;
    
    case 13:

      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[8] = data;
        cBuzzerCount = BUZZER_TIME_COUNT;
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
      }
     
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 12;
        NxLcdPageString("12");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {   
        lcd_icon.current_page = 14;
        NxLcdPageString("14");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x0A)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 0)
      {
          swap(&seg_buffer[0], &seg_buffer[1]);
      }

      if(lcd_icon.current_page == 13)
      {
          NxLcdText("t2",seg_buffer);
      }
      
      break;

      case 14:
      
      if(parse.ui == 0x0f)
      {
        uint16_t data = atoi(seg_buffer);
        Config.Value[9] = data;
        cBuzzerCount = BUZZER_TIME_COUNT;
        save_all_eeprom();
        if(seg_index != 0)
        {
          memset((void*)seg_buffer, '\0', 10);
          memset((void*)seg_buffer, '0', 2);
          seg_index = 2;
          NxLcdText("t2",seg_buffer);
        }
        break;
      }
      
      if(seg_index == 0)
      {
        memset((void*)seg_buffer, '\0', 10);
        memset((void*)seg_buffer, '0', 2);
        seg_index = 2;
      }
      
      if(parse.ui == 0x0d)
      {
        lcd_icon.current_page = 13;
        NxLcdPageString("13");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else if(parse.ui == 0x0e)
      {   
        lcd_icon.current_page = 15;
        NxLcdPageString("15");
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        seg_index = 0;
        break;
      }
      else
        seg_index--;
      
      if(parse.ui == 0x09)
      {
        seg_buffer[seg_index] = '1';
      }
      else if(parse.ui == 0x03)
      {
        seg_buffer[seg_index] = '2';
      }
      else if(parse.ui == 0x04)
      {
        seg_buffer[seg_index] = '3';
      }
      else if(parse.ui == 0x08)
      {
        seg_buffer[seg_index] = '4';
      }
      else if(parse.ui == 0x05)
      {
        seg_buffer[seg_index] = '5';
      }
      else if(parse.ui == 0x06)
      {
        seg_buffer[seg_index] = '6';
      }
      else if(parse.ui == 0x0c)
      {
        seg_buffer[seg_index] = '7';
      }
      else if(parse.ui == 0x0b)
      {
        seg_buffer[seg_index] = '8';
      }
      else if(parse.ui == 0x07)
      {
        seg_buffer[seg_index] = '9';
      }
      else if(parse.ui == 0x0A)
      {
        seg_buffer[seg_index] = '0';
      }
      else if(parse.ui == 0x11)
      {
        seg_index--;
      }
      else if(parse.ui == 0x12)
      {
        seg_index++;
      }
      
      if(seg_index == 0)
      {
          swap(&seg_buffer[0], &seg_buffer[1]);
      }

      if(lcd_icon.current_page == 14)
      {
          NxLcdText("t2",seg_buffer);
      }
      
    break;

    case 15:
      
      if(parse.ui == 1)
      {
        lcd_icon.current_page = 14;
        NxLcdPageString("14");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        break;
      }
      else if(parse.ui == 2)
      {
        lcd_icon.current_page = 16;
        NxLcdPageString("16");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        break;
      }
    break;

    case 16:
      
      if(parse.ui == 1)
      {
        lcd_icon.current_page = 15;
        NxLcdPageString("15");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        break;
      }
      else if(parse.ui == 2)
      {
        lcd_icon.current_page = 17;
        NxLcdPageString("17");      
        write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
        break;
      }
    break;
      
    }
}

void NxLcdPage(uint8_t pagenum)
{
  char strtemp[20];
  sprintf(strtemp,"page ");
  strtemp[5] = pagenum +0x30;
  strtemp[6] = 0xff;
  strtemp[7] = 0xff;
  strtemp[8] = 0xff;    
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp, 9, 10000);
  
  cBuzzerCount = BUZZER_TIME_COUNT;

  if((lcd_icon.current_page == 3) || (lcd_icon.current_page == 4))
      NxLcdShowCom(29,0);
  else if(lcd_icon.current_page == 2)
      NxLcdShowCom(17,0);
  
  if(lcd_icon.current_page != 1 && lcd_icon.current_page != 2)
    Set_Page16(lcd_icon.iconbuffer[lcd_icon.current_page]);
  else if(lcd_icon.current_page == 2)
  {
    char bufferout[16] = {0,};
    MakeBitText8CH(bufferout, lcd_icon.iconbuffer[lcd_icon.current_page], 16);
    Set_Page8(bufferout);
  }
}

void NxLcdPageString(char *String)
{
  char strtemp[20];
  uint8_t size = sprintf(strtemp,"page %s",String);
  strtemp[size] = 0xff;
  strtemp[size+1] = 0xff;
  strtemp[size+2] = 0xff;    
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp, size+3, 10000);
  
  cBuzzerCount = BUZZER_TIME_COUNT;
  
  if(lcd_icon.current_page == 3 && lcd_icon.current_page == 4)
      NxLcdShowCom(29,0);
  else if(lcd_icon.current_page == 2)
      NxLcdShowCom(17,0);
  
  if(lcd_icon.current_page != 1 && lcd_icon.current_page != 2)
    Set_Page16(lcd_icon.iconbuffer[lcd_icon.current_page]);
  else if(lcd_icon.current_page == 2)
  {
    char bufferout[16] = {0,};
    MakeBitText8CH(bufferout, lcd_icon.iconbuffer[lcd_icon.current_page], 16);
    Set_Page8(bufferout);
  }
}

void NxLcdLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,char* color)
{ 
  char strtemp1[20];
  uint8_t index;
  sprintf(strtemp1,"line %d,%d,%d,%d,%s",x1,y1,x2,y2,color);
  for(uint8_t i = 0; i < sizeof(strtemp1); i++)
  {
    if(strtemp1[i] == '\0')
    {
      index = i;
      break;
    }
  }
  strtemp1[index] = 0xff;
  strtemp1[index+1] = 0xff;
  strtemp1[index+2] = 0xff;    
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, index + 3, 10000);
}

void NxLcdClear(uint32_t val)
{
    char strtemp1[20];
    uint8_t index;
  
    sprintf(strtemp1,"cls ");
    sprintf(&strtemp1[4],"%d",val);
    for(uint8_t i = 0; i < sizeof(strtemp1); i++)
    {
      if(strtemp1[i] == '\0')
      {
        index = i;
        break;
      }
    }
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, index, 10000);
}

void NxLcdText(char* Text, char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"\"%s\".txt=\"%s\"",Text,String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText19(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t19.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText20(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t20.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText21(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t21.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText26(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t26.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText27(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t27.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText28(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t28.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText33(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t33.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText34(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t34.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText35(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t35.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText40(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t40.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText41(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t41.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText42(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t42.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText47(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t47.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText48(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t48.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText49(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t49.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText54(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t54.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText55(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t55.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText56(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t56.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText25(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t25.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText36(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t36.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}
void NxLcdText43(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t43.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
  
}void NxLcdText44(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t44.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
  
}void NxLcdText29(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t29.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);\

}

void NxLcdText30(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t30.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}
void NxLcdText37(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t37.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText38(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t38.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
  
}

void NxLcdText45(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t45.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText46(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t46.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText31(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t31.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText32(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t32.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdText39(char* String)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t39.txt=\"%s\"",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdSencond(uint8_t Sec)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"t10.txt=\"%d\"",Sec);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdString(char* String)
{ 
  char strtemp1[20];

  uint8_t size = sprintf(strtemp1,"xstr 10,10,100,30,1,BLACK,GREEN,1,1,1,%s",String);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}

void NxLcdColor(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"fill %d,%d,%d,%d,%d",x,y,w,h,color);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
}  

void NxLcdclick(uint8_t cid, uint8_t event)
{
  char strtemp1[20];
 
  uint8_t size = sprintf(strtemp1,"click %d,%d",cid,event);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
} 

void NxLcdShowCom(char id, uint8_t on_off)
{
  char strtemp1[20];
  
  uint8_t size = sprintf(strtemp1,"vis %d,%d",id,on_off);
  strtemp1[size] = 0xff;
  strtemp1[size +1] = 0xff;
  strtemp1[size +2] = 0xff;
  HAL_UART_Transmit(&huart7, (uint8_t*)strtemp1, size + 3, 10000);
} 

uint32_t local_tick;
uint32_t global_tick;
uint32_t lcd_tick;
uint8_t Occur_Event;
void sidewindow(uint8_t button)
{        
      local_tick = GTick();
      Occur_Event = 1;
      rfdata.Cmd = 2;
      rfdata.Id = 1;
      rfdata.Input = 2;
      rfdata.RelayTimer = 0xFFFF;
      
      if(button == 0x03)
      {
        rfdata.Relay = 0x00;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 0x05)
      {
        rfdata.Relay = 0x02;
      }
      else if(button == 0x06)
      {
        rfdata.Relay = 0x01;
      }
      else if(button == 21)
      {
        rfdata.Relay = 4;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 20)
      {
        rfdata.Relay = 4;
      }
      else if(button == 19)
      {
        rfdata.Relay = 3;
      }
      else if(button == 22)
      {
        rfdata.Relay = 5;
      }
      else if(button == 23)
      {
        rfdata.Relay = 6;
      }
      else if(button == 24)
      {
        rfdata.Relay = 6;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 25)
      {
        rfdata.Relay = 7;
      }
      else if(button == 26)
      {
         rfdata.Relay = 8;
      }
      else if(button == 27)
      {
        rfdata.Relay = 8;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 28)
      {
        rfdata.Relay = 9;
      }
      else if(button == 29)
      {
        rfdata.Relay = 10;
      }
      else if(button == 30)
      {
        rfdata.Relay = 10;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 31)
      {
        rfdata.Relay = 11;
      }
      else if(button == 32)
      {
        rfdata.Relay = 12;
      }
      else if(button == 33)
      {
        rfdata.Relay = 12;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 34)
      {
        rfdata.Relay = 13;
      }
      else if(button == 35)
      {
        rfdata.Relay = 14;
      }
      else if(button == 36)
      {
        rfdata.Relay = 14;
        rfdata.RelayTimer = 0x00;
      }
      else if(button == 37)
      {
        rfdata.Relay = 15;
      }
      else if(button == 38)
      {
        rfdata.Relay = 16;
      }
      else if(button == 39)
      {
        rfdata.Relay = 16;
        rfdata.RelayTimer = 0x00;
      }
      send_command(&rfdata);
}

void ict_event(uint8_t button)
{
      local_tick = GTick();
      Occur_Event = 1;
      rfdata.from = 1;
      rfdata.Cmd = 2;
      rfdata.Id = 2;
      rfdata.Input = 1;
      rfdata.RelayTimer = 0xffff;
      if(button == 0x03)
      {
        rfdata.Relay = 0x01;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x04)
      {
        rfdata.Relay = 0x01;
      }
      else if(button == 0x06)
      {
        rfdata.Relay = 0x02;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x07)
      {
        rfdata.Relay = 0x02;
      }
      else if(button == 0x09)
      {
        rfdata.Relay = 0x03;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x0A)
      {
        rfdata.Relay = 0x03;
      }
      else if(button == 0x0C)
      {
        rfdata.Relay = 0x04;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x0D)
      {
        rfdata.Relay = 0x04;
      }
      else if(button == 0x0F)
      {
        rfdata.Relay = 0x05;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x10)
      {
        rfdata.Relay = 0x05;
      }
      else if(button == 0x18)
      {
        rfdata.Relay = 0x06;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x17)
      {
        rfdata.Relay = 0x06;
      }
      else if(button == 0x15)
      {
        rfdata.Relay = 0x07;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x16)
      {
        rfdata.Relay = 0x07;
      }
      else if(button == 0x12)
      {
        rfdata.Relay = 0x08;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x11)
      {
        rfdata.Relay = 0x08;
      }
      send_command(&rfdata);
}
    
void ict_event2(uint8_t button)
{
      local_tick = GTick();
      Occur_Event = 1;
      rfdata.from = 1;
      rfdata.Cmd = 2;
      rfdata.Id = 2;
      rfdata.Input = 1;
      rfdata.RelayTimer = 0xffff;
      
      if(button == 0x03)
      {
        rfdata.Relay = 0x09;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x04)
      {
        rfdata.Relay = 0x09;
      }
      else if(button == 0x06)
      {
        rfdata.Relay = 0x0A;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x07)
      {
        rfdata.Relay = 0x0A;
      }
      else if(button == 0x09)
      {
        rfdata.Relay = 0x0B;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x0A)
      {
        rfdata.Relay = 0x0B;
      }
      else if(button == 0x0C)
      {
        rfdata.Relay = 0x0C;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x0D)
      {
        rfdata.Relay = 0x0C;
      }
      else if(button == 0x0F)
      {
        rfdata.Relay = 0x0D;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x10)
      {
        rfdata.Relay = 0x0D;
      }
      else if(button == 0x18)
      {
        rfdata.Relay = 0x0E;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x17)
      {
        rfdata.Relay = 0x0E;
      }
      else if(button == 0x15)
      {
        rfdata.Relay = 0x0F;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x16)
      {
        rfdata.Relay = 0x0F;
      }
      else if(button == 0x12)
      {
        rfdata.Relay = 0x10;
        rfdata.RelayTimer = 0;
      }
      else if(button == 0x11)
      {
        rfdata.Relay = 0x10;
      }
      send_command(&rfdata);
}

void send_command(Parserf_Data *data)
{
      unsigned int crc;
      uint8_t count = 0;
      uint8_t Rfdata[50];   
      Rfdata[count++] = 0xab;                 //s  0xab       
      Rfdata[count++] = AUTOMAN_SEND_OP_CODE;       
      Rfdata[count++] = AUTOMAN_DATA_SIZE;                     //s ID 1, ETX 1, CRC 2
      Rfdata[count++] = (uint8_t)CONFIG_FARM_PREFERENCE;       
      Rfdata[count++] = (uint8_t)(CONFIG_FARM_PREFERENCE >> 8);                     //s fcode 2byte  
      Rfdata[count++] = data->Id;

      Rfdata[count++] = data->Input;                  

      Rfdata[count++] = RF_MAIN_CMD_LCD;

      Rfdata[count++] = (uint8_t)(data->Relay);    
      Rfdata[count++] = (uint8_t)(data->Relay >> 8);       //s relay data 2byte 16bit값
      Rfdata[count++] = (uint8_t)data->RelayTimer;       
      Rfdata[count++] = (uint8_t)(data->RelayTimer>>8);                     //s  relay time 필요 없음

      Rfdata[count++] = 0;                
      Rfdata[count++] = 0;            
      Rfdata[count++] = 0;                
      Rfdata[count++] = 0;           
      Rfdata[count++] = 0;    
      Rfdata[count++] = 0;
      Rfdata[count++] = 0;
      
      crc = PacketCRC16(&Rfdata[3+AUTOMAN3025_RF_CONTROL], AUTOMAN_EX_SIZE);         //s fcode 부터 crc 계산

      Rfdata[count++] = (uint8_t)crc&0xFF;   
      Rfdata[count++] = (uint8_t)(crc>>8);  

      Rfdata[count++] = PACKET_ETX;        
     
      HAL_UART_Transmit(&huart6, Rfdata, count, 10000);  
      
      event_occur = 1;
}

void swap(char* data1, char* data2)
{
    uint8_t tmp = *data2;
    *data2 = *data1;
    *data1 = tmp;
}

