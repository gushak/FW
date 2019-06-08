//#include "hw_control.h"


#include "main.h"
#include "debug.h"
#include "rf_modules.h"
#include <stdlib.h> 
#include <string.h>
#include "usart.h"
#include "eeprom_4seri.h"
#include "lcd_hd44780.h"
#include "lcd.h"

strAutomanPacket rx_packet_Rf_ch1;
//uint8_t  buffRfRxData[LORA_BUFF_SIZE];
uint8_t  cUartRxDataCount = 0;
Lcdicon_st lcd_icon;


uint8_t _delay10;

uint16_t AutoLoopCount = 0;
uint16_t AutoCheckCount = 0;
uint8_t bDeviceCheck = 0;
uint8_t bReAliveCheck = 0;
uint8_t cReAliveCount = 0;
uint8_t RfRxCheckCount = 0;

uint16_t PacketCRC16(unsigned char * data, uint16_t size)
{
	uint16_t i,n;
	uint16_t crc = 0xffff;

	for(i = 0; i < size; i++)
	{
                //printf("data[%d][%02x]\r\n", i, data[i]);
		crc ^= data[i];
		for(n = 0; n < 8; n++)
		crc = (crc & 0x0001)?((crc >> 1)^0xa001):(crc >> 1);
	}
	return crc;
}

volatile uint8_t ttt1=0,ttt2=0;


uint8_t PorcessPacket_Rf_check(uint8_t rx_data, uint8_t* buffdata, uint8_t mode)
{
	unsigned int crc = 0;
        uint8_t result = 0;

        
        if(cRfProcessMode){     //s data 모드
        
          //printf("rx[%02x]", rx_data);
          switch(rx_packet_Rf_ch1.status)
          {
                  case RXSTATUS_IDLE:
                    if(rx_data == AUTOMAN_DATA_SIZE){
                        rx_packet_Rf_ch1.status = RXSTATUS_FARMCODE1;                   
                        rx_packet_Rf_ch1.size =  AUTOMAN_EX_SIZE;                   
                    }
                    else{
                        PorcessPacket_Rf_Clear();                         
                    }
                    break;
                  case RXSTATUS_FARMCODE1:
                      rx_packet_Rf_ch1.fcode = rx_data & 0x00FF;
                      rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                           
                      cUartRxDataCount++;                      
                      rx_packet_Rf_ch1.status = RXSTATUS_FARMCODE2;
                      break;
                  case RXSTATUS_FARMCODE2:
                      rx_packet_Rf_ch1.fcode |= (uint16_t)rx_data << 8;
                      rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                           
                      if(rx_packet_Rf_ch1.fcode == CONFIG_FARM_PREFERENCE_1){          //s 농장코드가 같다면 다음 진행
                        cUartRxDataCount++;        
                        rx_packet_Rf_ch1.status = RXSTATUS_ID;
                      }
                      else{
                        PorcessPacket_Rf_Clear();   
                        printf("Fcode differ %d\r\n", rx_packet_Rf_ch1.fcode);                          
                      }
                      break;
                  case RXSTATUS_ID:
                          rx_packet_Rf_ch1.status = RXSTATUS_MODE;
                          rx_packet_Rf_ch1.idx = rx_data;
                          rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                               
                          cUartRxDataCount++;                     
                    break;     
                  case RXSTATUS_MODE:
                          rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                          rx_packet_Rf_ch1.mode = rx_data;
                          rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                               
                          cUartRxDataCount++;                
                  break;
                  case RXSTATUS_CMD:
//                    if(rx_data == RF_SUB_CMD_ALIVE || rx_data == RF_SUB_CMD_REQ  || rx_data == RF_SUB_CMD_CHANGE || rx_data == RF_SUB_CMD_CONTROL || 
//                       rx_data == RF_SUB_CMD_INPUT ||rx_data == RF_SUB_PENDING_AUTO_START || rx_data == RF_SUB_PENDING_AUTO_STOP){
                        rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                        rx_packet_Rf_ch1.cmd = rx_data;	
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;     
                        cUartRxDataCount++;                          
                        //printf("RXSTATUS_ID\r\n"); 
//                    }
//                    else{
//                        PorcessPacket_Rf_Clear();   
//                        //printf("Commend error %d\r\n", rx_data);                        
//                    }
                    break;    
                          
                  case RXSTATUS_DATA:
                    if(rx_packet_Rf_ch1.size > cUartRxDataCount){           //s 9byte data 수신
                      rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;
                      cUartRxDataCount++;
                      //printf("RXSTATUS_DATA: %d\r\n", cUartRxDataCount);                          
                      break;
                    }
                    else{
                          rx_packet_Rf_ch1.status = RXSTATUS_CRC1;                //s break없이 case RXSTATUS_CRC1: 수행
                    }
                    
                    case RXSTATUS_CRC1:
                            rx_packet_Rf_ch1.status = RXSTATUS_CRC2;
                            rx_packet_Rf_ch1.crc_l = rx_data;
                            cUartRxDataCount++;                            
                            break;
                            
                    case RXSTATUS_CRC2:			
                            rx_packet_Rf_ch1.status = RXSTATUS_ETX;                          
                            rx_packet_Rf_ch1.crc_h= rx_data;
                            cUartRxDataCount++;    
                            break;
                                          
                    case RXSTATUS_ETX:	
                      if(rx_data == PACKET_ETX)
                      {                   
                          uint16_t crcresult;
                          crc = (rx_packet_Rf_ch1.crc_h<<8) | rx_packet_Rf_ch1.crc_l;   
                          crcresult  = PacketCRC16(rx_packet_Rf_ch1.data, rx_packet_Rf_ch1.size);
                          //TRACE_DEBUG("crc[%x] :  crcresult[%x]\r\n", crc, crcresult);                     
                            cUartRxDataCount++;                              
                          //CRC가 정상이면
                          if(crc == crcresult)						
                          {				
                              if(cUartRxDataCount == AUTOMAN_DATA_SIZE){        //s 길이가 같은지 확인
                                                                      
                                  if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_REQ){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);                                               
                                      
                                    result  = RF_SUB_CMD_REQ;                        
                                  }
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_CONTROL){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                    result = RF_SUB_CMD_CONTROL;
                                  }                                  
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_ALIVE){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);                                    
                                     result = RF_SUB_CMD_ALIVE;                                   
                                  } 
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_INPUT){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);                                               
                                     result = RF_SUB_CMD_INPUT;                                   
                                  }                                   
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_PENDING_AUTO_START){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);                                               
                                    result = RF_SUB_PENDING_AUTO_START;                                   
                                  } 
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_PENDING_AUTO_STOP){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);                                               
                                     result = RF_SUB_PENDING_AUTO_STOP;                                   
                                  }                                     
                                  else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_ALIVE){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);                                               
                                      result = RF_MAIN_CMD_ALIVE;
                                  }
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_FAN_CONTROL){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                    result = RF_SUB_CMD_FAN_CONTROL;
                                  }
                                  
                                  if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_LCD){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                   
                                   result = RF_SUB_CMD_LCD;
                                  }
                                  
                                  if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_CHANGE){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                    result = RF_SUB_CMD_CHANGE;
                                  }                                  

                                  //TRACE_DEBUG("result[%x] [%d]\r\n", result, rx_packet_Rf_ch1.cmd);                                            
                                  PorcessPacket_Rf_Clear();                                    
                                  return result;
                              }
                              else{                                    
                                      PorcessPacket_Rf_Clear();   
                                      return result;
                              }
                          } 
                          else {

                                PorcessPacket_Rf_Clear();                             
                          }
                        }
                        else{
                            PorcessPacket_Rf_Clear();                                  
                        }
                    break;		
                  default :
                            PorcessPacket_Rf_Clear();     
                      break;
            
          }	
          return result;      
        }
        else{           //s 셋팅모드
          
          if(cRfSettingState == RF_SETTING_FREQ_CHECK_WAIT || cRfSettingState == RF_SETTING_FREQ_CHECK2_WAIT){
            if(mode){
              
              switch(rx_packet_Rf_ch1.status){
                    case RXSTATUS_IDLE:
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                           
                        cUartRxDataCount++;                      
                        rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                        break;

                    case RXSTATUS_DATA:
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;         
                        
                        if((rx_packet_Rf_ch1.data[0] == CONFIG_RF_FREQ_ADDR1_9) && (rx_packet_Rf_ch1.data[1] == CONFIG_RF_FREQ_ADDR2_10)){                //s rf 주파수가 맞다면
                            cRfProcessMode = 1;
                            TRACE_DEBUG("RF_SETTING_FREQ_SET END\r\n");   
                        }
                        else{             //s 틀리다면
                          cRfSettingState = RF_SETTING_FREQ_SET;
                          cRfFreqRetryCount++;
                          if(cRfFreqRetryCount > 2){
                            sprintf(buffGLcd1Line, "AUTOMAN SET ERR2");		
                            TM_HD44780_Puts_State(0,0, buffGLcd1Line);                                
                            TRACE_DEBUG("RF_SETTING_FREQ_SET cRfFreqRetryCount [%d]\r\n",cRfFreqRetryCount);    
                            cRfProcessMode = 1;
                          }                          
                        }
                        TRACE_DEBUG("rx_packet_Rf_ch1.data[0] [%d]  rx_packet_Rf_ch1.data[1]  [%d]\r\n",rx_packet_Rf_ch1.data[0], rx_packet_Rf_ch1.data[1]);                                        
                        PorcessPacket_Rf_Clear();     
                        break;     
                    default :
                         PorcessPacket_Rf_Clear();     
                        break;                        
                  
              }
            }
          else{
              switch(rx_packet_Rf_ch1.status){
                    case RXSTATUS_IDLE:
                      if(rx_data == 0xAB){                    
                        rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                      }
                      break;                      
                          
                    case RXSTATUS_CMD:
                        if(rx_data == 0x0A){                    
                          rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                        }         
                        else{
                         PorcessPacket_Rf_Clear();                               
                        }
                        break;                               
                    case RXSTATUS_DATA:    
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                           
                        cUartRxDataCount++;       
                        if(cUartRxDataCount >= 3){
                          rx_packet_Rf_ch1.status = RXSTATUS_ETX;
                        }
                        break; 
                    case RXSTATUS_ETX:
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                                  
                        if((rx_packet_Rf_ch1.data[0] == CONFIG_RF_FREQ_ADDR1_9) && (rx_packet_Rf_ch1.data[1] == CONFIG_RF_FREQ_ADDR2_10)){                //s rf 주파수가 맞다면
                            cRfProcessMode = 1;
                            TRACE_DEBUG("RF_SETTING_FREQ_SET END\r\n");                           
                        }
                        else{             //s 틀리다면
                          cRfSettingState = RF_SETTING_FREQ_SET;
                          cRfFreqRetryCount++;
                          if(cRfFreqRetryCount > 2){
                            sprintf(buffGLcd1Line, "AUTOMAN SET ERR2");		
                            TM_HD44780_Puts_State(0,0, buffGLcd1Line);                                
                            TRACE_DEBUG("RF_SETTING_FREQ_SET cRfFreqRetryCount [%d]\r\n",cRfFreqRetryCount);    
                            cRfProcessMode = 1;
                          }
                        }
//                        TRACE_DEBUG("rx_packet_Rf_ch1.data[0] [%d]  rx_packet_Rf_ch1.data[1]  [%d]\r\n",rx_packet_Rf_ch1.data[0], rx_packet_Rf_ch1.data[1]);                                    
                        PorcessPacket_Rf_Clear();             
                    break;                        
                        
                    default :
                         PorcessPacket_Rf_Clear();     
                        break;                        
                  
              }
            }            
            
            return 0;
          }
        }
        return 0;        
}


