#include "NMS_format.h"

struct CS_NAME
{
        uint16_t code;
        const char* name;
};

const char* nms_CS2_name(uint16_t cmd_stat)
{
        static char cs_name[5];

        static const struct CS_NAME cs_table[] = {
                { 0x1030, "DEV_REQ" },
                { 0x1031, "DEV_ACK" },
                { 0x1032, "DEV_RDY" },
                
                { 0x2030, "DEV_INFO_REQ" },
                { 0x2031, "DEV_INFO_ACK" },
                { 0x2032, "DEV_CONF_REQ" },
                { 0x2033, "DEV_CONF_ACK" },
                { 0x2034, "DEV_LIST_REQ" },
                { 0x2035, "DEV_LIST_ACK" },
                
                { 0x3010, "VANGO_REQ" },
                { 0x3011, "VANGO_ACK" },
                
                { 0x4010, "CTRL_REQ" },
                { 0x4011, "CTRL_ACK" },
                
                { 0x5010, "BOOT_RES" },
                
                { 0xA030, "ALIVE_REQ" },
                { 0xA031, "ALIVE_ACK" },
        };
        size_t i;
        
        for(i = 0; i < sizeof(cs_table) / sizeof(cs_table[0]); ++i)
        {
                if (cs_table[i].code == cmd_stat)
                {
                        return cs_table[i].name;
                }
                
        }
	sprintf(cs_name, "%04X", cmd_stat);
	
	return cs_name;
}