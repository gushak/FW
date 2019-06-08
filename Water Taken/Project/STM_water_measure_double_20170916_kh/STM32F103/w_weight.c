#include "hw_control.h"

uint8_t Co2[5];
uint8_t Temp[5];
uint8_t Humidity[5];

uint32_t response;
uint16_t humidity,temperature;

uint16_t Co2_value = 0;

Wight_struct Parse_st;

char Weight_Dim[10];
volatile uint16_t Stable_weight_ch1 = 0, weight_ch1 = 0;
volatile uint16_t Stable_weight_ch2 = 0, weight_ch2 = 0;

void weight_ParseData_ch1(char* Buffer)
{
    char *token, *last;		
    char *buf = NULL;
    uint8_t tcnt = 0;
    buf = Buffer;
    
    while((token = strtok_r(buf,",", &last))!= NULL)
    {
        buf = NULL;
        
        if(tcnt == 0) 
          sprintf((char *)&Parse_st.name1, "%s", token);
        else if(tcnt == 1)
          sprintf((char *)&Parse_st.name2, "%s", token);
        else if(tcnt == 2)
          sprintf((char *)&Parse_st.name3, "%s", token);
        else if(tcnt == 3)
          sprintf((char *)&Parse_st.value, "%s", token);
        
        if(++tcnt >= 5) break;
    }		

    tcnt = 0;
    uint8_t len = 0;
    
    memset((void *)&Weight_Dim, 0, sizeof(Weight_Dim));
   
    for(int i=0; i < strlen((char const *)&Parse_st.value); i++)
    {
      if(Parse_st.value[i] >= '0' && Parse_st.value[i] <= '9')
      {
          Weight_Dim[len++] = Parse_st.value[i];
          if(len >= 10) break;
      }		
    }
    
    //weight = atoi((const char *)&Weight_Dim);
    weight_ch1 = atoi((const char *)&Weight_Dim);
    
    if(Parse_st.name1[1] == 'T') 
    {
        //LED1_FL;
          Stable_weight_ch1 = weight_ch1;
    }
} 

void weight_ParseData_ch2(char* Buffer)
{
    char *token, *last;		
    char*buf = NULL;
    uint8_t tcnt = 0;
    buf = Buffer;
    
    sprintf("%s\r\n", Buffer);
    
    while((token = strtok_r(buf,",", &last))!= NULL)
    {
        buf = NULL;
        
        if(tcnt == 0) 
            sprintf((char *)&Parse_st.name1, "%s", token);
        else if(tcnt == 1) 
            sprintf((char *)&Parse_st.name2, "%s", token);
        else if(tcnt == 2)
            sprintf((char *)&Parse_st.name3, "%s", token);
        else if(tcnt == 3)
            sprintf((char *)&Parse_st.value, "%s", token);
        
        if(++tcnt >= 5) break;
    }		

    tcnt = 0;
    uint8_t len = 0;
    
    memset((void *)&Weight_Dim, 0, sizeof(Weight_Dim));
    for(int i=0; i < strlen((char const *)&Parse_st.value); i++)
    {
        if(Parse_st.value[i] >= '0' && Parse_st.value[i] <= '9')
        {
            Weight_Dim[len++] = Parse_st.value[i];
            if(len >= 10) break;
        }		
    }
    
    weight_ch2 = atoi((const char *)&Weight_Dim);
    
    if(Parse_st.name1[1] == 'T') 
    {
        //LED1_FL;
        Stable_weight_ch2 = weight_ch2;
    }
} 
extern volatile uint8_t Stable_ch_flag;
volatile char Wbuf[100];
void parse_weight(void)
{	
    static uint8_t cntx = 0;
    static uint8_t stable_ch_flag_old = 0;
    static uint8_t dummy_cnt = 0;
    
    uint8_t c = 0;
    if(!uart2_is_empty())
    {
        c = uart2_dequeue();
        
        if(c == 0x0A && cntx == 21) 
        {
          Wbuf[cntx] = 0x0A;
              

          if(stable_ch_flag_old != Stable_ch_flag)
          {
              dummy_cnt = 3;
              stable_ch_flag_old = Stable_ch_flag;
          }
          else 
          {		
              if(dummy_cnt != 0)
              {
                  dummy_cnt--;
              } 
              else 
              {
                  if(Stable_ch_flag != 0) weight_ParseData_ch1((char *)&Wbuf);
                  else weight_ParseData_ch2((char *)&Wbuf);
              }
          }        
              cntx = 0;
              memset((void *)&Wbuf, 0, sizeof(Wbuf));
        } 
        else if(cntx > 22) 
        {
            cntx = 0;
            memset((void *)&Wbuf, 0, sizeof(Wbuf));
        } 
        else 
        {
            Wbuf[cntx++] = c;			
        }
    }
}

void parse_weight_interrupt(uint8_t c)
{
    static uint8_t cntx = 0;
#ifndef WEIGHT_SINGLE
    static uint8_t stable_ch_flag_old = 0;
    static uint8_t dummy_cnt = 0;
#endif
	
    if(c == 0x0A && cntx >= 21) 
    {
        Wbuf[cntx] = 0x0A;
            
#ifdef WEIGHT_SINGLE
        weight_ParseData_ch1((char *)&Wbuf);
#else		
        if(stable_ch_flag_old != Stable_ch_flag)
        {
            dummy_cnt = 1;
            stable_ch_flag_old = Stable_ch_flag;
        }
        else 
        {		
            if(dummy_cnt != 0) 
            {
              dummy_cnt--;
            } else 
            {
              if(Stable_ch_flag != 0) 
                weight_ParseData_ch1((char *)&Wbuf);
              else 
                weight_ParseData_ch2((char *)&Wbuf);
            }
        }
  #endif
      
        cntx = 0;
        memset((void *)&Wbuf, 0, sizeof(Wbuf));
    } 
    else if(cntx > 22) 
    {
      cntx = 0;
      memset((void *)&Wbuf, 0, sizeof(Wbuf));
    } 
    else 
    {
      Wbuf[cntx++] = c;			
    }		
}