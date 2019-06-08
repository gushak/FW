//#include "hw_control.h"


#include "main.h"
#include "rf_modules.h"
#include <stdlib.h> 
#include <string.h>
#include "usart.h"
#include "khh_eeprom.h"
#include "khh_lcd.h"

strAutomanPacket rx_packet_Rf_ch1;
//uint8_t  buffRfRxData[LORA_BUFF_SIZE];
uint8_t  cUartRxDataCount = 0;

//s lora rx compare
uint8_t* TextUbd2 = "Ubd2";
uint8_t* TextCmd = "Cmd";
uint8_t* TextCpkt = " C-pkt received.";


char buffUartRxExtract[LORA_EXTRACT_BUFF_SIZE][LORA_EXTRACT_BUFF_SIZE]; 

uint8_t cRfLegData = 0;;


uint8_t AsciiExtract(uint8_t* string, uint8_t size)
{
  uint8_t i, j;
  uint8_t k = 0;
  
  memset(buffUartRxExtract, 0, LORA_BUFF_SIZE);  
  
  for(i = 0; i < size; i++)
  {
    for(j = 0; j < size; j++)    
    {
      if(string[k] == ':')
      {
        k++;
        break;
      }
      else
      {
        buffUartRxExtract[i][j] = string[k];    //s not : text     
        k++;        
      }
      
      if(k >= (size - 1))
      {
        return i;                    //s complte
      }
    }
    if(k >= (size - 1))
    {
        return i;                    //s complte
    }
  }
  return i;
  
}


uint8_t LoraTrdpcDataCheck(uint8_t* buffdata)
{
  uint8_t i;
  uint8_t falseture = 0;
  
  for(i = 0; i < 4; i++)
  {
    if(buffUartRxExtract[0][i] != TextUbd2[i])
    {
      falseture = SET;
    }
  }
  if(falseture == RESET)
  {
    for(i = 0; i < 3; i++)
    {
      if(buffUartRxExtract[1][i] != TextCmd[i])
      {
        falseture = SET;
      }
    }
  }
  if(falseture == RESET)
  {
      if(atoi(buffUartRxExtract[2]) != 56)
      //if(atoi(buffUartRxExtract[2]) != 37)
      {
         falseture = SET;
      }
  }        
  if(falseture == RESET)
  {
      for(i = 0; i < 8; i++)
      {
        if(buffUartRxExtract[3][i+2] < 0x29 || buffUartRxExtract[3][i+2] > 0x3A)        //s if it dose not Relay Setting data
        {
          falseture = 2;
          return falseture;             
        }
        
          buffdata[i] = buffUartRxExtract[3][i+2] - 0x30;           //s ascii to decimal
      }
    
//      if(atoi(buffUartRxExtract[3]) != 1)
//      {
//         falseture = SET;
//      }
  }     
  return falseture;
}
  
uint8_t LoraTrupRxDataCheck(void)
{
  uint8_t i = 0;
  uint8_t falseture = 0;
  

  if(buffUartRxExtract[0][i] != 'D')
  {
    falseture = SET;
  }
  
  if(falseture == RESET)
  {
    
    for(i = 0; i < 16; i++)
    {
      if(buffUartRxExtract[1][i] != TextCpkt[i])
      {
        falseture = SET;
      }
    }
  }
  return falseture;
}




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