uint8_t PorcessPacket_Rf_check_Behap(uint8_t rx_data, uint8_t* buffdata, uint8_t mode)
{
	unsigned int crc = 0;
        uint8_t result = 0;
        uint8_t datatemp = 0;
        
        if(cRfProcessMode){     //s data 모드        
          //printf("rx[%02x]", rx_data);
          switch(rx_packet_Rf_ch1.status)
          {
                  case RXSTATUS_IDLE:
                    if(rx_data == AUTOMAN_BAHAP_DATA_SIZE){
                        rx_packet_Rf_ch1.status = RXSTATUS_ID;                   
                        rx_packet_Rf_ch1.size =  AUTOMAN_BAHAP_EX_SIZE;      
                        cUartRxDataCount++;                        
                    }
                    else{
                        PorcessPacket_Rf_Clear();                         
                    }
                    break;
                  case RXSTATUS_ID:
                          rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                          rx_packet_Rf_ch1.idx = rx_data;        
                        rx_packet_Rf_ch1.data[cUartRxDataCount - 1] = rx_data;                               
                          cUartRxDataCount++;                               
                    break;     
                  case RXSTATUS_CMD:
//                    if(rx_data == RF_SUB_CMD_ALIVE || rx_data == RF_SUB_CMD_REQ  || rx_data == RF_SUB_CMD_CHANGE || rx_data == RF_SUB_CMD_CONTROL || 
//                       rx_data == RF_SUB_CMD_INPUT ||rx_data == RF_SUB_PENDING_AUTO_START || rx_data == RF_SUB_PENDING_AUTO_STOP){
                        rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                        rx_packet_Rf_ch1.cmd = rx_data;	
                        rx_packet_Rf_ch1.data[cUartRxDataCount - 1] = rx_data + 1;                             
                        cUartRxDataCount++;                             
                        //printf("RXSTATUS_ID\r\n"); 
//                    }
//                    else{
//                        PorcessPacket_Rf_Clear();   
//                        //printf("Commend error %d\r\n", rx_data);                        
//                    }
                    break;    
                          
                  case RXSTATUS_DATA:
                    if(rx_packet_Rf_ch1.size + AUTOMAN_BAHAP_INFO_SIZE > cUartRxDataCount){           //s 9byte data 수신
                      datatemp = cUartRxDataCount - 1;
                      rx_packet_Rf_ch1.data[datatemp] = rx_data;
                      cUartRxDataCount++;
                      //printf("RXSTATUS_DATA: %d %d\r\n", cUartRxDataCount, rx_data);                          
                      break;
                    }
                    else{
                          rx_packet_Rf_ch1.status = RXSTATUS_CRC1;                //s break없이 case RXSTATUS_CRC1: 수행
                    }
                    
                    case RXSTATUS_CRC1:
                            rx_packet_Rf_ch1.status = RXSTATUS_CRC2;
                            rx_packet_Rf_ch1.crc_l = rx_data;
                            cUartRxDataCount++;                            
                            break;
                            
                    case RXSTATUS_CRC2:			
                            rx_packet_Rf_ch1.status = RXSTATUS_ETX;                          
                            rx_packet_Rf_ch1.crc_h= rx_data;
                            cUartRxDataCount++;    
                            break;
                                          
                    case RXSTATUS_ETX:	
                      if(rx_data == PACKET_ETX)
                      {                   
                          uint16_t crcresult;
                          crc = (rx_packet_Rf_ch1.crc_h<<8) | rx_packet_Rf_ch1.crc_l;   
                          crcresult  = PacketCRC16(&rx_packet_Rf_ch1.data[2], rx_packet_Rf_ch1.size);
                          //TRACE_DEBUG("crc[%x] :  crcresult[%x]\r\n", crc, crcresult);                     
                           // cUartRxDataCount++;                              
                          //CRC가 정상이면
                          if(crc == crcresult)						
                          {				
                            
                          //TRACE_DEBUG("cUartRxDataCount[%d] :  AUTOMAN_BAHAP_DATA_SIZE[%d]\r\n", cUartRxDataCount, AUTOMAN_BAHAP_DATA_SIZE);                                    
                              if(cUartRxDataCount == AUTOMAN_BAHAP_DATA_SIZE){        //s 길이가 같은지 확인
                                                                    
                                  if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_BAHAP_REQ){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_BAHAP_EX_SIZE + 2);                                               
                                      result  = RF_SUB_CMD_BAHAP_REQ_DATA;                        
                                  }
                                  else if(rx_packet_Rf_ch1.cmd == RF_SUB_CMD_BAHAP_CHANGE){
                                    memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_BAHAP_EX_SIZE + 2);
                                    result = RF_SUB_CMD_BAHAP_CHANGE_DATA;
                                  }
                                  //TRACE_DEBUG("result[%x] [%d]\r\n", result, rx_packet_Rf_ch1.cmd);                                            
                                  PorcessPacket_Rf_Clear();                                    
                                  return result;
                              }
                              else{                                    
                                      PorcessPacket_Rf_Clear();   
                                      return result;
                              }
                          } 
                          else {

                                PorcessPacket_Rf_Clear();                             
                          }
                        }
                        else{
                            PorcessPacket_Rf_Clear();                                  
                        }
                    break;		
                  default :
                            PorcessPacket_Rf_Clear();     
                      break;
            
          }	
          return result;          
        }
        else{           //s 셋팅모드
          
          if(cRfSettingState == RF_SETTING_FREQ_CHECK_WAIT || cRfSettingState == RF_SETTING_FREQ_CHECK2_WAIT){
            if(mode){
              
              switch(rx_packet_Rf_ch1.status){
                    case RXSTATUS_IDLE:
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                           
                        cUartRxDataCount++;                      
                        rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                        break;

                    case RXSTATUS_DATA:
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;         
                        
                        if((rx_packet_Rf_ch1.data[0] == CONFIG_RF_FREQ_ADDR1_9) && (rx_packet_Rf_ch1.data[1] == CONFIG_RF_FREQ_ADDR2_10)){                //s rf 주파수가 맞다면
                            cRfProcessMode = 1;
                            TRACE_DEBUG("RF_SETTING_FREQ_SET END\r\n");   
                        }
                        else{             //s 틀리다면
                          cRfSettingState = RF_SETTING_FREQ_SET;
                          cRfFreqRetryCount++;
                          if(cRfFreqRetryCount > 2){
                            sprintf(buffGLcd1Line, "AUTOMAN SET ERR2");		
                            TM_HD44780_Puts_State(0,0, buffGLcd1Line);                                
                            TRACE_DEBUG("RF_SETTING_FREQ_SET cRfFreqRetryCount [%d]\r\n",cRfFreqRetryCount);    
                            cRfProcessMode = 1;
                          }                          
                        }
                        TRACE_DEBUG("rx_packet_Rf_ch1.data[0] [%d]  rx_packet_Rf_ch1.data[1]  [%d]\r\n",rx_packet_Rf_ch1.data[0], rx_packet_Rf_ch1.data[1]);                                        
                        PorcessPacket_Rf_Clear();     
                        break;     
                    default :
                         PorcessPacket_Rf_Clear();     
                        break;                        
                  
              }
            }
          else{
              switch(rx_packet_Rf_ch1.status){
                    case RXSTATUS_IDLE:
                      if(rx_data == 0xAB){                    
                        rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                      }
                      break;                      
                          
                    case RXSTATUS_CMD:
                        if(rx_data == 0x0A){                    
                          rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                        }         
                        else{
                         PorcessPacket_Rf_Clear();                               
                        }
                        break;                               
                    case RXSTATUS_DATA:    
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                           
                        cUartRxDataCount++;       
                        if(cUartRxDataCount >= 3){
                          rx_packet_Rf_ch1.status = RXSTATUS_ETX;
                        }
                        break; 
                    case RXSTATUS_ETX:
                        rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                                  
                        if((rx_packet_Rf_ch1.data[0] == CONFIG_RF_FREQ_ADDR1_9) && (rx_packet_Rf_ch1.data[1] == CONFIG_RF_FREQ_ADDR2_10)){                //s rf 주파수가 맞다면
                            cRfProcessMode = 1;
                            TRACE_DEBUG("RF_SETTING_FREQ_SET END\r\n");                           
                        }
                        else{             //s 틀리다면
                          cRfSettingState = RF_SETTING_FREQ_SET;
                          cRfFreqRetryCount++;
                          if(cRfFreqRetryCount > 2){
                            sprintf(buffGLcd1Line, "AUTOMAN SET ERR2");		
                            TM_HD44780_Puts_State(0,0, buffGLcd1Line);                                
                            TRACE_DEBUG("RF_SETTING_FREQ_SET cRfFreqRetryCount [%d]\r\n",cRfFreqRetryCount);    
                            cRfProcessMode = 1;
                          }
                        }
//                        TRACE_DEBUG("rx_packet_Rf_ch1.data[0] [%d]  rx_packet_Rf_ch1.data[1]  [%d]\r\n",rx_packet_Rf_ch1.data[0], rx_packet_Rf_ch1.data[1]);                                    
                        PorcessPacket_Rf_Clear();             
                    break;                        
                        
                    default :
                         PorcessPacket_Rf_Clear();     
                        break;                        
                  
              }
            }            
            
            return 0;
          }
        }
        return 0;  

}



void PorcessPacket_Rf_Clear(void)
{
    rx_packet_Rf_ch1.status = RXSTATUS_IDLE;  
    cUartRxDataCount = 0;           
    memset(buffUartRxData, 0 , sizeof(buffUartRxData));
    memset(&rx_packet_Rf_ch1, 0, sizeof(rx_packet_Rf_ch1));
}

