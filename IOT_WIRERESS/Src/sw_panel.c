/*

File : Relay_595.c
Create : 180404

Author: skkim

*/
#include "sw_panel.h"
#include "main.h"
#include "khh_lcd.h"
#include "menu.h"
#include "khh_eeprom.h"
//#include <stdlib.h> 
//#include <string.h>
/* Private */

//SW_PANEL_POSITION SwPanelPosition;
//SwPanelDef SwPanelData;
uint8_t datavack;

void SwPanelTask(key_stat *keydata, uint8_t *SwData, uint8_t task)
{
  uint8_t i;
  uint32_t c = 0;
  uint8_t position;
  uint32_t RelayBack = 0;
  switch(task)
  {
    case TIMER_1MS_TASK_IN_24V_READ:
        c = 1;
      //printf("SwData[%x][%x][%x] \r\n",SwData[0],SwData[1], SwData[2]);
      if(SwData[2] == 0 ||  SwData[1] == 0 || SwData[0] == 0)
        {
            return;             //s protact unused data parsing
        }

        keydata->key = (~((uint32_t)SwData[2] << 16 | (uint32_t)SwData[1] << 8 | (uint32_t)SwData[0])) &0x00FFFFFF;

        if((CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH)  || (CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT)){
          for(uint8_t k = 0; k< 24; k++){
            position = k % 3;
            if(position == 0){
              if(keydata->key & (1 << k)){
                RelayBack |= (0x01 << (k+2));
              }
            }
            else if(position == 2){
              if(keydata->key & (1 << k)){
                RelayBack |= (0x01 << (k-2));
              }            
            }
            else{
              if(keydata->key & (1 << k)){
                RelayBack |= (0x01 << (k));
              }            
            }
          }
          keydata->key  = RelayBack;
        }       
               
        for(i=0;i<24;i++)
        { 
            if(keydata->push[i])
            {
                //만약 눌렸던 상
               if((keydata->key & c) == 0)
               {
                    //눌렀다가 띤상태
                    keydata->cnt[i] ++;
                    keydata->push[i] = RESET;    
                    
                    keydata->change = SET;
               }
            }
            else if((keydata->key & c) != 0)
            {
               //눌린상태
               keydata->push[i] = SET;    
            }
            c = c<<1;
        }
        break;
        
    default:
      break;
  }
}