void SendPacket_Rf(RfDataStruct* strdata)
{
	unsigned int crc;
        uint8_t count = 0;
        uint8_t Rfdata[100]; 
        
        if(CONFIG_RF_TYPE == AUTOMAN_RF_CONTROL){
          Rfdata[count++] = PACKET_STX1;                 //s  0xaa
          Rfdata[count++] = PACKET_STX2;                 //s  0xbb 
        }
        else if(CONFIG_RF_TYPE == AUTOMAN3025_RF_CONTROL){
           Rfdata[count++] = PACKET_STX3025;                 //s  0xab        
        }
	
  
        Rfdata[count++] = AUTOMAN_SEND_OP_CODE;       
	Rfdata[count++] = AUTOMAN_DATA_SIZE;                     //s ID 1, ETX 1, CRC 2
        Rfdata[count++] = (uint8_t)CONFIG_FARM_PREFERENCE;       
	Rfdata[count++] = (uint8_t)(CONFIG_FARM_PREFERENCE >> 8);                     //s fcode 2byte  
        Rfdata[count++] = (uint8_t)CONFIG_RF_ADDR;
        if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
          Rfdata[count++] = DEVICE_TYPE_PENDING;          
        }
        else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
          Rfdata[count++] = DEVICE_TYPE_8CH;                  
        }
        else{
          Rfdata[count++] = (uint8_t)CONFIG_DEVIEC_TPYE;
        }
        Rfdata[count++] = strdata->Cmd;

        Rfdata[count++] = (uint8_t)strdata->Relay;       
	Rfdata[count++] = (uint8_t)(strdata->Relay >> 8);       //s relay data 2byte 16bit값
        Rfdata[count++] = 0;       
        Rfdata[count++] = 0;                     //s  relay time 필요 없음

        Rfdata[count++] = strdata->Input;                   
        Rfdata[count++] = (uint8_t)strdata->InTimer1;              
        Rfdata[count++] = (uint8_t)(strdata->InTimer1 >> 8);           
        Rfdata[count++] = (uint8_t)strdata->InTimer2;              
        Rfdata[count++] = (uint8_t)(strdata->InTimer2 >> 8);              
        Rfdata[count++] = (uint8_t)strdata->InTimer3;              
        Rfdata[count++] = (uint8_t)(strdata->InTimer3 >> 8);              
        
	crc = PacketCRC16(&Rfdata[3+CONFIG_RF_TYPE], AUTOMAN_EX_SIZE);         //s fcode 부터 crc 계산
	
        Rfdata[count++] = (uint8_t)crc&0xFF;   
        Rfdata[count++] = (uint8_t)(crc>>8);   
        
	Rfdata[count++] = PACKET_ETX;        
       
       // for(uint8_t i = 0; i < sizeof(Rfdata); i++)
          //printf("%02x \r\n",Rfdata[i]);
        
        HAL_UART_Transmit(&huart3, Rfdata, count, 10000);  
}


void AutomanSetCommand(uint8_t cmd, uint8_t mode)
{

  uint8_t Rfdata[30];
  uint8_t count = 0;

  if(mode){
    Rfdata[count++] = PACKET_STX1;                 //s  0xaa
    Rfdata[count++] = PACKET_STX2;                 //s  0xbb    
    //printf("AutomanSetCommand cmd:%d\r\n", cmd);    
    if((cmd == RF_SETTING_FREQ_CHECK) || (cmd == RF_SETTING_FREQ_CHECK2)){
      Rfdata[count++] = 0x05; 
    }
    else if(cmd == RF_SETTING_FREQ_SET){
      Rfdata[count++] = 0x03;           
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR1;   
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR2;         
    }

  }
  else{
    Rfdata[count++] = PACKET_STX3025;                 //s  0xab
    //printf("AutomanSetCommand cmd:%d\r\n", cmd);          
    if(cmd == RF_SETTING_FREQ_CHECK || cmd == RF_SETTING_FREQ_CHECK2){
      Rfdata[count++] = 0x0A; 
    }
    else if(cmd == RF_SETTING_FREQ_SET){
      Rfdata[count++] = 0x04;           
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR1;   
      Rfdata[count++] = CONFIG_RF_FREQ_ADDR2;   
      
    }
  }
  

  //printf("strdata->Mode : %d \r\n",strdata->Mode);
  HAL_UART_Transmit(&huart3, Rfdata, count, 10000);  		

}


