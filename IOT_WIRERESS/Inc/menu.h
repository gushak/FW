#ifndef _MENU_H_
#define _MENU_H_
#include "main.h"
#include "khh_eeprom.h"




#define SW1 HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin)
#define SW2 HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin)
#define SW3 HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin)

#define DEVICE_TYPE_16CH        1
#define DEVICE_TYPE_8CH         2
#define DEVICE_TYPE_PENDING     3
#define DEVICE_TYPE_CONTROL     4
#define DEVICE_TYPE_PENDING_8CH 5
#define DEVICE_TYPE_8CH_LIMIT   6
//s menu enum
typedef enum
{
    MENU_FARM_CODE = 0,    
    MENU_DEVICE_TYPE,
    MENU_RF_ADDR,
    MENU_RF_TYPE,
    MENU_OUT_COUNT1,

    MENU_OUT_COUNT2,
    MENU_OUT_COUNT3, 
    MENU_OUT_COUNT4, 
    MENU_OUT_COUNT5, 
    MENU_OUT_COUNT6, 

    MENU_OUT_COUNT7,
    MENU_OUT_COUNT8, 
    MENU_OUT_COUNT9, 
    MENU_OUT_COUNT10, 
    MENU_OUT_COUNT11, 

    MENU_OUT_COUNT12,
    MENU_OUT_COUNT13, 
    MENU_OUT_COUNT14, 
    MENU_OUT_COUNT15, 
    MENU_OUT_COUNT16, 

    MENU_NOT_USED1,  
   
    MENU_MAX = 21
}MENU_CONTROL_STATE;


extern volatile uint8_t Setting_enable; 
//extern uint8_t setting_mode(char* str0, char* str1);
extern void MeunTitleCopy(void);
extern void setting_mode(void);
extern void SW_check(void);
extern void menu_list();
void Device_list(uint8_t subMenu);
void Rf_list(uint8_t subMenu);
extern void DW_menu(void);

#endif