uint16_t data16temp;
uint8_t pendingeventback;
void RfAutomanParse(uint8_t mode)
{
//    char c = 0;
//    static char AutomanData[30];
//    static uint8_t data_cnt = 0;
//    static uint16_t clear_cnt = 0;
//    static bool data_ok = false;
    //uint8_t i;
    //uint16_t value;
	
    uint8_t data8temp;
    uint8_t intime1back = 0;
    uint8_t intime2back = 0;
    uint8_t intime3back = 0;    

    uint8_t pendingevent = 0; 

  
    cRfRXBuffClearCount++;
  
    if(cRxdataCount)
    {
       for(uint8_t i = 0; i < cRxdataCount; i++)
       {
          cRxComplteState = PorcessPacket_Rf_check(buffUartRxDataSave[i], AutomanData, mode);
       }
       cRxdataCount = 0;
       memset(buffUartRxDataSave, 0, sizeof(buffUartRxDataSave));
       cRfRXBuffClearCount = 0;
    }
    if(cRfRXBuffClearCount > SERIAL_RF_RX_CLEAR_COUNT)
    {
      PorcessPacket_Rf_Clear();
      cRfRXBuffClearCount = 0;
      cRxdataCount = 0;
      memset(buffUartRxDataSave, 0, sizeof(buffUartRxDataSave));         
    }  
  
    if(cRxComplteState){
      
      cRxComplteState = 0;    
      RfRxEvent = AutomanData[RF_DATAPOS_ID] - 1; 
      strRfRxData[RfRxEvent].Id = RfRxEvent + 1;
      if(AutomanData[RF_DATAPOS_MODE] == SUB_TYPE_16CH){
              strRfRxData[RfRxEvent].Mode = MQTT_TYPE_16CH;
      }
      else if(AutomanData[RF_DATAPOS_MODE] == SUB_TYPE_8CH){
              strRfRxData[RfRxEvent].Mode = MQTT_TYPE_8CH;											
      }
      else if(AutomanData[RF_DATAPOS_MODE] == SUB_TYPE_PENDING){
              strRfRxData[RfRxEvent].Mode = MQTT_TYPE_PENDING;											
      }
      else if(AutomanData[RF_DATAPOS_MODE] == SUB_TYPE_SMALL_8CH){
              strRfRxData[RfRxEvent].Mode = MQTT_TYPE_SMALL_8CH;											
      }								
      else{		//s test    
       strRfRxData[RfRxEvent].Mode = MQTT_TYPE_8CH;
       TRACE_DEBUG("strRfRxData[RfRxEvent].Mode Not Define [%d]\n\r",RfRxEvent);              
      }
      strRfRxData[RfRxEvent].Cmd = AutomanData[RF_DATAPOS_CMD];
      
      if(strRfRxData[RfRxEvent].Cmd == RF_SUB_CMD_ALIVE || strRfRxData[RfRxEvent].Cmd == RF_MAIN_CMD_ALIVE) 
      {			//s 모드를 인식하기 위한 목적
            //상태 요청
            strRfRxData[RfRxEvent].Alive = SET;
            if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
                wErrorAutomanCheckCont[RfRxEvent]--;
                if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                        wErrorAutomanCheckCont[RfRxEvent]--;
                }
            }									
        } 
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_REQ) 
        {
          data16temp = AutomanData[RF_DATAPOS_RELAYOUTL] | (AutomanData[RF_DATAPOS_RELAYOUTH]<<8);
          if(strRfRxData[RfRxEvent].Relay != data16temp)
          {						
            sprintf(buffGLcd2Line, "IN EVENT:%d", strRfRxData[RfRxEvent].Id);									
            TM_HD44780_Puts_State(0,1, buffGLcd2Line); 	
            
            if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_16CH)
            {
                  strRfRxData[RfRxEvent].Relay = data16temp;		//s 최근데이터 업데이트
                  lcd_icon.iconbuffer[lcd_icon.current_page] = data16temp;
                  DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, RfRxEvent+1, 0);                  
                  Set_Page16(lcd_icon.iconbuffer[lcd_icon.current_page]);
            }
            else if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_8CH)
            {
                  strRfRxData[RfRxEvent].Relay = data16temp;		//s 최근데이터 업데이트         
                  lcd_icon.iconbuffer[lcd_icon.current_page] = data16temp;
                  DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, RfRxEvent+1, 0); 
                  char bufferout[16] = {0,};
                  MakeBitText8CH(bufferout, lcd_icon.iconbuffer[lcd_icon.current_page], 16);
                  Set_Page8(bufferout);
            }
            else if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_PENDING)
            {
                    pendingevent = 1;
                    strRfRxData[RfRxEvent].Relay = data16temp;			//s 최근데이터 업데이트													
            }
            else if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_SMALL_8CH)
            {  	
              
              strRfRxData[RfRxEvent].Relay = data16temp;			//s 최근데이터 업데이트              
              DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, RfRxEvent+1, 0);		
            }   
         }
             
          if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_PENDING){
            data8temp = AutomanData[RF_DATAPOS_INPUT];
            strRfRxData[RfRxEvent].InTimer1 = AutomanData[RF_DATAPOS_INPUTTIME1L] | (AutomanData[RF_DATAPOS_INPUTTIME1H]<<8);
            strRfRxData[RfRxEvent].InTimer2 = AutomanData[RF_DATAPOS_INPUTTIME2L] | (AutomanData[RF_DATAPOS_INPUTTIME2H]<<8);
            strRfRxData[RfRxEvent].InTimer3 = AutomanData[RF_DATAPOS_INPUTTIME3L] | (AutomanData[RF_DATAPOS_INPUTTIME3H]<<8);	

            if((strRfRxData[RfRxEvent].Input & 0x2A) != (data8temp & 0x2A)){		//s 이벤트발생이면 변경
              pendingevent |= 0x02;        
              if((strRfRxData[RfRxEvent].Input & 0x02) && ((data8temp & 0x02) == 0)){
                intime1back = 1;
              }
              if((strRfRxData[RfRxEvent].Input & 0x08) && ((data8temp & 0x08) == 0)){
                intime2back = 1;
              }
              if((strRfRxData[RfRxEvent].Input & 0x20) && ((data8temp & 0x20) == 0)){
                intime3back = 1;
              }              
            }
            
            if(strRfRxData[RfRxEvent].Input & 0x2A)
            {
                    pendingevent |= 0x04;							// 이송라인 input 1이면 계속 업데이트
            }
            
            strRfRxData[RfRxEvent].Input = data8temp;
            
          if(strRfRxData[RfRxEvent].MqttFreqSend == SET)		//s auto 상태이면 정보 계속 보냄
                    pendingevent |= 0x08;
          }
          pendingeventback = pendingevent;
          if(pendingevent){
                DawoonMqttSend(MQTT_IOT_HCOW_STAT, RfRxEvent + 1, 0);            
          }
          
          if(pendingevent & 0x02)
          {
            if(mqtt_First_connect == SET){
                    
              uint8_t savepos = strSen_value.SavePos + 1;
              if(savepos == MAIN_HTTP_MAX_BUFF)		//s 최대치 넘어가면 초기화
                      savepos = 0;
              
              if(strSen_value.NowPos == savepos){		//s http 전송 큐 저장
                sprintf(buffGLcd2Line, "HTTP BUFF MAX %d", CONFIG_RF_TYPE_3);									
                TM_HD44780_Puts_State(0,1, buffGLcd2Line);
                TRACE_DEBUG(" HTTP BUFF MAX %d\r\n", CONFIG_RF_TYPE_3);                
                HAL_Delay(3000);	
                                
                NVIC_SystemReset();		//s http 정상이 아니므로 리셋											
              }
              else{
                strSen_value.HttpData[strSen_value.SavePos].cType = HTTP_TYPE_AUTOMAN;
                strSen_value.HttpData[strSen_value.SavePos].cPos = RfRxEvent + 1;

                strSen_value.HttpData[strSen_value.SavePos].InTimer1Event = intime1back;                
                strSen_value.HttpData[strSen_value.SavePos].InTimer2Event = intime2back;
                strSen_value.HttpData[strSen_value.SavePos].InTimer3Event = intime3back;                
                
                strSen_value.HttpData[strSen_value.SavePos].InTimer1 = strRfRxData[RfRxEvent].InTimer1;
                strSen_value.HttpData[strSen_value.SavePos].InTimer2 = strRfRxData[RfRxEvent].InTimer2;
                strSen_value.HttpData[strSen_value.SavePos].InTimer3 = strRfRxData[RfRxEvent].InTimer3;
        
                //s display part
                if(strSen_value.HttpData[strSen_value.SavePos].cPos < 4){
                  TM_HD44780_Puts_State(12,1, "PAND");                  
                }

                strSen_value.SavePos = savepos;									
                cHttpSendCount++;												
                }
              }
          }
            if(wErrorAutomanCheckCont[RfRxEvent])
            {	//s ask를 받으면 에러 카운트를 감소시킴
                wErrorAutomanCheckCont[RfRxEvent]--;
                if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                        wErrorAutomanCheckCont[RfRxEvent]--;
                }
            }	          
										
        }			
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_CHANGE)   
        {
           strRfRxData[RfRxEvent].Relay = AutomanData[RF_DATAPOS_RELAYOUTL] | (AutomanData[RF_DATAPOS_RELAYOUTH]<<8);               
          //publish_message(MyTopic, client, Json_ret_change(&strRfRxData[RfRxEvent], RF_SUB_CMD_CHANGE));		
          DawoonMqttSend(MQTT_IOT_CHANGE_RESPONE, RfRxEvent, RF_SUB_CMD_CHANGE);
          cBuzzerCount = BUZZER_TIME_COUNT; 
          sprintf(buffGLcd2Line, "%d:SUB CHAN %04x ", RfRxEvent+1, strRfRxData[RfRxEvent].Relay);									
          TM_HD44780_Puts_State(0,1, buffGLcd2Line);
          if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
              wErrorAutomanCheckCont[RfRxEvent]--;
              if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                      wErrorAutomanCheckCont[RfRxEvent]--;
              }
          }	          
          strRfRxData[RfRxEvent].RelayBack   = strRfRxData[RfRxEvent].Relay;
              
          lcd_icon.iconbuffer[lcd_icon.current_page] =  strRfRxData[RfRxEvent].Relay;
            if(lcd_icon.current_page != 1 && lcd_icon.current_page != 2)
              Set_Page16(lcd_icon.iconbuffer[lcd_icon.current_page]);
            else if(lcd_icon.current_page == 2)
            {
              char bufferout[16] = {0,};
              MakeBitText8CH(bufferout, lcd_icon.iconbuffer[lcd_icon.current_page], 16);
              Set_Page8(bufferout);
            }
        }
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_LCD)
        {
          char bufferout[16] = {0,};
            data16temp = AutomanData[RF_DATAPOS_RELAYOUTL] | (AutomanData[RF_DATAPOS_RELAYOUTH]<<8);
            
            if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_8CH)
            {
                MakeBitText8CH(bufferout, data16temp, 16);
                Set_Page8(bufferout);
            }
            else if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_16CH)
            {
                for(uint8_t i = 0; i < 3; i++)
                  Set_Page16(data16temp);
            }
            
            lcd_icon.iconbuffer[lcd_icon.current_page] = data16temp;
            DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, RfRxEvent+1, 0);
            cBuzzerCount = BUZZER_TIME_COUNT; 
            write_eeprom((uint32_t*)&lcd_icon, sizeof(lcd_icon), 0x08060000);
            event_occur = 0;
            second = 0;
            NxLcdSencond(second);
        }
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_CONTROL) {
          strRfRxData[RfRxEvent].Relay = AutomanData[RF_DATAPOS_RELAYOUTL] | (AutomanData[RF_DATAPOS_RELAYOUTH]<<8);	
          if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
                  wErrorAutomanCheckCont[RfRxEvent]--;
                  if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                          wErrorAutomanCheckCont[RfRxEvent]--;
                  }
          }
          
          if(strTimeControl[RfRxEvent].cFeedSendFlag){			//s 수신 확인 후 활성화 중지
                  if(strRfRxData[RfRxEvent].Relay & 0x0001){		//s 피더가 동작했으면
                          strTimeControl[RfRxEvent].cFeedTimeFlag[strTimeControl[RfRxEvent].cFeedPos] = RESET;		//s 활성화 중지
                          strTimeControl[RfRxEvent].cFeedSendFlag = RESET;	
                  }
          }
          
          if(strRfRxData[RfRxEvent].RelayBack != strRfRxData[RfRxEvent].Relay){			//s 계사 릴레이 변환 앱에 알림
                  strRfRxData[RfRxEvent].RelayBack = strRfRxData[RfRxEvent].Relay;
                  DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, RfRxEvent+1, 0);
                  //publish_message(MyTopic, client, Json_device_status(RfRxEvent+1, MQTT_STATUS_RESPONE));													
          }	          
          
//          sprintf(buffGLcd2Line, "%d:SUB CONT %04x ", RfRxEvent+1, strRfRxData[RfRxEvent].Relay);									
//          TM_HD44780_Puts_State(0,1, buffGLcd2Line);    														
          TRACE_DEBUG("RF_SUB_CMD_CONTROL: %d, %04x\r\n", RfRxEvent, strRfRxData[RfRxEvent].Relay);						
        }
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_FAN_CONTROL) {
          strRfRxData[RfRxEvent].Relay = AutomanData[RF_DATAPOS_RELAYOUTL] | (AutomanData[RF_DATAPOS_RELAYOUTH]<<8);	
          if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
                  wErrorAutomanCheckCont[RfRxEvent]--;
                  if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                          wErrorAutomanCheckCont[RfRxEvent]--;
                  }
          }

          if(strRfRxData[RfRxEvent].RelayBack != strRfRxData[RfRxEvent].Relay)
{			//s 16CH 릴레이 변환 앱에 알림
                  strRfRxData[RfRxEvent].RelayBack = strRfRxData[RfRxEvent].Relay;
                  DawoonMqttSend(MQTT_IOT_STATUS_RESPONE, RfRxEvent+1, 0);
                  //publish_message(MyTopic, client, Json_device_status(RfRxEvent+1, MQTT_STATUS_RESPONE));													
          }	          
          
//          sprintf(buffGLcd2Line, "%d:SUB CONT %04x ", RfRxEvent+1, strRfRxData[RfRxEvent].Relay);									
//          TM_HD44780_Puts_State(0,1, buffGLcd2Line);    														
          TRACE_DEBUG("RF_SUB_CMD_FAN_CONTROL: %d, %04x\r\n", RfRxEvent, strRfRxData[RfRxEvent].Relay);						
        }	      
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_PENDING_AUTO_START) {
                //publish_message(MyTopic, client, Json_ret_change(&strRfRxData[RfRxEvent], RF_SUB_PENDING_AUTO_START));	
          strRfRxData[RfRxEvent].MqttFreqSend = SET;
          DawoonMqttSend(MQTT_IOT_CHANGE_RESPONE, RfRxEvent, RF_SUB_PENDING_AUTO_START);          
          DawoonMqttSend(MQTT_IOT_HCOW_STAT, RfRxEvent + 1, 0);        	
          cBuzzerCount = BUZZER_TIME_COUNT;           
            if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
                wErrorAutomanCheckCont[RfRxEvent]--;
                if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                        wErrorAutomanCheckCont[RfRxEvent]--;
                }
            }	          
        } 
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_PENDING_AUTO_STOP) {
          //publish_message(MyTopic, client, Json_ret_change(&strRfRxData[RfRxEvent], RF_SUB_PENDING_AUTO_STOP));		
          strRfRxData[RfRxEvent].MqttFreqSend = RESET;
          DawoonMqttSend(MQTT_IOT_CHANGE_RESPONE, RfRxEvent, RF_SUB_PENDING_AUTO_STOP);          
          DawoonMqttSend(MQTT_IOT_HCOW_STAT, RfRxEvent + 1, 0);        
          cBuzzerCount = BUZZER_TIME_COUNT;           
            if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
                wErrorAutomanCheckCont[RfRxEvent]--;
                if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                        wErrorAutomanCheckCont[RfRxEvent]--;
                }
            }	          
        } 						
        else {
            TRACE_DEBUG("What !!!!!!!!!!!!!!!!!![%d] \r\n", strRfRxData[RfRxEvent].Cmd);
        }

      }
   

}


uint16_t RfAutomanBahapRelayMake(uint8_t* data)
{
  uint16_t redata = 0;
  
  redata  = data[0];
  redata  |= data[1] << 2;
  redata  |= data[2] << 4;
  redata  |= data[3] << 6;
  redata  |= data[4] << 8;
  redata  |= data[5] << 10;
  redata  |= data[6] << 12;
  redata  |= data[7] << 14;
  
  return redata;
}