//uint16_t cdksdkfjlsjd;
//uint16_t cdakdsjdfls2;
//uint8_t ksdjflksjdcount = 0;
//uint8_t buffdefaultdata[256];
uint8_t PorcessPacket_Rf_check(uint8_t rx_data, uint8_t* buffdata)
{
	unsigned int crc = 0;
        uint8_t result = 0;
        
//        if(CONFIG_RF_TYPE ==  LORA_RF_CONTROL)
//        {
//            buffRfRxData[cUartRxDataCount] = rx_data;
//            if((buffRfRxData[cUartRxDataCount] == '\n')  && (buffRfRxData[cUartRxDataCount - 1] == '\r')) 
//            {
//              memcpy(buffdata, buffRfRxData, cUartRxDataCount+1);
//              memset(buffRfRxData, 0, LORA_BUFF_SIZE);    
//              //cRxComplteState = cUartRxDataCount;
//              result = cUartRxDataCount;
//              cUartRxDataCount = 0;
//              return result;
//
//            }
//            else
//            {
//              cUartRxDataCount++;
//            }
//        }
        if(CONFIG_RF_TYPE ==  AUTOMAN_RF_CONTROL || CONFIG_RF_TYPE == AUTOMAN3025_RF_CONTROL)          
        {
          if(cRfProcessMode){     //s data 모드
          
          
            //printf("rx[%02x]", rx_data);
            switch(rx_packet_Rf_ch1.status)
            {
                    case RXSTATUS_IDLE:
                      if(rx_data == AUTOMAN_DATA_SIZE){
                          rx_packet_Rf_ch1.status = RXSTATUS_FARMCODE1;                   
                          rx_packet_Rf_ch1.size =  AUTOMAN_EX_SIZE;
                          cRfLegData = AUTOMAN_DATA_SIZE;
                          //printf("RXSTATUS_IDLE\r\n");                                  
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
                        if(rx_packet_Rf_ch1.fcode == CONFIG_FARM_PREFERENCE){          //s 농장코드가 같다면 다음 진행
                          cUartRxDataCount++;        
                          rx_packet_Rf_ch1.status = RXSTATUS_ID;
                        }
                        else{
                          PorcessPacket_Rf_Clear();   
                          //printf("Fcode differ %d\r\n", rx_packet_Rf_ch1.fcode);                          
                        }
                        break;
                    case RXSTATUS_ID:
                      if(rx_data == CONFIG_RF_ADDR){
                            rx_packet_Rf_ch1.status = RXSTATUS_MODE;
                            rx_packet_Rf_ch1.idx = rx_data;
                            rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                               
                            cUartRxDataCount++;  
                      }
                      else{
                          PorcessPacket_Rf_Clear();   
                          //printf("Rf id differ %d\r\n", rx_data);
                      }                             
                      break;     
                    case RXSTATUS_MODE:
                      if(rx_data == DEVICE_TYPE_PENDING && CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
                            rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                            //rx_packet_Rf_ch1.idx = rx_data;
                            rx_packet_Rf_ch1.mode = rx_data;
                            rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                               
                            cUartRxDataCount++;  

                      }                 
                      if(rx_data == DEVICE_TYPE_8CH && CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
                            rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                            //rx_packet_Rf_ch1.idx = rx_data;
                            rx_packet_Rf_ch1.mode = rx_data;
                            rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                               
                            cUartRxDataCount++;  

                      }            
                      else{
                        if(rx_data == CONFIG_DEVIEC_TPYE  || rx_data == RF_DEVIEC_TPYE_ASK){          // 응답부분 추가
                              rx_packet_Rf_ch1.status = RXSTATUS_CMD;
                              //rx_packet_Rf_ch1.idx = rx_data;
                              rx_packet_Rf_ch1.mode = rx_data;
                              rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;                               
                              cUartRxDataCount++;  
                        }
                        else{
                            PorcessPacket_Rf_Clear();   
                            //printf("type differ %d %d\r\n", rx_data, CONFIG_DEVIEC_TPYE);
                        }                          
                      }
                    break;
                    case RXSTATUS_CMD:
                      if(rx_data == RF_MAIN_CMD_ALIVE || rx_data == RF_MAIN_CMD_REQ  || rx_data == RF_MAIN_CMD_CHANGE || rx_data == RF_MAIN_CMD_ACK || 
                         rx_data == RF_MAIN_CMD_CONTROL  ||  rx_data == RF_MAIN_CMD_FAN_CONTROL|| rx_data == RF_MAIN_PENDING_AUTO_START || rx_data == RF_MAIN_PENDING_AUTO_STOP || rx_data == RF_MAIN_CMD_LCD){
                          rx_packet_Rf_ch1.status = RXSTATUS_DATA;
                          rx_packet_Rf_ch1.cmd = rx_data;	
                          rx_packet_Rf_ch1.data[cUartRxDataCount] = rx_data;     
                          cUartRxDataCount++;                          
                          //printf("RXSTATUS_ID\r\n"); 
                      }
                      else{
                          PorcessPacket_Rf_Clear();   
                          //printf("Commend error %d\r\n", rx_data);                        
                      }
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
                            //cdksdkfjlsjd = crc;
                            crcresult  = PacketCRC16(rx_packet_Rf_ch1.data, rx_packet_Rf_ch1.size);
                            //cdakdsjdfls2 = crcresult;
                            cUartRxDataCount++;  
                            //CRC가 정상이면
                            if(crc == crcresult)			//hhlee			
                            {				
                                if(cUartRxDataCount == AUTOMAN_DATA_SIZE){        //s 길이가 같은지 확인
                                                                      
                                    if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_REQ){
                                        result  = RF_MAIN_CMD_REQ;                        
                                    }
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_CHANGE){
                                      memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                      result = RF_MAIN_CMD_CHANGE;
                                    }
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_CONTROL){
                                      memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                      result = RF_MAIN_CMD_CONTROL;
                                    }          
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_FAN_CONTROL){
                                      memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                      result = RF_MAIN_CMD_FAN_CONTROL;
                                    }                                            
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_ACK){
                                       result = RF_MAIN_CMD_ACK;                                   
                                    } 
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_ALIVE){
                                       result = RF_MAIN_CMD_ALIVE;                                   
                                    } 
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_PENDING_AUTO_START){
                                       result = RF_MAIN_PENDING_AUTO_START;                                   
                                    } 
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_PENDING_AUTO_STOP){
                                       result = RF_MAIN_PENDING_AUTO_STOP;                                   
                                    }     
                                    else if(rx_packet_Rf_ch1.cmd == RF_MAIN_CMD_LCD)
                                    {
                                      memcpy(buffdata, rx_packet_Rf_ch1.data, AUTOMAN_EX_SIZE);
                                      result = RF_MAIN_CMD_LCD;
                                    }
                                    PorcessPacket_Rf_Clear();                                    
                                    return result;
                                }
                                else{
                                        //printf("cUartRxDataCount Erorr %d \r\n",  cUartRxDataCount);                                             
                                        PorcessPacket_Rf_Clear();   
                                        return result;
                                }
                            } 
                            else {
                                    //CRC ERROR 
                                  //crc = PacketCRC16(rx_packet_Rf_ch1.data, rx_packet_Rf_ch1.size);
                                  //printf("RF CRC Erorr [%04x] [%04x]\r\n",  crc, crcresult);
                                  PorcessPacket_Rf_Clear();                             
                            }
                          }
                          else{
                              //printf("RF ETX Erorr %x\r\n", rx_data);
                              PorcessPacket_Rf_Clear();                                  
                          }
                      break;		
                    default :
  //                            buffdefaultdata[ksdjflksjdcount++] = rx_data;
                              PorcessPacket_Rf_Clear();     
                        break;
              
            }	
            return result;          
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
                        if((rx_packet_Rf_ch1.data[0] == CONFIG_RF_FREQ_ADDR1) && (rx_packet_Rf_ch1.data[1] == CONFIG_RF_FREQ_ADDR2)){                //s rf 주파수가 맞다면
                            cRfProcessMode = 1;
                            //printf("RF_SETTING_FREQ_SET END\r\n");                           
                        }
                        else{             //s 틀리다면
                          cRfSettingState = RF_SETTING_FREQ_SET;
                          cRfFreqRetryCount++;
                          if(cRfFreqRetryCount > 2){
                            sprintf(buffGLcd1Line, "AUTOMAN SET ERR2");		
                            TM_HD44780_Puts_State(0,0, buffGLcd1Line);                                
                            //printf("RF_SETTING_FREQ_SET cRfFreqRetryCount [%d]\r\n",cRfFreqRetryCount);    
                            cRfProcessMode = 1;
                          }
                        }
