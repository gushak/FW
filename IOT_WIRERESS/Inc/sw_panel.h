#ifndef RELAY_595_H
#define RELAY_595_H

#include "main.h"
#include "stm32f1xx_hal.h"   
#include "stm32f1xx_hal_def.h"


#define SW_VALUE_OFF 0
#define SW_VALUE_POSITIVE 1
#define SW_VALUE_NEGATIVE 2


#define SW_CONTROL_FEEDER1			  0
//#define SW_CONTROL_MOVEING			1
#define SW_CONTROL_LIGHT1			2
#define SW_CONTROL_LIGHT2 			3
//#define SW_CONTROL_RESERVED			4
#define SW_CONTROL_HEATER1			5
#define SW_CONTROL_HEATER2 			6
#define SW_CONTROL_HEATER3 			7


#define SW_PANDING_MOVEING1_ON  0
#define SW_PANDING_MOVEING1_OFF 1
#define SW_PANDING_MOVEING2_ON  4
#define SW_PANDING_MOVEING2_OFF 5
#define SW_PANDING_MOVEING3_ON  8
#define SW_PANDING_MOVEING3_OFF 9
#define SW_PANDING_PULL         12
#define SW_PANDING_PANDING_PULL   14
#define SW_PANDING_PANDING_ANNEAL 15


#define RELAY_PANDING_MOVEING1_ON  0
#define RELAY_PANDING_MOVEING1_OFF 2
#define RELAY_PANDING_MOVEING2_ON  4
#define RELAY_PANDING_MOVEING2_OFF 6
#define RELAY_PANDING_MOVEING3_ON  8
#define RELAY_PANDING_MOVEING3_OFF 10
#define RELAY_PANDING_PULL         12
#define RELAY_PANDING_PULL2         14
//#define RELAY_PANDING_PANDING_PULL   14
//#define RELAY_PANDING_PANDING_ANNEAL 15


#define SW_PANDING_8CH_MOVEING1_ON  0
#define SW_PANDING_8CH_MOVEING1_OFF 1
#define SW_PANDING_8CH_MOVEING2_ON  4
#define SW_PANDING_8CH_MOVEING2_OFF 5
#define SW_PANDING_8CH_MOVEING3_ON  8
#define SW_PANDING_8CH_MOVEING3_OFF 9
#define SW_PANDING_8CH_PULL         12

#define RELAY_PANDING_8CH_MOVEING1_ON  0
#define RELAY_PANDING_8CH_MOVEING1_OFF 1
#define RELAY_PANDING_8CH_MOVEING2_ON  2
#define RELAY_PANDING_8CH_MOVEING2_OFF 3
#define RELAY_PANDING_8CH_MOVEING3_ON  4
#define RELAY_PANDING_8CH_MOVEING3_OFF 5
#define RELAY_PANDING_8CH_PULL         6
#define RELAY_PANDING_8CH_PULL2         7


//s SwPanel Position
typedef enum
{
  SW_PANEL_POSITIVE1 = 0,
  SW_PANEL_OFF1,
  SW_PANEL_NEGATIVE1,  

  SW_PANEL_POSITIVE2 = 3,
  SW_PANEL_OFF2,
  SW_PANEL_NEGATIVE2,    
  
  SW_PANEL_POSITIVE3 = 6,
  SW_PANEL_OFF3,
  SW_PANEL_NEGATIVE3,   

  SW_PANEL_POSITIVE4 = 9,
  SW_PANEL_OFF4,
  SW_PANEL_NEGATIVE4,   

  SW_PANEL_POSITIVE5 = 12,
  SW_PANEL_OFF5,
  SW_PANEL_NEGATIVE5,   

  SW_PANEL_POSITIVE6 = 15,
  SW_PANEL_OFF6,
  SW_PANEL_NEGATIVE6,   

  SW_PANEL_POSITIVE7 = 18,
  SW_PANEL_OFF7,
  SW_PANEL_NEGATIVE7,   

  SW_PANEL_POSITIVE8 = 21,
  SW_PANEL_OFF8,
  SW_PANEL_NEGATIVE8       
  
}SW_PANEL_POSITION;


//s SwPanel State
typedef struct
{
  uint8_t Sw1_State;
  uint8_t Sw2_State;
  uint8_t Sw3_State;
  uint8_t Sw4_State;    
  uint8_t Sw5_State;
  uint8_t Sw6_State;
  uint8_t Sw7_State;
  uint8_t Sw8_State;    
    
}SwPanelDef;


typedef struct
{
    uint8_t pin[32];
    int8_t stat[8];
    int8_t stat16[16];    
    uint16_t time[32];
    int8_t menual[16];     
}Out_struct;


typedef struct
{
    uint32_t old_key;
    uint32_t key;
    uint8_t change;
    
    uint8_t push[32];
    uint8_t cnt[32];
}key_stat;

void SwPanelTask(key_stat *keydata, uint8_t *SwData, uint8_t task);
void SwPanelAction(key_stat *keydata, Out_struct *relaydata);
//void SwPanelRelayControl(Out_struct *relaydata, uint8_t *spidata, uint8_t *rfdata);
uint16_t SwPanelRelayControl(Out_struct *relaydata);
void SwPanelRfDataChange(Out_struct *relaydata, uint8_t rfdata, uint16_t time);
uint16_t SwPanelLedControlReverse(uint16_t data);
uint16_t SwPanelLedControl16ChReverse(uint16_t data);
#endif