void RfAutomanBahapParse(uint8_t mode)
{
	
    //uint8_t data8temp;
    uint16_t data16temp;

  
    cRfRXBuffClearCount++;
  
    if(cRxdataCount)
    {
       for(uint8_t i = 0; i < cRxdataCount; i++)
       {
          cRxComplteState = PorcessPacket_Rf_check_Behap(buffUartRxDataSave[i], AutomanData, mode);
       }
       cRxdataCount = 0;
       memset(buffUartRxDataSave, 0, sizeof(buffUartRxDataSave));
       cRfRXBuffClearCount = 0;
    }
    if(cRfRXBuffClearCount > SERIAL_RF_RX_CLEAR_COUNT)
    {
      PorcessPacket_Rf_Clear();
      cRfRXBuffClearCount = 0;
      cRxdataCount = 0;
      memset(buffUartRxDataSave, 0, sizeof(buffUartRxDataSave));      
    }  
  
    if(cRxComplteState){
      cRxComplteState = 0;    
      RfRxEvent = AutomanData[RF_BAHAP_DATAPOS_ID] - 1;
      strRfRxData[RfRxEvent].Id = RfRxEvent + 1;
      strRfRxData[RfRxEvent].Cmd = AutomanData[RF_BAHAP_DATAPOS_CMD];
      

//      cBuzzerCount = BUZZER_TIME_COUNT; 
//      TRACE_DEBUG("strRfRxData[RfRxEvent].Cmd %d  id:[%d]\n\r", strRfRxData[RfRxEvent].Cmd, RfRxEvent);
      
        if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_BAHAP_REQ_DATA) {
          
          strRfRxData[RfRxEvent].Alive = SET;          
          data16temp = RfAutomanBahapRelayMake(&AutomanData[RF_BAHAP_DATAPOS_RELAY_POS]);
          if(strRfRxData[RfRxEvent].Relay != data16temp){						
            
              strRfRxData[RfRxEvent].Relay = data16temp;		//s 최근데이터 업데이트
              sprintf(buffGLcd2Line, "IN EVENT:%d,%04x", strRfRxData[RfRxEvent].Id, strRfRxData[RfRxEvent].Relay);									
              TM_HD44780_Puts_State(0,1, buffGLcd2Line); 	              
              DawoonMqttSend(MQTT_IOT_BEHAP_STATUS_RESPONE, RfRxEvent+1, 0);                  

          }
        }
        else if(strRfRxData[RfRxEvent].Cmd  == RF_SUB_CMD_BAHAP_CHANGE_DATA) {
           strRfRxData[RfRxEvent].Relay = RfAutomanBahapRelayMake(&AutomanData[RF_BAHAP_DATAPOS_RELAY_POS]);	
          DawoonMqttSend(MQTT_IOT_BEHAP_CHANGE_RESPONE, RfRxEvent, RF_SUB_CMD_CHANGE);
//         if(strRfRxData[RfRxEvent].Mode == MQTT_TYPE_PENDING){
//                DawoonMqttSend(MQTT_IOT_HCOW_STAT, RfRxEvent + 1, 0);                
//         }

          cBuzzerCount = BUZZER_TIME_COUNT; 
          sprintf(buffGLcd2Line, "%d:SUB CHAN %04x ", RfRxEvent+1, strRfRxData[RfRxEvent].Relay);									
          TM_HD44780_Puts_State(0,1, buffGLcd2Line);
                            
          if(wErrorAutomanCheckCont[RfRxEvent]){	//s ask를 받으면 에러 카운트를 감소시킴
              wErrorAutomanCheckCont[RfRxEvent]--;
              if(wErrorAutomanCheckCont[RfRxEvent] > 10){		//s 잠깐 장애라고 생각하고 카운터 감소를 늘림
                      wErrorAutomanCheckCont[RfRxEvent]--;
              }
          }	          
          strRfRxData[RfRxEvent].RelayBack = strRfRxData[RfRxEvent].Relay;
          TRACE_DEBUG("RF_SUB_CMD_CHANGE %d, %04x\r\n", RfRxEvent, strRfRxData[RfRxEvent].Relay);

        }    				
        else {
            TRACE_DEBUG("What !!!!!!!!!!!!!!!!!![%d] \r\n", strRfRxData[RfRxEvent].Cmd);
        }

      }
   

}

void Automan_tx_InOutCont(RfDataStruct* strdata, uint8_t mode)
{
  unsigned int crc;
  uint8_t count = 0;
  uint8_t modetemp = 0;
  uint8_t Rfdata[30];
  
  if(mode){
    Rfdata[count++] = PACKET_STX1;                 //s  0xaa
    Rfdata[count++] = PACKET_STX2;                 //s  0xbb    
  }
  else{
    Rfdata[count++] = PACKET_STX3025;                 //s  0xab
  }
  
    Rfdata[count++] = AUTOMAN_SEND_OP_CODE;       
    Rfdata[count++] = AUTOMAN_DATA_SIZE;                     //s ID 1, ETX 1, CRC 2
    Rfdata[count++] = (uint8_t)CONFIG_FARM_PREFERENCE_1;       
    Rfdata[count++] = (uint8_t)(CONFIG_FARM_PREFERENCE_1 >> 8);                     //s fcode 2byte  
    Rfdata[count++] = strdata->Id;//2
    if(strdata->Mode == RF_DEVIEC_TPYE_ASK){	
            modetemp = RF_DEVIEC_TPYE_ASK;		
    }
    else if(strdata->Mode == MQTT_TYPE_16CH){
            modetemp = SUB_TYPE_16CH;
    }
    else if(strdata->Mode == MQTT_TYPE_8CH){
            modetemp = SUB_TYPE_8CH;											
    }
    else if(strdata->Mode == MQTT_TYPE_PENDING){
            modetemp = SUB_TYPE_PENDING;											
    }	
    else if(strdata->Mode == MQTT_TYPE_SMALL_8CH){
        modetemp = SUB_TYPE_SMALL_8CH;							
    }		
    
      wErrorAutomanCheckCont[strdata->Id - 1]++;		//s 에러 카운트를 위해 저장
      if(wErrorAutomanCheckCont[strdata->Id - 1] > 10000){	//s 오버플로 방지
              wErrorAutomanCheckCont[strdata->Id - 1] = 10000;
      }		
      if(wErrorAutomanCheckCont[strdata->Id - 1] > 20){               //s 정신 없으니 20이상인것만 표시
        TRACE_DEBUG("wErrorAutomanCheckCont[%d] :  [%d]\r\n", strdata->Id - 1, wErrorAutomanCheckCont[strdata->Id - 1]);
        if(wErrorAutomanCheckCont[strdata->Id - 1] > 30){
            DawoonMqttSendInfo(MQTT_IOT_INFO_ERROR, strdata->Id - 1, 0);            //s mqtt로 에러 송출
            strRfRxData[strdata->Id-1].Mode = 0;                //s 장비 연결을 없앰
        }
      }    
    

  Rfdata[count++] = modetemp;			//s 초창기 물어보고 그 값을 저장
  Rfdata[count++] = strdata->Cmd;

  Rfdata[count++] = (uint8_t)strdata->Relay;         
  Rfdata[count++] = (uint8_t)(strdata->Relay >> 8);       //s relay data 2byte 16bit값
  Rfdata[count++] = (uint8_t)strdata->RelayTimer;       
  Rfdata[count++] = (uint8_t)(strdata->RelayTimer >> 8);       //s relay data 2byte 16bit값
  Rfdata[count++] = strdata->Input;                   
  Rfdata[count++] = (uint8_t)strdata->InTimer1;              
  Rfdata[count++] = (uint8_t)(strdata->InTimer1 >> 8);           
  Rfdata[count++] = (uint8_t)strdata->InTimer2;              
  Rfdata[count++] = (uint8_t)(strdata->InTimer2 >> 8);              
  Rfdata[count++] = (uint8_t)strdata->InTimer3;              
  Rfdata[count++] = (uint8_t)(strdata->InTimer3 >> 8);     

  crc = PacketCRC16(&Rfdata[3+mode], AUTOMAN_EX_SIZE);         //s fcode 부터 crc 계산
  
  Rfdata[count++] = (uint8_t)crc&0xFF;   
  Rfdata[count++] = (uint8_t)(crc>>8);   
  
  Rfdata[count++] = PACKET_ETX;  
  
   HAL_UART_Transmit(&huart6, Rfdata, count, 10000);  		

}


void AutomanSetCommand(uint8_t cmd, uint8_t mode)
{

  uint8_t Rfdata[30];
  uint8_t count = 0;

  if(mode){
    Rfdata[count++] = PACKET_STX1;                 //s  0xaa
    Rfdata[count++] = PACKET_STX2;                 //s  0xbb    
    TRACE_DEBUG("AutomanSetCommand cmd:%d\r\n", cmd);    
    if((cmd == RF_SETTING_FREQ_CHECK) || (cmd == RF_SETTING_FREQ_CHECK2)){
      Rfdata[count++] = 0x05; 
    }
    else if(cmd == RF_SETTING_FREQ_SET){
      Rfdata[count++] = 0x03;           
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR1_9;   
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR2_10;         
    }

  }
  else{
    Rfdata[count++] = PACKET_STX3025;                 //s  0xab
    TRACE_DEBUG("AutomanSetCommand cmd:%d\r\n", cmd);          
    if(cmd == RF_SETTING_FREQ_CHECK || cmd == RF_SETTING_FREQ_CHECK2){
      Rfdata[count++] = 0x0A; 
    }
    else if(cmd == RF_SETTING_FREQ_SET){
      Rfdata[count++] = 0x04;           
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR1_9;   
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR2_10;   
      
    }
  }


  //TRACE_DEBUG("strdata->Mode : %d \r\n",strdata->Mode);
  HAL_UART_Transmit(&huart6, Rfdata, count, 10000);  		

}


void Automan_tx_InOutCont_Bebap(RfDataStruct* strdata, uint8_t mode)
{
  unsigned int crc;
  uint8_t Rfdata[30];
  uint8_t RelayData[8];
  uint8_t count = 0;
	
  if(mode){
    Rfdata[count++] = PACKET_STX1;                 //s  0xaa
    Rfdata[count++] = PACKET_STX2;                 //s  0xbb    
  }
  else{
    Rfdata[count++] = PACKET_STX3025;                 //s  0xab
  }
  
    Rfdata[count++] = AUTOMAN_SEND_OP_CODE;       
    Rfdata[count++] = AUTOMAN_BAHAP_DATA_SIZE;                     //s ID 1, ETX 1, CRC 2
    Rfdata[count++] = strdata->Id;
    
    wErrorAutomanCheckCont[strdata->Id - 1]++;		//s 에러 카운트를 위해 저장
//    if(wErrorAutomanCheckCont[strdata->Id - 1] > 10000){	//s 오버플로 방지
//            wErrorAutomanCheckCont[strdata->Id - 1] = 10000;
//    }		
//    if(wErrorAutomanCheckCont[strdata->Id - 1] > 20){               //s 정신 없으니 20이상인것만 표시
//      TRACE_DEBUG("wErrorAutomanCheckCont[%d] :  [%d]\r\n", strdata->Id - 1, wErrorAutomanCheckCont[strdata->Id - 1]);
//      if(wErrorAutomanCheckCont[strdata->Id - 1] > 30){
//          DawoonMqttSendInfo(MQTT_IOT_INFO_ERROR, strdata->Id - 1, 0);            //s mqtt로 에러 송출
//      }
//    }    

  Rfdata[count++] = strdata->Cmd;

  memset(RelayData, 0, sizeof(RelayData));
  
   switch(strdata->Relay)
    {
        case 1:
            RelayData[0] = 1;
            break;
        case 2:
            RelayData[0] = 2;
            break;
        
        case 3:
            RelayData[1] = 1;
            break;
        case 4:
            RelayData[1] = 2;
            break;
            
        case 5:
            RelayData[2] = 1;
            break;
        case 6:
            RelayData[2] = 2;
            break;
            
        case 7:
            RelayData[3] = 1;
            break;
        case 8:
            RelayData[3] = 2;
            break;
            
        case 9:
            RelayData[4] = 1;
            break;
        case 10:
            RelayData[4] = 2;
            break;                                   
    }   

   memcpy(&Rfdata[count], RelayData, sizeof(RelayData));
   
   count += 8;
   

  crc = PacketCRC16(&Rfdata[5+mode], AUTOMAN_BAHAP_EX_SIZE);         //s fcode 부터 crc 계산
  
  Rfdata[count++] = (uint8_t)crc&0xFF;   
  Rfdata[count++] = (uint8_t)(crc>>8);   
  
  Rfdata[count++] = PACKET_ETX;        
  //TRACE_DEBUG("Automan_tx_InOutCont_Bebap crc : %x %d\r\n",crc, strdata->Relay);
  HAL_UART_Transmit(&huart6, Rfdata, count, 10000);  		

}