//                        printf("rx_packet_Rf_ch1.data[0] [%d]  rx_packet_Rf_ch1.data[1]  [%d]\r\n",rx_packet_Rf_ch1.data[0], rx_packet_Rf_ch1.data[1]);                                    
                        PorcessPacket_Rf_Clear();             
                    break;                        
                        
                    default :
                         PorcessPacket_Rf_Clear();     
                        break;                        
                  
              }
            }            
            
            return 0;
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
      //printf("RF_SETTING_FREQ_CHECK\r\n");                
      break;
    case RF_SETTING_FREQ_CHECK_WAIT:    
      if(cRfSettingDelay > 10){          //s 200ms
        cRfRetryCount++;
        cRfSettingState = RF_SETTING_FREQ_CHECK; 
        //printf("RF_SETTING_FREQ_CHECK_WAIT\r\n");                    
      }
      break;    
    case RF_SETTING_FREQ_SET:
      AutomanSetCommand(RF_SETTING_FREQ_SET, mode);
      cRfSettingState = RF_SETTING_FREQ_SET_WAIT;
      cRfSettingDelay = 0;
      //printf("RF_SETTING_FREQ_SET\r\n");          
      break;      
    case RF_SETTING_FREQ_SET_WAIT:    
      if(cRfSettingDelay > 10){          //s 200ms
        cRfSettingState = RF_SETTING_FREQ_CHECK2;        
        cRfRetryCount = 0;        
      }
      break;    
    case RF_SETTING_FREQ_CHECK2:
      AutomanSetCommand(RF_SETTING_FREQ_CHECK2, mode);
      cRfSettingState = RF_SETTING_FREQ_CHECK2_WAIT;
      cRfSettingDelay = 0;
      //printf("RF_SETTING_FREQ_CHECK2\r\n");        
      break;
    case RF_SETTING_FREQ_CHECK2_WAIT:    
      if(cRfSettingDelay > 10){          //s 200ms
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
      //printf("AUTOMAN SET ERROR [%d] FREQ[%d][%d]\r\n", mode,CONFIG_RF_FREQ_ADDR1, CONFIG_RF_FREQ_ADDR2);      
      cRfProcessMode = 1;
  }
  
  cRfSettingDelay++;        
  
}

