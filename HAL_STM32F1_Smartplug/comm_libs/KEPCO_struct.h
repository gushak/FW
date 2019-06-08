#pragma once

/* KEPCO data struct
*/

#include "comm_def.h"

enum ERROR_3032_Type
{
        kERR_SmartplugBusy = 0x00,
        kERR_CRC_Error = 0x01
};

enum RELAY_RES_Type
{
       RES_FAIL = 0x00,
       RES_SUCCESS = 0x01
};

#pragma pack(push,1)

typedef struct KEPCO_3032_RES 
{
        uint8_t status;
}KEPCO_3032_RES_t;

typedef struct KEPCO_5034_RES
{
	uint8_t STATUS;
} KEPCO_5034_RES_t;

typedef struct KEPCO_5438_REQS
{       
        uint8_t SYSMODE;
        uint8_t ALIVE_P;
        uint8_t REG_P;
        uint8_t DEV_ID[11];
} KEPCO_5438_REQ_t;

typedef struct RELAY_RES
{
        uint8_t status;
}RELAY_RES_t;

#pragma pack(pop)