void SwPanelAction(key_stat *keydata, Out_struct *relaydata)
{

  uint8_t i;
  if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){  
    for (i = 0; i < 8; i++)
    {
        //키가 눌리면 해당 출력 동작하기 !! 
        if(keydata->cnt[0+(i*3)] != 0)
        {
            keydata->cnt[0+(i*3)] = 0;
            
            if(relaydata->pin[1+(i*2)] == SET){            //s 이미 역동작이 실행되고 있다면 종료 동작
              relaydata->pin[1+(i*2)] = RESET;        
              relaydata->pin[0+(i*2)] = RESET;  

              relaydata->time[0+(i*2)] = 0;        //s 스위치도 시간 추가함   
              relaydata->time[1+(i*2)] = 0;        //s 스위치도 시간 추가함   
              
              relaydata->pin[16+(i*2)] = RESET;
              relaydata->pin[17+(i*2)] = RESET;          
              relaydata->stat[i] = 0;              
            }
            else{
              relaydata->pin[0+(i*2)] = SET;
              relaydata->pin[1+(i*2)] = RESET;        

              relaydata->time[0+(i*2)] = Config.Value[0+(i*2)+4];        //s 스위치도 시간 추가함   
              relaydata->time[1+(i*2)] = 0;        //s 스위치도 시간 추가함   
              
              relaydata->pin[16+(i*2)] = SET;
              relaydata->pin[17+(i*2)] = RESET;              
              relaydata->stat[i] = 1;
            }
        }
        else if(keydata->cnt[1+(i*3)] != 0)
        {
            keydata->cnt[1+(i*3)] = 0;
            
            relaydata->pin[0+(i*2)] = RESET;
            relaydata->pin[1+(i*2)] = RESET;
            relaydata->time[0+(i*2)] = 0;        //s 스위치도 시간 추가함   
            relaydata->time[1+(i*2)] = 0;        //s 스위치도 시간 추가함   
            
            relaydata->pin[16+(i*2)] = RESET;
            relaydata->pin[17+(i*2)] = RESET;
            
            relaydata->stat[i] = 0;
        }
        else if(keydata->cnt[2+(i*3)] != 0)
        {
            keydata->cnt[2+(i*3)] = 0;
            
            if(relaydata->pin[0+(i*2)] == SET){            //s 이미 역동작이 실행되고 있다면 종료 동작
              relaydata->pin[1+(i*2)] = RESET;        
              relaydata->pin[0+(i*2)] = RESET;  
              relaydata->time[0+(i*2)] = 0;        //s 스위치도 시간 추가함   
              relaydata->time[1+(i*2)] = 0;        //s 스위치도 시간 추가함   

              
              relaydata->pin[16+(i*2)] = RESET;
              relaydata->pin[17+(i*2)] = RESET;              
              
              relaydata->stat[i] = 0;              
            }
            else{
              relaydata->pin[0+(i*2)] = RESET;
              relaydata->pin[1+(i*2)] = SET;
              relaydata->time[0+(i*2)] = 0;        //s 스위치도 시간 추가함   
              relaydata->time[1+(i*2)] = Config.Value[1+(i*2)+4];        //s 스위치도 시간 추가함   

              
              relaydata->pin[16+(i*2)] = RESET;
              relaydata->pin[17+(i*2)] = SET;
              
              relaydata->stat[i] = -1;
            }            
            

        }    
    } 
  }
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_16CH || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL){                 //s 16ch 및 소형보드용
      for (i = 0; i < 8; i++)
      {
        if(keydata->cnt[0+(i*3)] != 0)
        {
            keydata->cnt[0+(i*3)] = 0;

            if(relaydata->pin[i] == SET){
              relaydata->pin[i] = RESET;
              relaydata->stat16[i] = 0;              
            }
            else{
              relaydata->pin[i] = SET;  
              relaydata->stat16[i] = 1;                  
            }
        }
        else if(keydata->cnt[2+(i*3)] != 0)
        {
            keydata->cnt[2+(i*3)] = 0;
            
            if(relaydata->pin[i+8] == SET){
              relaydata->pin[i+8] = RESET;         
              relaydata->stat16[i+8] = 0;              
            }
            else{
              relaydata->pin[i+8] = SET;  
              relaydata->stat16[i+8] = 1;                  
            }
        }
      }
      
  }
      
}

uint16_t SwPanelLedControlReverse(uint16_t data)
{
    uint16_t reversevalue = 0;
        if((CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH) || (CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT)){  
          if(data & 0x0001) reversevalue |= (1<<14);    
          if(data & 0x0002) reversevalue |= (1<<15);
          if(data & 0x0004) reversevalue |= (1<<12);    
          if(data & 0x0008) reversevalue |= (1<<13);
          if(data & 0x0010) reversevalue |= (1<<10);    
          if(data & 0x0020) reversevalue |= (1<<11);
          if(data & 0x0040) reversevalue |= (1<<8);     
          if(data & 0x0080) reversevalue |= (1<<9);
          if(data & 0x0100) reversevalue |= (1<<6);     
          if(data & 0x0200) reversevalue |= (1<<7);
          if(data & 0x0400) reversevalue |= (1<<4);     
          if(data & 0x0800) reversevalue |= (1<<5);
          if(data & 0x1000) reversevalue |= (1<<2);     
          if(data & 0x2000) reversevalue |= (1<<3);
          if(data & 0x4000) reversevalue |= (1);     
          if(data & 0x8000) reversevalue |= (1<<1);           
        }
        else{
          if(data & 0x0001) reversevalue |= (1<<15);    
          if(data & 0x0002) reversevalue |= (1<<14);
          if(data & 0x0004) reversevalue |= (1<<13);    
          if(data & 0x0008) reversevalue |= (1<<12);
          if(data & 0x0010) reversevalue |= (1<<11);    
          if(data & 0x0020) reversevalue |= (1<<10);
          if(data & 0x0040) reversevalue |= (1<<9);     
          if(data & 0x0080) reversevalue |= (1<<8);
          if(data & 0x0100) reversevalue |= (1<<7);     
          if(data & 0x0200) reversevalue |= (1<<6);
          if(data & 0x0400) reversevalue |= (1<<5);     
          if(data & 0x0800) reversevalue |= (1<<4);
          if(data & 0x1000) reversevalue |= (1<<3);     
          if(data & 0x2000) reversevalue |= (1<<2);
          if(data & 0x4000) reversevalue |= (1<<1);     
          if(data & 0x8000) reversevalue |= (1);
        }
  
    return reversevalue;
}

