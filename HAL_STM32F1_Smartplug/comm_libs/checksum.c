#include <string.h>
#include "checksum.h"

uint8_t DoCheckSum(uint8_t* pucBuf, uint8_t ucLen)
{
      uint8_t sum = 0;
      for(int index = 0; index < ucLen; index++)
      {
              sum += pucBuf[index];
      }
      sum = ~sum;
      sum += 0x01;
      
      return sum;
}