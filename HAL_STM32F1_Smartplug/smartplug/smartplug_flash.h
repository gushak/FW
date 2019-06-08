#pragma once

#include "smartplug_def.h"

#define FLASH_USER_START_ADDR           ((uint32_t)0x0803E000) /* Start @ of user Flash are */
#define FLASH_USER_END_ADDR             ((uint32_t)FLASH_USER_START_ADDR) + FLASH_PAGE_SIZE /* End @ of user Flash area */

typedef struct Flash_Parameter
{       
    
      uint8_t AlivePeriod;
      uint8_t RegiPeriod;
      uint8_t szDevid[11];
}Flash_Parameter_t;

uint8_t flash_empty(uint32_t addr);
uint32_t flash_write(uint32_t addr, uint32_t *data, uint16_t size);
uint32_t flash_read(uint32_t addr, Flash_Parameter_t* Flash, uint16_t size);
