#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "comm_str.h"

static const char hex_lut[] = "0123456789ABCDEF";

#define FROM_HEX(ch) (isdigit(ch) ? ch - '0' : tolower(ch) - 'a' +10)

size_t bin_from_hex_str(uint8_t bin[], size_t bin_size, const char *str)
{
        size_t len = strlen(str);
        size_t pos = 0;
        
        for(size_t i = 0; pos < bin_size && i + 1 < len;  pos++, i += 2)
        {
                uint8_t high = FROM_HEX(str[i]);
                uint8_t low = FROM_HEX(str[i +1 ]);
                
                if (high >= 16 || low >= 16)
                         break;
                bin[pos] = high << 4 | low;
        }
        
        return pos;
}

char* hex_str_from_bin(const uint8_t *bstr, size_t len, char result[], size_t result_size)
{
        size_t i, n;
        
        for(n = i = 0; i < len; ++i)
        {
               const uint8_t c = bstr[i];
               
               if (n + 1 >= result_size)
                        break;
               result[n++] = hex_lut[c >> 4];
               result[n++] = hex_lut[c & 15];
        }
        
        if ( n< result_size)
                result[n] = 0;
                
        return result;
}

void set_network_word(uint8_t* dest, uint16_t val)
{
        dest[0] = (uint8_t)(val >> 8);
        dest[1] = (uint8_t)(val);
}

void set_network_dword(uint8_t* dest, uint32_t val)
{
        dest[0] = (uint8_t)(val >> 24);
        dest[1] = (uint8_t)(val >> 16);
        dest[2] = (uint8_t)(val >> 8);
        dest[3] = (uint8_t)(val >> 0);
}

uint16_t get_network_word(const uint8_t* data)
{
        uint16_t val = ((uint16_t)data[0]<<8) | data[1];
        
        return val;
}

uint32_t get_network_dword(const uint8_t* data)
{
	uint32_t val = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | data[3];

	return val;
}

size_t nstrlen(const char *str, size_t max_len)
{
        size_t len = 0;
        
        while (len < max_len && str[len])
                len++;
                
        return len;
}

int is_plain_str(const char* str, size_t max_len)
{
	size_t i;
	size_t plen = nstrlen(str, max_len);

	for (i = 0; i < plen; ++i)
	{
		if (str[i] < 0x20 || str[i] > 0x7F)
		{
			return 0;
		}
	}

	return 1;
}