uint16_t SwPanelLedControl16ChReverse(uint16_t data)
{
    uint16_t reversevalue = 0;
  
    if(data & 0x0001) reversevalue |= (1<<15);    
    if(data & 0x0002) reversevalue |= (1<<13);
    if(data & 0x0004) reversevalue |= (1<<11);    
    if(data & 0x0008) reversevalue |= (1<<9);
    if(data & 0x0010) reversevalue |= (1<<7);    
    if(data & 0x0020) reversevalue |= (1<<5);
    if(data & 0x0040) reversevalue |= (1<<3);     
    if(data & 0x0080) reversevalue |= (1<<1);
    if(data & 0x0100) reversevalue |= (1<<14);     
    if(data & 0x0200) reversevalue |= (1<<12);
    if(data & 0x0400) reversevalue |= (1<<10);     
    if(data & 0x0800) reversevalue |= (1<<8);
    if(data & 0x1000) reversevalue |= (1<<6);     
    if(data & 0x2000) reversevalue |= (1<<4);
    if(data & 0x4000) reversevalue |= (1<<2);     
    if(data & 0x8000) reversevalue |= (1);
  
    return reversevalue;
}



uint16_t SwPanelRelayControl(Out_struct *relaydata)
{

  uint16_t high = 0;
  
  if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
    if(relaydata->pin[0]) {high |= (1<<SW_PANDING_8CH_MOVEING1_ON);}      
    if(relaydata->pin[1]) {high |= (1<<SW_PANDING_8CH_MOVEING1_OFF);}           
    if(relaydata->pin[2]) {high |= (1<<SW_PANDING_8CH_MOVEING2_ON);}        
    if(relaydata->pin[3]) {high |= (1<<SW_PANDING_8CH_MOVEING2_OFF);}  
    if(relaydata->pin[4]) {high |= (1<<SW_PANDING_8CH_MOVEING3_ON);}        
    if(relaydata->pin[5]) {high |= (1<<SW_PANDING_8CH_MOVEING3_OFF);}  
//    if(relaydata->pin[6]) {high |= (0<<6);}        
    if(relaydata->pin[7]) {high |= (1<<SW_PANDING_8CH_PULL);}      
  }
  else{
    if(relaydata->pin[0]) {high |= (1<<0);}      
    if(relaydata->pin[1]) {high |= (1<<1);}           
    if(relaydata->pin[2]) {high |= (1<<2);}        
    if(relaydata->pin[3]) {high |= (1<<3);}  
    if(relaydata->pin[4]) {high |= (1<<4);}        
    if(relaydata->pin[5]) {high |= (1<<5);}  
    if(relaydata->pin[6]) {high |= (1<<6);}        
    if(relaydata->pin[7]) {high |= (1<<7);}  
    if(relaydata->pin[8]) {high |= (1<<8);}        
    if(relaydata->pin[9]) {high |= (1<<9);}  
    if(relaydata->pin[10]) {high |= (1<<10);}      
    if(relaydata->pin[11]) {high |= (1<<11);}  
    if(relaydata->pin[12]) {high |= (1<<12);}      
    if(relaydata->pin[13]) {high |= (1<<13);}  
    if(relaydata->pin[14]) {high |= (1<<14);}      
    if(relaydata->pin[15]) {high |= (1<<15);}  
  }

   return  high;
}