//s 181023 계사 컨트롤로 추가됨 
void Automan_tx_TimeCont(uint8_t rfcount, uint8_t mode, TimeControlStruct* control)
{
	uint8_t event = RESET;
	
	uint8_t LightNegaFlag = RESET;
	
	switch(cAutoDeviceSendTiming[rfcount- 1]){
		case RF_CONTROL_FEEDER:
		//s 급여 1 처음에는 0이여도 동작
		if(strManualSwControl[rfcount- 1].cFeedSwFlag == RESET){
			if((wDawoonTime >= control->wFeedTime[0]) && control->cFeedTimeFlag[0]){	//s 현시간보다 셋팅 시간이 작을때 아직 수행하지 않았을때 
				//control->cFeedTimeFlag[0] = RESET;		//s 활성화 중지
				control->cFeedSendFlag = SET;			//s 수신 확인 후 활성화 중지
				control->cFeedPos = 0;
				strRfTxData[rfcount- 1].Relay = SW_CONTROL_FEEDER + 1;//s 위치 입력
				strRfTxData[rfcount- 1].RelayTimer = CONFIG_FEEDER_TIME_11;
				Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);			
	//			control->wMovingTime = wDawoonTime + (CONFIG_FEEDER_TIME_11 / 60) + 1;			//s 사료 온 타임 + 1분 이후 이송라인 작동
	//			control->wMovingTimeFlag = SET;
				event = SET;			//s 두번 통신 방지함
                                TM_HD44780_Puts_State(0,1, "                ");                                		
                                sprintf(buffGLcd2Line, "%d:FEEDER:%d ", rfcount, 1);									
                                TM_HD44780_Puts_State(0,1, buffGLcd2Line);         
                                
				TRACE_DEBUG("%d:FEEDER %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);
			}

		
			//s 급여 1~n	
			for(uint8_t i = 1; i < control->cFeedCnt; i ++){	//s 1~n번째는 for문
				if((wDawoonTime >= control->wFeedTime[i]) && control->cFeedTimeFlag[i] && (event == RESET)){
					if(control->wFeedTime[i] != 0){		//s 값이 0이면 셋팅 되지 않은것으로 간주 함
						//control->cFeedTimeFlag[i] = false;
						control->cFeedSendFlag = SET;	
						control->cFeedPos = i;					
						strRfTxData[rfcount- 1].Relay = SW_CONTROL_FEEDER + 1;//s 위치 입력				
						strRfTxData[rfcount- 1].RelayTimer = CONFIG_FEEDER_TIME_11;
						Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
	//					control->wMovingTime = wDawoonTime + (CONFIG_FEEDER_TIME_11 / 60) + 1;			//s 사료 온 타임 + 1분 이후 이송라인 작동
	//					control->wMovingTimeFlag = SET;		
						event = SET;				
                                                TM_HD44780_Puts_State(0,1, "                ");    							
                                                sprintf(buffGLcd2Line, "%d:FEEDER:%d ", rfcount, i+1);									
                                                TM_HD44780_Puts_State(0,1, buffGLcd2Line);         								
						TRACE_DEBUG("%d:FEEDER %d  time %02d:%02d\r\n", rfcount, i+1, wDawoonTime/60, wDawoonTime%60);
						break;				//s 사용부분 찾았으면 전송 후 브레이트 (RF연속 데이터 허용 불가)
					}
					else{
						control->cFeedTimeFlag[i] = RESET;
					}
				}
			}
		}
		break;
//		case RF_CONTROL_MOVEING:		
//		//s 이송라인 작동	
//		if((wDawoonTime >= control->wMovingTime) && control->wMovingTimeFlag && (event == false)){			//s 사료 공급 후 이송라인 동작 시간
//			control->wMovingTimeFlag = false;		
//			strRfTxData[rfcount- 1].Relay = SW_CONTROL_MOVEING + 1;//s 위치 입력		
//			strRfTxData[rfcount- 1].RelayTimer = CONFIG_MOVING_TIME_13;
//			Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);					
//			event = SET;
//			clcd.locate(0,1);
//			clcd.printf("                ");			
//			clcd.locate(0,1);									
//			clcd.printf("%d:MOVING:%d ", rfcount, 1);	
//					#ifdef DEBUG_PC						
//					TRACE_DEBUG("%d:MOVING %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);					
//					#endif				
//		}
//		break;
		case RF_CONTROL_LIGHT1:			
		//s 전등 1
		if(strManualSwControl[rfcount- 1].cLight1SwFlag == RESET){		
			if(control->wLightOnTime[0] > control->wLightOffTime[0]){
				LightNegaFlag = 1;
			}
			else if(control->wLightOnTime[0] == control->wLightOffTime[0]){
				LightNegaFlag = 2;				
			}
			else{
				LightNegaFlag = 0;				
			}
			
			if(LightNegaFlag == 1){			//s 점등과 소등이 반대 방향이면
				if((wDawoonTime >= control->wLightOffTime[0]) && (wDawoonTime < control->wLightOnTime[0]) && (event == RESET)){			//s 전등 on off
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;				//s 밝기 디밍				
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정	
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT OFF:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  							
					TRACE_DEBUG("%d:LIGHT OFF %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);	
		
				}
				else if(event == RESET){
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;				//s 밝기 디밍		
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정
					
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    						
                                        sprintf(buffGLcd2Line, "%d:LIGHT ON:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);					
					TRACE_DEBUG("%d:LIGHT ON %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);
				}
			}
			else if(LightNegaFlag == 2){			//s 같다면 on
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;				//s 밝기 디밍		
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정
					
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);		
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    						
                                        sprintf(buffGLcd2Line, "%d:LIGHT ON:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);					
					TRACE_DEBUG("%d:LIGHT ON %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);
			}	                        
			else{
				if((wDawoonTime >= control->wLightOnTime[0]) && (wDawoonTime < control->wLightOffTime[0]) && (event == RESET)){			//s 전등 on off
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;				//s 밝기 디밍		
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정
					
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT ON:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);	
							TRACE_DEBUG("%d:LIGHT ON %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);
				}
				else if(event == RESET){
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;				//s 밝기 디밍				
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정	
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT OFF:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  			
							TRACE_DEBUG("%d:LIGHT OFF %d  time %02d:%02d\r\n", rfcount, 1, wDawoonTime/60, wDawoonTime%60);
				}
			}			
		}
		break;
		case RF_CONTROL_LIGHT2:			
		//s 전등 2	
		if(strManualSwControl[rfcount- 1].cLight2SwFlag == RESET){				
			if(control->wLightOnTime[1] > control->wLightOffTime[1]){
				LightNegaFlag = 1;
			}
			else if(control->wLightOnTime[1] == control->wLightOffTime[1]){
				LightNegaFlag = 2;
			}				
			else{
				LightNegaFlag = 0;				
			}
			
			if(LightNegaFlag == 1){			//s 점등과 소등이 반대 방향이면
				if((wDawoonTime >= control->wLightOffTime[1]) && (wDawoonTime < control->wLightOnTime[1]) && (event == RESET)){			//s 전등 on off
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;					//s 밝기 디밍			
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정				
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT OFF:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  				
					TRACE_DEBUG("%d:LIGHT OFF %d  time %02d:%02d\r\n", rfcount, 2, wDawoonTime/60, wDawoonTime%60);
			
				}
				else if(event == RESET){
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;					//s 밝기 디밍			
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정		
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT ON:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  			
					TRACE_DEBUG("%d:LIGHT ON %d  time %02d:%02d\r\n", rfcount, 2, wDawoonTime/60, wDawoonTime%60);
				}	
			}
			else if(LightNegaFlag == 2){			//s 같다면 on
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;				//s 밝기 디밍		
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정
					
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT ON:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  			
					TRACE_DEBUG("%d:LIGHT ON %d  time %02d:%02d\r\n", rfcount, 2, wDawoonTime/60, wDawoonTime%60);
			}	                        
			else{
				
				if((wDawoonTime >= control->wLightOnTime[1]) && (wDawoonTime < control->wLightOffTime[1]) && (event == RESET)){			//s 전등 on off
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;					//s 밝기 디밍			
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정		
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT ON:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  			
					TRACE_DEBUG("%d:LIGHT ON %d  time %02d:%02d\r\n", rfcount, 2, wDawoonTime/60, wDawoonTime%60);
				}
				else if(event == RESET){
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_LIGHT2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					strRfTxData[rfcount- 1].InTimer1 = control->wLightTime;					//s 밝기 디밍			
					strRfTxData[rfcount- 1].InTimer2 = control->wLightBright;				//s 밝기 조정				
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    					
                                        sprintf(buffGLcd2Line, "%d:LIGHT OFF:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line);  				
							TRACE_DEBUG("%d:LIGHT OFF %d  time %02d:%02d\r\n", rfcount, 2, wDawoonTime/60, wDawoonTime%60);
				}	
			}
		}
		break;		
		case RF_CONTROL_HEATER1:			
		//s 히터 1
		if((strManualSwControl[rfcount- 1].cHeater1SwFlag == RESET) && (TempDataFirstFlag[rfcount- 1] == SET)){		
			if((wErrorTempCheckCont[rfcount- 1] > 100) || (wErrorSensorCheckCont[rfcount- 1] > 100)){		//s 온도 에러 발생시 동작 오프 시킴
					control->cHeaterOnFlag[0] = RESET;					//s 가운데 값에서 on되지 않게
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    								
                                        sprintf(buffGLcd2Line, "%d:HEATER ERR OFF:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
						TRACE_DEBUG("%d:HEATER ERR OFF:%d  temp%d\r\n", rfcount, 1, control->wTempData);
			}
			else{
				if((control->wTempData <= control->wHeaterOnValue[0]) && (event == RESET)){			//s 낮은온도에서 히터 온
					control->cHeaterOnFlag[0] = SET;
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    									
                                        sprintf(buffGLcd2Line, "%d:HEATER ON:%d", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
							TRACE_DEBUG("%d:HEATER ON %d  temp%d\r\n", rfcount, 1, control->wTempData);
				}
				else if((control->cHeaterOnFlag[0] == SET) && (control->wTempData <= control->wHeaterOffValue[0]) && (event == RESET)){	//s 오프값에 도달할때까지 on
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;	
                                        TM_HD44780_Puts_State(0,1, "                ");    								
                                        sprintf(buffGLcd2Line, "%d:HEATER M:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
							TRACE_DEBUG("%d:HEATER M %d  temp%d\r\n", rfcount, 1, control->wTempData);
				}
				else if((control->wTempData >= control->wHeaterOffValue[0]) && (event == RESET)){	//s 기준값에 도달했으면
					control->cHeaterOnFlag[0] = RESET;					//s 가운데 값에서 on되지 않게
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    									
                                        sprintf(buffGLcd2Line, "%d:HEATER OFF:%d ", rfcount, 1);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
							TRACE_DEBUG("%d:HEATER OFF %d  temp%d\r\n", rfcount, 1, control->wTempData);
				}		
			}
		}
		
		break;		
		case RF_CONTROL_HEATER2:		
		//s 히터 2	
		if((strManualSwControl[rfcount- 1].cHeater2SwFlag == RESET) && (TempDataFirstFlag[rfcount- 1] == SET)){					
			if((wErrorTempCheckCont[rfcount- 1] > 100) || (wErrorSensorCheckCont[rfcount- 1] > 100)){
					control->cHeaterOnFlag[0] = RESET;					//s 가운데 값에서 on되지 않게
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER1 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    								
                                        sprintf(buffGLcd2Line, "%d:HEATER ERR OFF:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
					TRACE_DEBUG("%d:HEATER ERR OFF:%d  temp%d\r\n", rfcount, 1, control->wTempData);
			}
			else{		
				if((control->wTempData <= control->wHeaterOnValue[1]) && (event == RESET)){			//s 낮은온도에서 히터 온
					control->cHeaterOnFlag[1] = SET;
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    								
                                        sprintf(buffGLcd2Line, "%d:HEATER ON:%d", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 						
							TRACE_DEBUG("%d:HEATER ON %d  temp%d\r\n", rfcount, 2, control->wTempData);
				}
				else if((control->cHeaterOnFlag[1] == SET) && (control->wTempData <= control->wHeaterOffValue[1]) && (event == RESET)){	//s 오프값에 도달할때까지 on
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;	
                                        TM_HD44780_Puts_State(0,1, "                ");    							
                                        sprintf(buffGLcd2Line, "%d:HEATER M:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
							TRACE_DEBUG("%d:HEATER M %d  temp%d\r\n", rfcount, 2, control->wTempData);
				}
				else if((control->wTempData >= control->wHeaterOffValue[1]) && (event == RESET)){	//s 기준값에 도달했으면
					control->cHeaterOnFlag[1] = RESET;					//s 가운데 값에서 on되지 않게
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER2 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    							
                                        sprintf(buffGLcd2Line, "%d:HEATER OFF:%d ", rfcount, 2);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 						
							TRACE_DEBUG("%d:HEATER OFF %d  temp%d\r\n", rfcount, 2, control->wTempData);
				}			
			}
		}
		break;		
		case RF_CONTROL_HEATER3:		
		//s 히터 3	
		if((strManualSwControl[rfcount- 1].cHeater3SwFlag == RESET) && (TempDataFirstFlag[rfcount- 1] == SET)){					
			if((wErrorTempCheckCont[rfcount- 1] > 100) || (wErrorSensorCheckCont[rfcount- 1] > 100)){
					control->cHeaterOnFlag[0] = RESET;					//s 가운데 값에서 on되지 않게
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER3 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    									
                                        sprintf(buffGLcd2Line, "%d:HEATER ERR OFF:%d ", rfcount, 3);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 						
					TRACE_DEBUG("%d:HEATER ERR OFF:%d  temp%d\r\n", rfcount, 1, control->wTempData);
			}
			else{				
				if((control->wTempData <= control->wHeaterOnValue[2]) && (event == RESET)){			//s 낮은온도에서 히터 온
					control->cHeaterOnFlag[2] = SET;
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER3 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;			
                                        TM_HD44780_Puts_State(0,1, "                ");    								
                                        sprintf(buffGLcd2Line, "%d:HEATER ON:%d", rfcount, 3);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 			
					TRACE_DEBUG("%d:HEATER ON %d  temp%d\r\n", rfcount, 3, control->wTempData);
				}
				else if((control->cHeaterOnFlag[2] == SET) && (control->wTempData <= control->wHeaterOffValue[2]) && (event == RESET)){	//s 오프값에 도달할때까지 on
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER3 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;	
                                        TM_HD44780_Puts_State(0,1, "                ");    							
                                        sprintf(buffGLcd2Line, "%d:HEATER M:%d ", rfcount, 3);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
					TRACE_DEBUG("%d:HEATER M %d  temp%d\r\n", rfcount, 3, control->wTempData);
				}
				else if((control->wTempData >= control->wHeaterOffValue[2]) && (event == RESET)){	//s 기준값에 도달했으면
					control->cHeaterOnFlag[2] = RESET;					//s 가운데 값에서 on되지 않게
					strRfTxData[rfcount- 1].Relay = SW_CONTROL_HEATER3 + 1;//s 위치 입력				
					strRfTxData[rfcount- 1].RelayTimer = 0;
					Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
					event = SET;
                                        TM_HD44780_Puts_State(0,1, "                ");    								
                                        sprintf(buffGLcd2Line, "%d:HEATER OFF:%d ", rfcount, 3);									
                                        TM_HD44780_Puts_State(0,1, buffGLcd2Line); 						
					TRACE_DEBUG("%d:HEATER OFF %d  temp%d\r\n", rfcount, 3, control->wTempData);
				}
			}
		}
		break;		
		default:
			cAutoDeviceSendTiming[rfcount- 1] = 0;
		break;					
	}
	cAutoDeviceSendTiming[rfcount- 1]++;
	
	if(cAutoDeviceSendTiming[rfcount- 1] == RF_CONTROL_LIGHT1){		//s 없는 데이터는 보내지 않음
		if(control->cLightCnt == 0){
			if(control->cHeaterCnt == 0){
				cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_FEEDER;
			}
			else{
				cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_HEATER1;
			}
		}

	}
  else if(cAutoDeviceSendTiming[rfcount- 1] == RF_CONTROL_LIGHT2){
		if(control->cLightCnt == 1){
			if(control->cHeaterCnt == 0){
				cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_FEEDER;
			}
			else{
				cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_HEATER1;
			}			
		}
	}
  else if(cAutoDeviceSendTiming[rfcount- 1] == RF_CONTROL_HEATER2){
		if(control->cHeaterCnt == 1){
			cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_FEEDER;
		}
	}
  else if(cAutoDeviceSendTiming[rfcount- 1] == RF_CONTROL_HEATER3){
		if(control->cHeaterCnt == 2){
			cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_FEEDER;
		}
	}	
	
	if(cAutoDeviceSendTiming[rfcount- 1] > RF_CONTROL_HEATER3){
		cAutoDeviceSendTiming[rfcount- 1] = RF_CONTROL_FEEDER;		
	}
}



//s 181218 16체널 온습도 컨트롤로 추가됨 
void AutomanTx16ChTempCont(uint8_t rfcount, uint8_t mode, Temp16ChControlStruct* control)
{
  uint8_t ChPos = cAutoDeviceChSend[rfcount-1];
  
  uint8_t TempNegaFlag = RESET;  
  uint8_t HumNegaFlag = RESET;  
  
    control->wHumData = strTimeControl[strTemp16ChControl[rfcount-1].cSensorId[ChPos]].wHumData;
    control->wTempData = strTimeControl[strTemp16ChControl[rfcount-1].cSensorId[ChPos]].wTempData;       
  
    if((control->c16ChManualFlag[ChPos] == SET) && (TempDataFirstFlag[control->cDeviceSetting - 1] == SET)){		
        if((wErrorTempCheckCont[rfcount- 1] > 100) || (wErrorSensorCheckCont[rfcount- 1] > 100)){		//s 온도 에러 발생시 동작 오프 시킴
            control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
            strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
            strRfTxData[rfcount- 1].RelayTimer = 0;
            Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);				
            TM_HD44780_Puts_State(0,1, "                ");    								
            sprintf(buffGLcd2Line, "%d:16CH %d ERR", rfcount, ChPos + 1);									
            TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
            TRACE_DEBUG("%d:16CH %d ERR OFF  temp%d\r\n", rfcount, 1, control->wTempData);
        }
        else{
          if((control->cTempEnalbeFlag[ChPos] == SET) && (control->cHumEnalbeFlag[ChPos] == SET)){
            if(control->w16ChTempOnValue[ChPos] < control->w16ChTempOffValue[ChPos])
              TempNegaFlag = SET;
            if(control->w16ChHumOnValue[ChPos] < control->w16ChHumOffValue[ChPos])
              HumNegaFlag = SET;            
            
            if(TempNegaFlag == SET && HumNegaFlag == SET){
              if((control->wTempData <= control->w16ChTempOnValue[ChPos]) || (control->wHumData <= control->w16ChHumOnValue[ChPos]) ){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("TempNegaFlag == SET && HumNegaFlag == SET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wTempData <= control->w16ChTempOffValue[ChPos]) 
                      || (control->wHumData <= control->w16ChHumOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("TempNegaFlag == SET && HumNegaFlag == SET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->wTempData >= control->w16ChTempOffValue[ChPos]) && (control->wHumData >= control->w16ChHumOffValue[ChPos]) ){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("TempNegaFlag == SET && HumNegaFlag == SET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }
            } 
            else if(TempNegaFlag == SET && HumNegaFlag == RESET){
              if((control->wTempData <= control->w16ChTempOnValue[ChPos]) || (control->wHumData >= control->w16ChHumOnValue[ChPos]) ){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("TempNegaFlag == SET && HumNegaFlag == RESET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wTempData <= control->w16ChTempOffValue[ChPos]) 
                      || (control->wHumData >= control->w16ChHumOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("TempNegaFlag == SET && HumNegaFlag == RESET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->wTempData >= control->w16ChTempOffValue[ChPos]) && (control->wHumData <= control->w16ChHumOffValue[ChPos]) ){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("TempNegaFlag == SET && HumNegaFlag == RESET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }              
              
            }
            else if(TempNegaFlag == RESET && HumNegaFlag == SET){            
              if((control->wTempData >= control->w16ChTempOnValue[ChPos]) || (control->wHumData <= control->w16ChHumOnValue[ChPos]) ){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("TempNegaFlag == RESET && HumNegaFlag == SET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wTempData >= control->w16ChTempOffValue[ChPos]) 
                      || (control->wHumData <= control->w16ChHumOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("TempNegaFlag == RESET && HumNegaFlag == SET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->wTempData <= control->w16ChTempOffValue[ChPos]) && (control->wHumData >= control->w16ChHumOffValue[ChPos]) ){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("TempNegaFlag == RESET && HumNegaFlag == SET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              } 
            }
            else if(TempNegaFlag == RESET && HumNegaFlag == RESET){
            
              if((control->wTempData >= control->w16ChTempOnValue[ChPos]) || (control->wHumData >= control->w16ChHumOnValue[ChPos]) ){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("TempNegaFlag == RESET && HumNegaFlag == RESET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wTempData >= control->w16ChTempOffValue[ChPos]) 
                      || (control->wHumData >= control->w16ChHumOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("TempNegaFlag == RESET && HumNegaFlag == RESET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->wTempData <= control->w16ChTempOffValue[ChPos]) && (control->wHumData <= control->w16ChHumOffValue[ChPos]) ){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("TempNegaFlag == RESET && HumNegaFlag == RESET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }	            
            }
          }
          else if(control->cTempEnalbeFlag[ChPos] == SET){
            if(control->w16ChTempOnValue[ChPos] < control->w16ChTempOffValue[ChPos])
              TempNegaFlag = SET;
            
            if(TempNegaFlag == SET){
              if(control->wTempData <= control->w16ChTempOnValue[ChPos]){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("TempNegaFlag == SET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wTempData <= control->w16ChTempOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("TempNegaFlag == SET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if(control->wTempData >= control->w16ChTempOffValue[ChPos]){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("TempNegaFlag == SET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }	            
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }        
            }        
            else if(TempNegaFlag == RESET){            
              if(control->wTempData >= control->w16ChTempOnValue[ChPos]){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("TempNegaFlag == RESET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wTempData >= control->w16ChTempOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("TempNegaFlag == RESET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if(control->wTempData <= control->w16ChTempOffValue[ChPos]){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("TempNegaFlag == RESET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }	            
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }	
            }
          }
           else if(control->cHumEnalbeFlag[ChPos] == SET){
            if(control->w16ChHumOnValue[ChPos] < control->w16ChHumOffValue[ChPos])
              HumNegaFlag = SET;        
            
            if(HumNegaFlag == SET){
              if(control->wHumData <= control->w16ChHumOnValue[ChPos]){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("HumNegaFlag == SET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wHumData <= control->w16ChHumOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("HumNegaFlag == SET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if(control->wHumData >= control->w16ChHumOffValue[ChPos]){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("HumNegaFlag == SET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }	              
            }
            else if(HumNegaFlag == RESET){
              if(control->wHumData >= control->w16ChHumOnValue[ChPos]){	//s 높은온도에서 높은 습도  ON
                  control->c16ChOnFlag[ChPos] = SET;					//s 가운데 값에서 on되지 않게              
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d ON", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 				
                  TRACE_DEBUG("HumNegaFlag == RESET  %d:16CH %d ON  temp%d Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if((control->c16ChOnFlag[ChPos] == SET) && (control->wHumData >= control->w16ChHumOffValue[ChPos])){	//s 오프값에 도달할때까지 on
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 600;				//s 통신 장애시 10분간 on유지
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    								
                  sprintf(buffGLcd2Line, "%d:16CH %d M", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 		
                  TRACE_DEBUG("HumNegaFlag == RESET  %d:16CH %d  M  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else if(control->wHumData <= control->w16ChHumOffValue[ChPos]){	//s 기준값에 도달했으면
                  control->c16ChOnFlag[ChPos] = RESET;					//s 가운데 값에서 on되지 않게
                  strRfTxData[rfcount- 1].Relay = ChPos + 1;//s 위치 입력				
                  strRfTxData[rfcount- 1].RelayTimer = 0;
                  Automan_tx_InOutCont(&strRfTxData[rfcount - 1], mode);
                  TM_HD44780_Puts_State(0,1, "                ");    									
                  sprintf(buffGLcd2Line, "%d:16CH %d  OFF", rfcount, ChPos);									
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 					
                  TRACE_DEBUG("HumNegaFlag == RESET  %d:16CH %d  OFF  temp%d  Hum%d\r\n", rfcount, ChPos, control->wTempData, control->wHumData);
              }
              else{
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;              
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
              }	
            }
            
          }         
        }
    }
    else{
      strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;      
      Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);             //s 해당 사항 없으므로 REQ로 보냄
    }		
    
    cAutoDeviceChSend[rfcount-1]++;     //s 다음 ch로 변경

    if(cAutoDeviceChSend[rfcount-1] >= 16){
      cAutoDeviceChSend[rfcount-1] = 0;
    }    
    
    for(uint8_t i = 0; i < 16; i++){
      if(control->c16ChSettingFlag[cAutoDeviceChSend[rfcount-1]] == SET){
        break; //s 데이터가 있음
      }
      else{
        if(cAutoDeviceChSend[rfcount-1] < 16){
          cAutoDeviceChSend[rfcount-1]++;
        }
        else{
          cAutoDeviceChSend[rfcount-1] = 0;          //s 16ch 한바퀴 돌았으므로 0부터 시작
        }
      }
    }    
      
    
}

void RfAutomanDataProcess(uint8_t mode)
{
    uint8_t i;  
    uint8_t TxPos = 0;  

    if(_delay10++ >= 10) {
        _delay10 = 0;
      if(AutoLoopCount++> cAutomanSendTimming) {
				
        AutoLoopCount = 0;
        
        for(i = 0; i < CONFIG_RF_DEVICE_COUNT_2; i++)
        {
          if(strRfTxData[i].RelayEvent == SET){
            TxPos = i + 1;
            break;
          }
        }
        
        if(mqtt_First_connect){		//s mqtt 연결때만 수행됨
          if(!first_check){
            //최초 연결을 위한 루틴!!
            RfRxAliveCount++;
            strRfTxData[RfRxAliveCount-1].Id = RfRxAliveCount;
            strRfTxData[RfRxAliveCount-1].Mode = RF_DEVIEC_TPYE_ASK;							
            strRfTxData[RfRxAliveCount-1].Cmd = RF_MAIN_CMD_ALIVE;
            Automan_tx_InOutCont(&strRfTxData[RfRxAliveCount - 1], mode);			
            if(RfRxAliveCount >= CONFIG_RF_DEVICE_COUNT_2){
                    first_check = SET;
                    //RfRxAliveCount = 0;									
            }								
          }
          else if(TxPos){		//s 보낼 이벤트 데이터가 있다면 0이면 없음이므로 1이상으로 표시됨
              if(strRfTxData[TxPos - 1].Mode == MQTT_TYPE_SMALL_8CH){
                  if(strRfTxData[TxPos - 1].Relay == (SW_CONTROL_FEEDER + 1)){
                      strManualSwControl[TxPos - 1].cFeedSwFlag = SET;								
                      strManualSwControl[TxPos - 1].wFeedManualTime = CONFIG_FEEDER_TIME_11;		//s 60초로 박자		
                      strRfRxData[TxPos - 1].RelayBack |= 0x0001;									//s 급여는 전환이 빠름                      
                      TRACE_DEBUG("SW_CONTROL_FEEDER [%d]\r\n", TxPos-1);	
                  }
                  else if(strRfTxData[TxPos - 1].Relay == (SW_CONTROL_LIGHT1 + 1)){
                      strManualSwControl[TxPos - 1].cLight1SwFlag = SET;								
                      strManualSwControl[TxPos - 1].wLight1ManualTime = 60;		//s 60초로 박자					
                      TRACE_DEBUG("SW_CONTROL_LIGHT1 [%d]\r\n", TxPos-1);	
                  }
                  else if(strRfTxData[TxPos - 1].Relay == (SW_CONTROL_LIGHT2 + 1)){
                      strManualSwControl[TxPos - 1].cLight2SwFlag = SET;								
                      strManualSwControl[TxPos - 1].wLight2ManualTime = 60;		//s 60초로 박자					
                      TRACE_DEBUG("SW_CONTROL_LIGHT2 [%d]\r\n", TxPos-1);							
                  }
                  else if(strRfTxData[TxPos - 1].Relay == (SW_CONTROL_HEATER1 + 1)){
                      strManualSwControl[TxPos - 1].cHeater1SwFlag = SET;								
                      strManualSwControl[TxPos - 1].wHeater1ManualTime = 60;		//s 60초로 박자					
                      TRACE_DEBUG("SW_CONTROL_HEATER1 [%d]\r\n", TxPos-1);						
                  }
                  else if(strRfTxData[TxPos - 1].Relay == (SW_CONTROL_HEATER2 + 1)){
                      strManualSwControl[TxPos - 1].cHeater2SwFlag = SET;								
                      strManualSwControl[TxPos - 1].wHeater2ManualTime = 60;		//s 60초로 박자	
                      TRACE_DEBUG("SW_CONTROL_HEATER2 [%d]\r\n", TxPos-1);						
                  }
                  else if(strRfTxData[TxPos - 1].Relay == (SW_CONTROL_HEATER3 + 1)){
                      strManualSwControl[TxPos - 1].cHeater3SwFlag = SET;								
                      strManualSwControl[TxPos - 1].wHeater3ManualTime = 60;		//s 60초로 박자			
                      TRACE_DEBUG("SW_CONTROL_HEATER3 [%d]\r\n", TxPos-1);	
                  }											
                              
              }		
              
              strRfTxData[TxPos - 1].RelayEvent = RESET;
              Automan_tx_InOutCont(&strRfTxData[TxPos - 1], mode);		

              cBuzzerCount = BUZZER_TIME_COUNT;    
              
              sprintf(buffGLcd2Line, "MQTT CHAN:%d",TxPos);		
              TM_HD44780_Puts_State(0,1, buffGLcd2Line);
              TRACE_DEBUG("RF_MAIN_CMD_CHANGE [%d] [%d] TxPos[%d]\r\n", strRfTxData[TxPos - 1].Mode, strRfTxData[TxPos - 1].Id, TxPos);
          }
          else if(bDeviceCheck){
            RfRxCheckCount++;
            strRfTxData[RfRxCheckCount-1].Id = RfRxCheckCount;
            strRfTxData[RfRxCheckCount-1].Mode = strRfRxData[RfRxCheckCount-1].Mode;							
            strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_REQ;
//            if(strRfRxData[RfRxCheckCount-1].Mode == MQTT_TYPE_PENDING)         //s 게류식인 경우 상태를 계속 요청
//            {
//                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);		
//            }
            if(strRfRxData[RfRxCheckCount-1].Mode == MQTT_TYPE_SMALL_8CH){         //s 계사의 경우 정보를 계속 업데이트함
              strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_CONTROL;
              if(RfDeviceEnableFlag[RfRxCheckCount-1] == SET){			//s 서버에서 데이터를 읽어 왔을때만 동작함
                if(strTimeControl[RfRxCheckCount-1].cDataChangeFlag){
                                        
                  if(strTimeControl[RfRxCheckCount-1].cDataChangeFlag & (1 << RF_CONTROL_LIGHT1)){
                          cAutoDeviceSendTiming[RfRxCheckCount-1] = RF_CONTROL_LIGHT1;
                  }
                  else if(strTimeControl[RfRxCheckCount-1].cDataChangeFlag & (1 << RF_CONTROL_LIGHT2)){
                          cAutoDeviceSendTiming[RfRxCheckCount-1] = RF_CONTROL_LIGHT2;
                  }
                  else if(strTimeControl[RfRxCheckCount-1].cDataChangeFlag & (1 << RF_CONTROL_HEATER1)){
                          cAutoDeviceSendTiming[RfRxCheckCount-1] = RF_CONTROL_HEATER1;
                  }
                  else if(strTimeControl[RfRxCheckCount-1].cDataChangeFlag & (1 << RF_CONTROL_HEATER2)){
                          cAutoDeviceSendTiming[RfRxCheckCount-1] = RF_CONTROL_HEATER2;
                  }
                  else if(strTimeControl[RfRxCheckCount-1].cDataChangeFlag & (1 << RF_CONTROL_HEATER3)){
                          cAutoDeviceSendTiming[RfRxCheckCount-1] = RF_CONTROL_HEATER3;
                  }				
                  strTimeControl[RfRxCheckCount-1].cDataChangeFlag = 0;
                }									
                
                Automan_tx_TimeCont(RfRxCheckCount, mode, &strTimeControl[RfRxCheckCount-1]);
              }
             
            }
            else if(strRfRxData[RfRxCheckCount-1].Mode == MQTT_TYPE_16CH){ //s 16ch인 경우 온습도 체크 하는지 여부 확인
              if(strTemp16ChControl[RfRxCheckCount-1].cDeviceSetting == SET){           
                strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_FAN_CONTROL;
                AutomanTx16ChTempCont(RfRxCheckCount, mode, &strTemp16ChControl[RfRxCheckCount-1]);
              }
              else{
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);
              }		
            }            
            else if(strRfRxData[RfRxCheckCount-1].Mode == MQTT_TYPE_PENDING ||strRfRxData[RfRxCheckCount-1].Mode == MQTT_TYPE_8CH){ //s 게류식인 경우 상태를 계속 요청
                Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);		             
            }
            
            if(RfRxCheckCount >= CONFIG_RF_DEVICE_COUNT_2){
                    bDeviceCheck = RESET;
                    RfRxCheckCount = 0;									
            }
          }
          else if(bReAliveCheck){
            RfRxCheckCount++;
            strRfTxData[RfRxCheckCount-1].Id = RfRxCheckCount;
            strRfTxData[RfRxCheckCount-1].Mode = RF_DEVIEC_TPYE_ASK;							
            strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_ALIVE;
            Automan_tx_InOutCont(&strRfTxData[RfRxCheckCount - 1], mode);			
            if(RfRxCheckCount >= CONFIG_RF_DEVICE_COUNT_2){
                    bReAliveCheck = RESET;
                    RfRxCheckCount = 0;										
            }											
          }
        }
      }

     if(mqtt_First_connect){     
        iAutomanCheckCount = cAutomanSendTimming * (CONFIG_RF_DEVICE_COUNT_2 + 2);		//s 통신 제어가 교체 루틴
        if(AutoCheckCount ++ >= iAutomanCheckCount) {
          //10초에 한번씩 상태 확인하기!!
          AutoCheckCount = 0;
          cReAliveCount++;
          if(cReAliveCount < AUTOMAN_ALIVE_TIMMING){	//s 주기적 체크 부분
            bDeviceCheck = SET;							
          }
          else{			                                                //s 10번에 한번 체크
            cReAliveCount = 0;
            bReAliveCheck = SET;
          }
        }
    }
  }
  
  
}


void RfAutomanBahapDataProcess(uint8_t mode)
{
    uint8_t i;  
    uint8_t TxPos = 0;  

    if(_delay10++ >= 10) {
        _delay10 = 0;
      if(AutoLoopCount++> cAutomanSendTimming) {
				
        AutoLoopCount = 0;
        
        for(i = 0; i < CONFIG_RF_DEVICE_COUNT_2; i++)
        {
          if(strRfTxData[i].RelayEvent == SET){
            TxPos = i + 1;
            break;
          }
        }
        
        if(mqtt_First_connect){		//s mqtt 연결때만 수행됨
          if(!first_check){
            //최초 연결을 위한 루틴!!
            RfRxAliveCount++;
            strRfTxData[RfRxAliveCount-1].Id = RfRxAliveCount;			
            strRfTxData[RfRxAliveCount-1].Cmd = RF_MAIN_CMD_BAHAP_REQ;
            Automan_tx_InOutCont_Bebap(&strRfTxData[RfRxAliveCount - 1], mode);			
            if(RfRxAliveCount >= CONFIG_RF_DEVICE_COUNT_2){
                    first_check = SET;
                    //RfRxAliveCount = 0;									
            }								
          }
          else if(TxPos){		//s 보낼 이벤트 데이터가 있다면 0이면 없음이므로 1이상으로 표시됨
              strRfTxData[TxPos - 1].RelayEvent = RESET;        
              Automan_tx_InOutCont_Bebap(&strRfTxData[TxPos - 1], mode);		

              cBuzzerCount = BUZZER_TIME_COUNT;    
              
              sprintf(buffGLcd2Line, "MQTT CHAN:%d",TxPos);		
              TM_HD44780_Puts_State(0,1, buffGLcd2Line);
              TRACE_DEBUG("RF_MAIN_CMD_CHANGE [%d] TxPos[%d]\r\n", strRfTxData[TxPos - 1].Id, TxPos);
          }
          else if(bDeviceCheck){
            RfRxCheckCount++;
            strRfTxData[RfRxCheckCount-1].Id = RfRxCheckCount;					
            strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_BAHAP_REQ;
       
            Automan_tx_InOutCont_Bebap(&strRfTxData[RfRxCheckCount - 1], mode);		             

            
            if(RfRxCheckCount >= CONFIG_RF_DEVICE_COUNT_2){
                    bDeviceCheck = RESET;
                    RfRxCheckCount = 0;									
            }
          }
          else if(bReAliveCheck){
            RfRxCheckCount++;
            strRfTxData[RfRxCheckCount-1].Id = RfRxCheckCount;					
            strRfTxData[RfRxCheckCount-1].Cmd = RF_MAIN_CMD_BAHAP_REQ;
            Automan_tx_InOutCont_Bebap(&strRfTxData[RfRxCheckCount - 1], mode);			
            if(RfRxCheckCount >= CONFIG_RF_DEVICE_COUNT_2){
                    bReAliveCheck = RESET;
                    RfRxCheckCount = 0;										
            }											
          }
        }
      }

     if(mqtt_First_connect){     
       
        if((CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_3025_BEHAP) || (CONFIG_RF_TYPE_3 == DEVICE_TYPE_AUTOMAN_BEHAP)){
          iAutomanCheckCount = cAutomanSendTimming * (CONFIG_RF_DEVICE_COUNT_2 + 5);		//s 통신 제어가 교체 루틴        
        }
        else{
          iAutomanCheckCount = cAutomanSendTimming * (CONFIG_RF_DEVICE_COUNT_2 + 2);		//s 통신 제어가 교체 루틴                 
        }
        if(AutoCheckCount ++ >= iAutomanCheckCount) {
          //10초에 한번씩 상태 확인하기!!
          AutoCheckCount = 0;
          cReAliveCount++;
          if(cReAliveCount < AUTOMAN_ALIVE_TIMMING){	//s 주기적 체크 부분
            bDeviceCheck = SET;							
          }
          else{			                                                //s 10번에 한번 체크
            cReAliveCount = 0;
            bReAliveCheck = SET;
          }
        }
    }
  }
  
}


void RfAutomanSetProcess(uint8_t mode)
{
  
  switch(cRfSettingState){
  case RF_SETTING_START_WAIT:
      cRfRetryCount = 0;
      cRfFreqRetryCount = 0;
      cRfSettingState = RF_SETTING_FREQ_CHECK;
      break;
    case RF_SETTING_FREQ_CHECK:
      AutomanSetCommand(RF_SETTING_FREQ_CHECK, mode);
      cRfSettingState = RF_SETTING_FREQ_CHECK_WAIT;
      cRfSettingDelay = 0;
      TRACE_DEBUG("RF_SETTING_FREQ_CHECK\r\n");                
      break;
    case RF_SETTING_FREQ_CHECK_WAIT:    
      if(cRfSettingDelay > 100){          //s 200ms
        cRfRetryCount++;
        cRfSettingState = RF_SETTING_FREQ_CHECK; 
        TRACE_DEBUG("RF_SETTING_FREQ_CHECK_WAIT\r\n");                    
      }
      break;    
    case RF_SETTING_FREQ_SET:
      AutomanSetCommand(RF_SETTING_FREQ_SET, mode);
      cRfSettingState = RF_SETTING_FREQ_SET_WAIT;
      cRfSettingDelay = 0;
      TRACE_DEBUG("RF_SETTING_FREQ_SET\r\n");          
      break;      
    case RF_SETTING_FREQ_SET_WAIT:    
      if(cRfSettingDelay > 100){          //s 200ms
        cRfSettingState = RF_SETTING_FREQ_CHECK2;        
        cRfRetryCount = 0;        
      }
      break;    
    case RF_SETTING_FREQ_CHECK2:
      AutomanSetCommand(RF_SETTING_FREQ_CHECK2, mode);
      cRfSettingState = RF_SETTING_FREQ_CHECK2_WAIT;
      cRfSettingDelay = 0;
      TRACE_DEBUG("RF_SETTING_FREQ_CHECK2\r\n");        
      break;
    case RF_SETTING_FREQ_CHECK2_WAIT:    
      if(cRfSettingDelay > 100){          //s 200ms
        cRfRetryCount++;
        cRfSettingState = RF_SETTING_FREQ_CHECK2;        
      }
      break;     
    case RF_SETTING_END:    
      cRfProcessMode = 1;
      break;       
    default:
        break;    
  }
  
  if(cRfRetryCount > 2){
      sprintf(buffGLcd1Line, "AUTOMAN SET ERR1");		
      TM_HD44780_Puts_State(0,0, buffGLcd1Line);    
      TRACE_DEBUG("AUTOMAN SET ERROR [%d] FREQ[%d][%d]\r\n", mode,CONFIG_RF_FREQ_ADDR1_9, CONFIG_RF_FREQ_ADDR2_10);      
      cRfProcessMode = 1;
  }
  
  cRfSettingDelay++;        
  
}


void RfAutomanLoop(uint8_t mode)
{
    
    if(CONFIG_RF_DEVICE_COUNT_2 == 0){
      return;           //s 오토만 사용안하는 관계로 사용하지 않음
    }
          
    RfAutomanParse(mode);
    
    
    if(cRfProcessMode){            //s 셋팅 완료후 데이터 전송
      RfAutomanDataProcess(mode);
    }
    else{
      RfAutomanSetProcess(mode);      
    }


}


void RfAutomanBehapLoop(uint8_t mode)
{

    
    if(CONFIG_RF_DEVICE_COUNT_2 == 0){
      return;           //s 오토만 사용안하는 관계로 사용하지 않음
    }
    
    RfAutomanBahapParse(mode);

    if(cRfProcessMode){            //s 셋팅 완료후 데이터 전송
      RfAutomanBahapDataProcess(mode);
    }
    else{
      RfAutomanSetProcess(mode);      
    }    

}

void Check_Page2(uint8_t data)
{
    if(lcd_icon.current_page == 0x02)
          {
              if(data == 0x01)
              {
                NxLcdclick(6,1);
                NxLcdclick(7,0);
              }
              else if(data == 0x02)
              {
                NxLcdclick(6,0);
                NxLcdclick(7,1);
              }
              else if(data== 0x03)
              {
                NxLcdclick(9,1);
                NxLcdclick(10,0);
              }
              else if(data == 0x03)
              {
                NxLcdclick(9,0);
                NxLcdclick(10,1);
              }
              else if(data == 0x04)
              {
                NxLcdclick(15,1);
                NxLcdclick(16,0);
              }
              else if(data == 0x05)
              {
                NxLcdclick(15,0);
                NxLcdclick(16,1);
              }
              else if(data == 0x07)
              {
                NxLcdclick(21,1);
                NxLcdclick(22,0);
              }
              else if(data == 0x08)
              {
                NxLcdclick(21,0);
                NxLcdclick(22,1);
              }
              else if(data == 0x09)
              {
                NxLcdclick(3,1);
                NxLcdclick(4,0);
              }
              else if(data == 0x0A)
              {
                NxLcdclick(3,0);
                NxLcdclick(4,1);
              }
              else if(data == 0x0B)
              {
                NxLcdclick(12,1);
                NxLcdclick(13,0);
              }
              else if(data == 0x0C)
              {
                NxLcdclick(12,0);
                NxLcdclick(13,1);
              }
              else if(data == 0x0D)
              {          
                NxLcdclick(18,1);
                NxLcdclick(19,0);
              }
              else if(data == 0x0E)
              {
                NxLcdclick(18,0);
                NxLcdclick(19,1);
              }
              else if(data== 0x0F)
              {
                NxLcdclick(24,1);
                NxLcdclick(25,0);
              }
              else if(data == 0x10)
              {
                NxLcdclick(24,0);
                NxLcdclick(25,1);
              }
           }   
}

void Check_Page3(uint8_t data)
{
      if(lcd_icon.current_page ==  0x03)
      {
              if(data == 1)
              {
                NxLcdclick(3,1);
                NxLcdclick(4,0);
              }
              else if(data == 2)
              {
                NxLcdclick(3,0);
                NxLcdclick(4,1);
              }
              else if(data == 3)
              {
                NxLcdclick(6,1);
                NxLcdclick(7,0);
              }
              else if(data == 4)
              {
                NxLcdclick(6,0);
                NxLcdclick(7,1);
              }
              else if(data == 5)
              {
                NxLcdclick(9,1);
                NxLcdclick(10,0);
              }
              else if(data== 6)
              {
                NxLcdclick(9,0);
                NxLcdclick(10,1);
              }
              else if(data == 7)
              {
                NxLcdclick(12,1);
                NxLcdclick(13,0);
              }
              else if(data == 8)
              {
                NxLcdclick(12,0);
                NxLcdclick(13,1);
              }
              else if(data== 9)
              {
                NxLcdclick(15,1);
                NxLcdclick(16,0);
              }
              else if(data == 0x0A)
              {
                NxLcdclick(15,0);
                NxLcdclick(16,1);
              }
              else if(data == 0x0B)
            {
              NxLcdclick(24,1);
              NxLcdclick(23,0);
            }
              else if(data== 0x0C)
            {
              NxLcdclick(24,0);
              NxLcdclick(23,1);
            }
              else if(data== 0x0D)
            {
              NxLcdclick(21,1);
              NxLcdclick(22,0);
            }
              else if(data == 0x0E)
            {
              NxLcdclick(21,0);
              NxLcdclick(22,1);
            }
              else if(data == 0x0F)
            {
              NxLcdclick(18,1);
              NxLcdclick(17,0);
            }
              else if(data == 0x10)
            {
              NxLcdclick(18,0);
              NxLcdclick(17,1);
            }
      }
}

void Check_Page4(uint8_t data)
{
      if(lcd_icon.current_page == 4)
      {
          if(data == 1)
          {
            NxLcdclick(3,1);
            NxLcdclick(4,0);
          }
          else if(data == 2)
          {
            NxLcdclick(3,0);
            NxLcdclick(4,1);
          }
          else if(data == 3)
          {
            NxLcdclick(6,1);
            NxLcdclick(7,0);
          }
          else if(data == 4)
          {
            NxLcdclick(6,0);
            NxLcdclick(7,1);
          }
          else if(data == 5)
          {
            NxLcdclick(9,1);
            NxLcdclick(10,0);
          }
          else if(data == 6)
          {
            NxLcdclick(9,0);
            NxLcdclick(10,1);
          }
          else if(data == 7)
          {
            NxLcdclick(12,1);
            NxLcdclick(13,0);
          }
          else if(data == 8)
          {
            NxLcdclick(12,0);
            NxLcdclick(13,1);
          }
          else if(data == 9)
          {
            NxLcdclick(15,1);
            NxLcdclick(16,0);
          }
          else if(data == 10)
          {
            NxLcdclick(15,0);
            NxLcdclick(16,1);
          }
          else if(data == 11)
          {
            NxLcdclick(24,1);
            NxLcdclick(23,0);
          }
          else if(data == 12)
          {
            NxLcdclick(24,0);
            NxLcdclick(23,1);
          }
          else if(data == 13)
          {
            NxLcdclick(21,1);
            NxLcdclick(22,0);
          }
          else if(data == 14)
          {
            NxLcdclick(21,0);
            NxLcdclick(22,1);
          }
          else if(data == 15)
          {
            NxLcdclick(18,1);
            NxLcdclick(17,0);
          }
          else if(data == 16)
          {
            NxLcdclick(18,0);
            NxLcdclick(17,1);
          }
      }
}
void Set_Page16(uint16_t data)
{

    uint16_t tmp = 0;
    if(lcd_icon.current_page == 4)
    {
        data = (data >> 8) & 0xFF;
    }
    
    for(uint8_t i = 0; i < 8; i++)
    {
        tmp = data & (0x0001 << i);
        
        if(lcd_icon.current_page == 3)
        {
            if(tmp == 0)
            {
                Check_Page3((i*2)+1);
            }
            else
                Check_Page3((i*2)+2);
        }
        else if(lcd_icon.current_page == 4)
        {
            if(tmp == 0)
            {
                Check_Page4((i*2)+1);
            }
            else
                Check_Page4((i*2)+2);
        }
        HAL_Delay(75U);
    }
      lcd_icon.iconbuffer[lcd_icon.current_page] =  data16temp;
}

char* pString[3] = { "0", "1", "2"};
void Set_Page8(char* String)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        if(String[i] == *pString[0])
        {
            if(i == 0)
            {
                NxLcdclick(3,1);
                NxLcdclick(6,0);
                NxLcdclick(5,0);
            }
            else if(i == 1)
            {
                NxLcdclick(19,0);
                NxLcdclick(20,0);
                NxLcdclick(21,1);
            }
            else if(i == 2)
            {
                NxLcdclick(22,0);
                NxLcdclick(23,0);
                NxLcdclick(24,1);
            }
            else if(i == 3)
            {
                NxLcdclick(25,0);
                NxLcdclick(26,0);
                NxLcdclick(27,1);
            }
            else if(i == 4)
            {
                NxLcdclick(28,1);
                NxLcdclick(29,0);
                NxLcdclick(30,0);
            }
            else if(i == 5)
            {
                NxLcdclick(31,0);
                NxLcdclick(32,0);
                NxLcdclick(33,1);
            }
            else if(i == 6)
            {
                NxLcdclick(34,1);
                NxLcdclick(35,0);
                NxLcdclick(36,0);
            }
            else if(i == 7)
            {
                NxLcdclick(37,1);
                NxLcdclick(38,0);
                NxLcdclick(39,0);
            }
        }
        else if(String[i] == *pString[1])
        {
            if(i == 0)
            {
                NxLcdclick(3,0);
                NxLcdclick(5,0);
                NxLcdclick(6,1);
            }
            else if(i == 1)
            {
                NxLcdclick(19,1);
                NxLcdclick(20,0);
                NxLcdclick(21,0);
            }
            else if(i == 2)
            {
                NxLcdclick(22,1);
                NxLcdclick(23,0);
                NxLcdclick(24,0);
            }
            else if(i == 3)
            {
                NxLcdclick(25,1);
                NxLcdclick(26,0);
                NxLcdclick(27,0);
            }
            else if(i == 4)
            {
                NxLcdclick(28,0);
                NxLcdclick(29,0);
                NxLcdclick(30,1);
            }
            else if(i == 5)
            {
                NxLcdclick(31,1);
                NxLcdclick(32,0);
                NxLcdclick(33,0);

            }
            else if(i == 6)
            {
                NxLcdclick(34,0);
                NxLcdclick(35,0);
                NxLcdclick(36,1);
            }
            else if(i == 7)
            {
                NxLcdclick(37,0);
                NxLcdclick(38,0);
                NxLcdclick(39,1);
            }
        }
        else if(String[i] == *pString[2])
        {
            if(i == 0)
            {
                NxLcdclick(3,0);
                NxLcdclick(5,1);
                NxLcdclick(6,0);
            }
            else if(i == 1)
            {
                NxLcdclick(19,0);
                NxLcdclick(20,1);
                NxLcdclick(21,0);
            }
            else if(i == 2)
            {
                NxLcdclick(22,0);
                NxLcdclick(23,1);
                NxLcdclick(24,0);
            }
            else if(i == 3)
            {
                NxLcdclick(25,0);
                NxLcdclick(26,1);
                NxLcdclick(27,0);
            }
            else if(i == 4)
            {
                NxLcdclick(28,0);
                NxLcdclick(29,1);
                NxLcdclick(30,0);
            }
            else if(i == 5)
            {
                NxLcdclick(31,0);
                NxLcdclick(32,1);
                NxLcdclick(33,0);
            }
            else if(i == 6)
            {
                NxLcdclick(34,0);
                NxLcdclick(35,1);
                NxLcdclick(36,0);
            }
            else if(i == 7)
            {
                NxLcdclick(37,0);
                NxLcdclick(38,1);
                NxLcdclick(39,0);
            }
        }
        HAL_Delay(50U);
    }
}






