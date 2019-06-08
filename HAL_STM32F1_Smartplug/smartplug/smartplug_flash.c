#include "stm32f1xx.h"
#include "smartplug_flash.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "comm_def.h"

FLASH_EraseInitTypeDef EraseInitStruct;

uint8_t flash_empty(uint32_t u32Addr)
{
      uint32_t StartAddr = u32Addr;
  
      while(*(uint32_t*)u32Addr != 0xffffffff)
      { 
            u32Addr = u32Addr + 4;
      }
      
      uint16_t u16Addr = (uint16_t)u32Addr;
        
      while(*(uint16_t*)u16Addr != 0xffff)
      {
            u16Addr = u16Addr + 2;
      }
      
      uint32_t EndAddr = (uint32_t)u16Addr;
      
      
      return (EndAddr - StartAddr)%11;
}

uint32_t flash_write(uint32_t addr, uint32_t *u32Data, uint16_t size)
{       
      uint16_t index = 0;
      uint16_t addrIndex = 0;
      uint16_t DataSize = 0;
      uint32_t PageError = 0;

      EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
      EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
      
      HAL_FLASH_Unlock();
      
      if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
      {
           DOUT("Eraser Error\r\n");
           while(1)
           {;}
      }
      
      DataSize = size/4;
      
      while(index < DataSize)
      {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, u32Data[index]) != HAL_OK)
            {               
                  DOUT("Write Error\r\n");
                  return 0;
            }
            HAL_Delay(1);
            addr = addr + 4;
            addrIndex = addrIndex + 4;
            index++;
      }
      
      DataSize = (size % 4) / 2;
      uint16_t* u16Data = (uint16_t*)(u32Data + index); 
      
      while(index < DataSize)
      {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, u16Data[index]) != HAL_OK)
            {               
                  DOUT("Write Error\r\n");
                  return 0;
            }
            HAL_Delay(1);
            addr = addr + addrIndex;
            addrIndex = addrIndex + 2;
            index++;
      }
      
      HAL_FLASH_Lock();
      
      return addr;
}

uint32_t flash_read(uint32_t addr, Flash_Parameter_t* Flash, uint16_t size)
{
      memcpy(Flash, (Flash_Parameter_t*)addr, size);
            
      return addr;
}