void SwPanelRfDataChange(Out_struct *relaydata, uint8_t rfdata, uint16_t time)
{
 datavack = rfdata;
      //키가 눌리면 해당 출력 동작하기 !! 
  if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH || CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
    if(time){           //s 시간이 있다면 on 수행
      if(rfdata % 2){
        if(relaydata->pin[rfdata-1] == SET){            //s 이미 역동작이 실행되고 있다면 종료 동작
          relaydata->pin[rfdata] = RESET;
          relaydata->pin[rfdata-1] = RESET;         
          relaydata->time[rfdata] = 0;
          relaydata->time[rfdata-1] = 0;                  
        }
        else{
            relaydata->pin[rfdata] = SET;
            relaydata->pin[rfdata-1] = RESET;
            if(time == 0xFFFF || time == Config.Value[rfdata+4]){
              relaydata->time[rfdata] = Config.Value[rfdata+4];            
            }
            else{
              relaydata->time[rfdata] = time;
              Config.Value[rfdata+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[rfdata-1] = 0;
        }
      }
      else{
        
        if(relaydata->pin[rfdata+1] == SET){            //s 이미 역동작이 실행되고 있다면 종료 동작
          relaydata->pin[rfdata] = RESET;
          relaydata->pin[rfdata+1] = RESET;         
          relaydata->time[rfdata] = 0;
          relaydata->time[rfdata+1] = 0;              
        }
        else{
            relaydata->pin[rfdata] = SET;
            relaydata->pin[rfdata+1] = RESET;
            if(time == 0xFFFF || time == Config.Value[rfdata+4]){
              relaydata->time[rfdata] = Config.Value[rfdata+4];            
            }
            else{
              relaydata->time[rfdata] = time;
              Config.Value[rfdata+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }        
            relaydata->time[rfdata+1] = 0;            
        }
      }
    }
    else{               //s 2ch 모두 off
       if(rfdata % 2){     
         relaydata->pin[rfdata] = RESET;
         relaydata->pin[rfdata-1] = RESET;         
         relaydata->time[rfdata] = 0;
         relaydata->time[rfdata-1] = 0;         
       }
       else{
         relaydata->pin[rfdata] = RESET;
         relaydata->pin[rfdata+1] = RESET;         
         relaydata->time[rfdata] = 0;
         relaydata->time[rfdata+1] = 0;                     
       }
    }
  }
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING)
  {
    if(time){           //s 시간이 있다면 on 수행
      if(rfdata == SW_PANDING_MOVEING1_ON){
            relaydata->pin[RELAY_PANDING_MOVEING1_ON] = SET;
            relaydata->pin[RELAY_PANDING_MOVEING1_OFF] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_MOVEING1_ON+4]){
              relaydata->time[RELAY_PANDING_MOVEING1_ON] = Config.Value[RELAY_PANDING_MOVEING1_ON+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_MOVEING1_ON] = time;
              Config.Value[RELAY_PANDING_MOVEING1_ON+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[RELAY_PANDING_MOVEING1_OFF] = 0;            
      }
      else if(rfdata == SW_PANDING_MOVEING1_OFF){
            relaydata->pin[RELAY_PANDING_MOVEING1_OFF] = SET;
            relaydata->pin[RELAY_PANDING_MOVEING1_ON] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_MOVEING1_OFF+4]){
              relaydata->time[RELAY_PANDING_MOVEING1_OFF] = Config.Value[RELAY_PANDING_MOVEING1_OFF+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_MOVEING1_OFF] = time;
              Config.Value[RELAY_PANDING_MOVEING1_OFF+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[RELAY_PANDING_MOVEING1_ON] = 0;
      }
      else if(rfdata == SW_PANDING_MOVEING2_ON){
            relaydata->pin[RELAY_PANDING_MOVEING2_ON] = SET;
            relaydata->pin[RELAY_PANDING_MOVEING2_OFF] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_MOVEING2_ON+4]){
              relaydata->time[RELAY_PANDING_MOVEING2_ON] = Config.Value[RELAY_PANDING_MOVEING2_ON+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_MOVEING2_ON] = time;
              Config.Value[RELAY_PANDING_MOVEING2_ON+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            else{
              relaydata->pin[RELAY_PANDING_MOVEING2_OFF] = RESET;
//              relaydata->pin[RELAY_PANDING_MOVEING2_ON] = RESET; 
              relaydata->time[RELAY_PANDING_MOVEING2_OFF] = 0;              
            }
            relaydata->time[RELAY_PANDING_MOVEING2_OFF] = 0;
      }
      else if(rfdata == SW_PANDING_MOVEING2_OFF){
            relaydata->pin[RELAY_PANDING_MOVEING2_OFF] = SET;
            relaydata->pin[RELAY_PANDING_MOVEING2_ON] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_MOVEING2_OFF+4]){
              relaydata->time[RELAY_PANDING_MOVEING2_OFF] = Config.Value[RELAY_PANDING_MOVEING2_OFF+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_MOVEING2_OFF] = time;
              Config.Value[RELAY_PANDING_MOVEING2_OFF+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            else       
            relaydata->time[RELAY_PANDING_MOVEING2_ON] = 0;
      }
      else if(rfdata == SW_PANDING_MOVEING3_ON){
            relaydata->pin[RELAY_PANDING_MOVEING3_ON] = SET;
            relaydata->pin[RELAY_PANDING_MOVEING3_OFF] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_MOVEING3_ON+4]){
              relaydata->time[RELAY_PANDING_MOVEING3_ON] = Config.Value[RELAY_PANDING_MOVEING3_ON+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_MOVEING3_ON] = time;
              Config.Value[RELAY_PANDING_MOVEING3_ON+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[RELAY_PANDING_MOVEING3_OFF] = 0;
      }
      else if(rfdata == SW_PANDING_MOVEING3_OFF){
            relaydata->pin[RELAY_PANDING_MOVEING3_OFF] = SET;
            relaydata->pin[RELAY_PANDING_MOVEING3_ON] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_MOVEING3_OFF+4]){
              relaydata->time[RELAY_PANDING_MOVEING3_OFF] = Config.Value[RELAY_PANDING_MOVEING3_OFF+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_MOVEING3_OFF] = time;
              Config.Value[RELAY_PANDING_MOVEING3_OFF+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }      
            relaydata->time[RELAY_PANDING_MOVEING3_ON] = 0;
      }      
      else if(rfdata == SW_PANDING_PULL){
            relaydata->pin[RELAY_PANDING_PULL] = SET;
            relaydata->pin[RELAY_PANDING_PULL2] = SET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_PULL+4]){
              relaydata->time[RELAY_PANDING_PULL] = Config.Value[RELAY_PANDING_PULL+4];  
              relaydata->time[RELAY_PANDING_PULL2] = Config.Value[RELAY_PANDING_PULL2+4];                
            }
            else if(time){
              relaydata->time[RELAY_PANDING_PULL] = time;
              Config.Value[RELAY_PANDING_PULL+4] = time;            
              relaydata->time[RELAY_PANDING_PULL2] = time;
              Config.Value[RELAY_PANDING_PULL2+4] = time;                       
	      save_all_eeprom();                    //s 변경값 저장             
            }
      }    
//      else if(rfdata == SW_PANDING_PANDING_PULL){
//            relaydata->pin[RELAY_PANDING_PANDING_PULL] = SET;
//            relaydata->pin[RELAY_PANDING_PANDING_ANNEAL] = RESET;
//            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_PANDING_PULL+4]){
//              relaydata->time[RELAY_PANDING_PANDING_PULL] = Config.Value[RELAY_PANDING_PANDING_PULL+4];            
//            }
//            else if(time){
//              relaydata->time[RELAY_PANDING_PANDING_PULL] = time;
//              Config.Value[RELAY_PANDING_PANDING_PULL+4] = time;               
//	      save_all_eeprom();                    //s 변경값 저장             
//            }
//            relaydata->time[RELAY_PANDING_PANDING_ANNEAL] = 0;
//      }
//      else if(rfdata == SW_PANDING_PANDING_ANNEAL){
//            relaydata->pin[RELAY_PANDING_PANDING_ANNEAL] = SET;
//            relaydata->pin[RELAY_PANDING_PANDING_PULL] = RESET;
//            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_PANDING_ANNEAL+4]){
//              relaydata->time[RELAY_PANDING_PANDING_ANNEAL] = Config.Value[RELAY_PANDING_PANDING_ANNEAL+4];            
//            }
//            else if(time){
//              relaydata->time[RELAY_PANDING_PANDING_ANNEAL] = time;
//              Config.Value[RELAY_PANDING_PANDING_ANNEAL+4] = time;               
//	      save_all_eeprom();                    //s 변경값 저장             
//            }      
//            relaydata->time[RELAY_PANDING_PANDING_PULL] = 0;
//      }        
    }
    else{
      if(rfdata == SW_PANDING_MOVEING1_ON || rfdata == SW_PANDING_MOVEING1_OFF){
            relaydata->pin[RELAY_PANDING_MOVEING1_ON] = RESET;
            relaydata->pin[RELAY_PANDING_MOVEING1_OFF] = RESET;
            relaydata->time[RELAY_PANDING_MOVEING1_ON] = 0;               
            relaydata->time[RELAY_PANDING_MOVEING1_OFF] = 0;            
      }
      else if(rfdata == SW_PANDING_MOVEING2_ON || rfdata == SW_PANDING_MOVEING2_OFF){
            relaydata->pin[RELAY_PANDING_MOVEING2_ON] = RESET;
            relaydata->pin[RELAY_PANDING_MOVEING2_OFF] = RESET;
            relaydata->time[RELAY_PANDING_MOVEING2_ON] = 0;               
            relaydata->time[RELAY_PANDING_MOVEING2_OFF] = 0;     
      }
      else if(rfdata == SW_PANDING_MOVEING3_ON || rfdata == SW_PANDING_MOVEING3_OFF){
            relaydata->pin[RELAY_PANDING_MOVEING3_ON] = RESET;
            relaydata->pin[RELAY_PANDING_MOVEING3_OFF] = RESET;
            relaydata->time[RELAY_PANDING_MOVEING3_ON] = 0;               
            relaydata->time[RELAY_PANDING_MOVEING3_OFF] = 0;     
      }
      else if(rfdata == SW_PANDING_PULL){
            relaydata->pin[RELAY_PANDING_PULL] = RESET;
            relaydata->time[RELAY_PANDING_PULL] = 0;   
            relaydata->pin[RELAY_PANDING_PULL2] = RESET;
            relaydata->time[RELAY_PANDING_PULL2] = 0;               
      }  
//      else if(rfdata == SW_PANDING_PANDING_PULL || rfdata == SW_PANDING_PANDING_ANNEAL){
//            relaydata->pin[RELAY_PANDING_PANDING_PULL] = RESET;
//            relaydata->pin[RELAY_PANDING_PANDING_ANNEAL] = RESET;
//            relaydata->time[RELAY_PANDING_PANDING_PULL] = 0;               
//            relaydata->time[RELAY_PANDING_PANDING_ANNEAL] = 0;     
//      }           
    }    
  }
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH)
  {
    if(time){           //s 시간이 있다면 on 수행
      if(rfdata == SW_PANDING_8CH_MOVEING1_ON){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING1_ON] = SET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING1_OFF] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_MOVEING1_ON+4]){
              relaydata->time[RELAY_PANDING_8CH_MOVEING1_ON] = Config.Value[RELAY_PANDING_8CH_MOVEING1_ON+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_MOVEING1_ON] = time;
              Config.Value[RELAY_PANDING_8CH_MOVEING1_ON+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[RELAY_PANDING_8CH_MOVEING1_OFF] = 0;            
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING1_OFF){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING1_OFF] = SET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING1_ON] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_MOVEING1_OFF+4]){
              relaydata->time[RELAY_PANDING_8CH_MOVEING1_OFF] = Config.Value[RELAY_PANDING_8CH_MOVEING1_OFF+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_MOVEING1_OFF] = time;
              Config.Value[RELAY_PANDING_8CH_MOVEING1_OFF+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[RELAY_PANDING_8CH_MOVEING1_ON] = 0;
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING2_ON){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING2_ON] = SET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING2_OFF] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_MOVEING2_ON+4]){
              relaydata->time[RELAY_PANDING_8CH_MOVEING2_ON] = Config.Value[RELAY_PANDING_8CH_MOVEING2_ON+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_MOVEING2_ON] = time;
              Config.Value[RELAY_PANDING_8CH_MOVEING2_ON+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            else{
              relaydata->pin[RELAY_PANDING_8CH_MOVEING2_OFF] = RESET;
//              relaydata->pin[RELAY_PANDING_8CH_MOVEING2_ON] = RESET; 
              relaydata->time[RELAY_PANDING_8CH_MOVEING2_OFF] = 0;              
            }
            relaydata->time[RELAY_PANDING_8CH_MOVEING2_OFF] = 0;
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING2_OFF){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING2_OFF] = SET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING2_ON] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_MOVEING2_OFF+4]){
              relaydata->time[RELAY_PANDING_8CH_MOVEING2_OFF] = Config.Value[RELAY_PANDING_8CH_MOVEING2_OFF+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_MOVEING2_OFF] = time;
              Config.Value[RELAY_PANDING_8CH_MOVEING2_OFF+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            else       
            relaydata->time[RELAY_PANDING_8CH_MOVEING2_ON] = 0;
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING3_ON){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING3_ON] = SET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING3_OFF] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_MOVEING3_ON+4]){
              relaydata->time[RELAY_PANDING_8CH_MOVEING3_ON] = Config.Value[RELAY_PANDING_8CH_MOVEING3_ON+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_MOVEING3_ON] = time;
              Config.Value[RELAY_PANDING_8CH_MOVEING3_ON+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }
            relaydata->time[RELAY_PANDING_8CH_MOVEING3_OFF] = 0;
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING3_OFF){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING3_OFF] = SET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING3_ON] = RESET;
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_MOVEING3_OFF+4]){
              relaydata->time[RELAY_PANDING_8CH_MOVEING3_OFF] = Config.Value[RELAY_PANDING_8CH_MOVEING3_OFF+4];            
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_MOVEING3_OFF] = time;
              Config.Value[RELAY_PANDING_8CH_MOVEING3_OFF+4] = time;               
	      save_all_eeprom();                    //s 변경값 저장             
            }      
            relaydata->time[RELAY_PANDING_8CH_MOVEING3_ON] = 0;
      }      
      else if(rfdata == SW_PANDING_8CH_PULL){
            relaydata->pin[RELAY_PANDING_8CH_PULL] = SET;
            relaydata->pin[RELAY_PANDING_8CH_PULL2] = SET;            
            if(time == 0xFFFF || time == Config.Value[RELAY_PANDING_8CH_PULL+4]){
              relaydata->time[RELAY_PANDING_8CH_PULL] = Config.Value[RELAY_PANDING_8CH_PULL+4];            
               relaydata->time[RELAY_PANDING_8CH_PULL2] = Config.Value[RELAY_PANDING_8CH_PULL2+4];                         
            }
            else if(time){
              relaydata->time[RELAY_PANDING_8CH_PULL] = time;
              Config.Value[RELAY_PANDING_PULL+4] = time;               
              relaydata->time[RELAY_PANDING_8CH_PULL2] = time;
              Config.Value[RELAY_PANDING_PULL2+4] = time;                   
	      save_all_eeprom();                    //s 변경값 저장             
            }
      }    
    }
    else{
      if(rfdata == SW_PANDING_8CH_MOVEING1_ON || rfdata == SW_PANDING_8CH_MOVEING1_OFF){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING1_ON] = RESET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING1_OFF] = RESET;
            relaydata->time[RELAY_PANDING_8CH_MOVEING1_ON] = 0;               
            relaydata->time[RELAY_PANDING_8CH_MOVEING1_OFF] = 0;            
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING2_ON || rfdata == SW_PANDING_8CH_MOVEING2_OFF){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING2_ON] = RESET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING2_OFF] = RESET;
            relaydata->time[RELAY_PANDING_8CH_MOVEING2_ON] = 0;               
            relaydata->time[RELAY_PANDING_8CH_MOVEING2_OFF] = 0;     
      }
      else if(rfdata == SW_PANDING_8CH_MOVEING3_ON || rfdata == SW_PANDING_8CH_MOVEING3_OFF){
            relaydata->pin[RELAY_PANDING_8CH_MOVEING3_ON] = RESET;
            relaydata->pin[RELAY_PANDING_8CH_MOVEING3_OFF] = RESET;
            relaydata->time[RELAY_PANDING_8CH_MOVEING3_ON] = 0;               
            relaydata->time[RELAY_PANDING_8CH_MOVEING3_OFF] = 0;     
      }
      else if(rfdata == SW_PANDING_8CH_PULL){
            relaydata->pin[RELAY_PANDING_8CH_PULL] = RESET;
            relaydata->time[RELAY_PANDING_8CH_PULL] = 0;   
            relaydata->pin[RELAY_PANDING_8CH_PULL2] = RESET;
            relaydata->time[RELAY_PANDING_8CH_PULL2] = 0;              
      }     
    }    
  }  
  else{                 //s 16ch
    if(time){           //s 시간이 있다면 on 수행      
      relaydata->pin[rfdata] = SET;
      if(time == 0xFFFF || time == Config.Value[rfdata+4]){
        relaydata->time[rfdata] = Config.Value[rfdata+4];            
      }
      else{
        relaydata->time[rfdata] = time;
        Config.Value[rfdata+4] = time;               
        save_all_eeprom();                    //s 변경값 저장             
      }
    }
    else{
        relaydata->pin[rfdata] = RESET;      
        relaydata->time[rfdata] = 0;      
    }
  }
  
}

