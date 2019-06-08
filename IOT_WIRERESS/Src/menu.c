//#include "hw_control.h"
#include "menu.h"
#include "khh_lcd.h"
#include "khh_eeprom.h"

volatile uint8_t Setting_enable = 0;    
volatile uint8_t SW_S[3] = {0,};	//s   짧게 SW 누른 상태 
volatile uint8_t SW_L[3] = {0,};	//s   길게 1초 SW 눌린 상태 
volatile uint8_t SW_Push[3] = {0,};     //s 눌린 상태 체크
volatile uint8_t SW_Cnt[3] = {0,}; 	//s 길게 눌린 키 카운트

#define SW_LONG_CNT 100	                //s 1.5초 셋팅


#define	MENU_CNT		25              //s khh_eeprom.h 

volatile uint8_t menu_key_wait=0;  
volatile uint8_t Menu = 0;  
volatile uint8_t setValue = 0;  


volatile uint16_t MAX_Value[MAX_EEPROM_ADDRESS]  = {  9999,           //s Farm Code  
                                                                                        6,           //s Device Type
											16,              //s rf id
                                                                                        1,               //s rf type       
											65534,           //s  out1 on시간

                                                                                        65534,           //s  out2 on시간
											65534,           //s  out3 on시간
                                                                                        65534,          //s  out4 on시간
											65534,           //s  out5 on시간
                                                                                        65534,           //s  out6 on시간

											65534,           //s  out7 on시간
                                                                                        65534,          //s  out8 on시간
											65534,           //s  out9 on시간
                                                                                        65534,           //s  out10 on시간
											65534,           //s  out11 on시간

                                                                                        65534,          //s  out12 on시간
											65534,           //s  out13 on시간
                                                                                        65534,           //s  out14 on시간
											65534,           //s  out15 on시간
                                                                                        65534,          //s  out16 on시간

                                                                                        3,	            //s   moving Count    
											1,                 //s   reset enable 
                                                                                        65534,           //s
											65534,           //s  
                                                                                        65534,     

                                                                                        65534,	
											65534,           //s  out13 on시간
                                                                                        65534,           //s  out14 on시간
											65534,           //s  out15 on시간
                                                                                        65534,         //s  out16 on시간
                                                                                          
                                                                                        3,	            //s   moving Count    
											1,                 //s   reset enable 
                                                                                        65534,           //s
											65534,           //s  
                                                                                        65534,     

                                                                                        65534,	
											65534,           //s  out13 on시간
                                                                                        65534,           //s  out14 on시간
											65534,           //s  out15 on시간
                                                                                        65534         //s  out16 on시간                                                                                          


};       //{ 9, 999, 9, 999, 99, 99, 9, 9, 9, 999};       


volatile uint16_t MIN_Value[MAX_EEPROM_ADDRESS] = {1,1,1,1,1,
                                                                                    1,1,1,1,1,
                                                                                    1,1,1,1,1,
                                                                                    1,1,1,1,1,
                                                                                    1,1,1,1,1,
                                                                                    1,1,1,1,1,
                                                                                    1,1,1,1,1,
                                                                                    1,1,1,1,1,                                                                                    
};
volatile uint16_t Setting_delay = 0;

//extern char buf_menu_num[16]; 
//extern volatile uint16_t FET_value[10];

#define SW_LONG_ADD		10


void MeunTitleCopy(void)
{
  if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_16CH){
    sprintf(buffGLcd1Line, "IOT RF 16CH     ");                  
    TM_HD44780_Puts_State(0,0,buffGLcd1Line);	 
  }
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH){
    sprintf(buffGLcd1Line, "IOT RF 8CH      ");                  
    TM_HD44780_Puts_State(0,0,buffGLcd1Line);	 
  }
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){
    sprintf(buffGLcd1Line, "IOT RF PENDING  ");                  
    TM_HD44780_Puts_State(0,0,buffGLcd1Line);	 
  }  
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL){
    sprintf(buffGLcd1Line, "IOT RF CHICK      ");                  
    TM_HD44780_Puts_State(0,0,buffGLcd1Line);	 
  }    
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){
    sprintf(buffGLcd1Line, "IOT RF PEND 8CH ");                  
    TM_HD44780_Puts_State(0,0,buffGLcd1Line);	 
  }
  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
    sprintf(buffGLcd1Line, "IOT RF 8CH LIMIT ");                  
    TM_HD44780_Puts_State(0,0,buffGLcd1Line);	 
  }
  
}


void menu_list();

void SW_check(void)
{
	if(SW1==0) {
		if(SW_L[0]==0) {	
			if(SW_Cnt[0]++ >= SW_LONG_CNT){ 
				SW_Push[0] = 0;	
				SW_Cnt[0] = 0;		
				SW_L[0] = 1;
                                Setting_enable = 1;
                                cBuzzerOnOff = SET;
                                cBuzzerCount = BUZZER_TIME_COUNT;                                
                                //printf("SW1 LONG KEY \r\n");                                
			} 
                        else {
				SW_Push[0] = 1; 
			}
		} 
                else {
			SW_Push[0] = 1; 
		}
	} 
        else {
		if(SW_Push[0] == 1) {
			SW_Push[0] = 0; //누름
			//SW_S[0] = 1;	
			SW_S[0] ++;
                        cBuzzerOnOff = SET;
                        cBuzzerCount = BUZZER_TIME_COUNT;                              
		}
		SW_Cnt[0] = 0;	
	}
		
	if(SW2==0) {
		if(SW_L[1] == 0) {
			if(SW_Cnt[1]++>= SW_LONG_CNT) {
				SW_Push[1] = 0;
				SW_Cnt[1] = 0;
				SW_L[1] = 1;
                                cBuzzerOnOff = SET;
                                cBuzzerCount = BUZZER_TIME_COUNT;                                      
                                //printf("SW2 LONG KEY \r\n");                                          
			} 
                        else {
				SW_Push[1] = 1;
			}
		} 
                else {
			SW_Push[1] = 1;
		}
		
	} 
        else {
		if(SW_Push[1] == 1) {
			SW_Push[1] = 0;
			SW_S[1] ++ ;
                        cBuzzerOnOff = SET;
                        cBuzzerCount = BUZZER_TIME_COUNT;                              
		}
		SW_Cnt[1] =0;
	} 
	
	if(SW3==0) {
		if(SW_L[2] == 0) {
			if(SW_Cnt[2]++>= SW_LONG_CNT) {
				SW_Push[2] = 0;
				SW_Cnt[2] = 0;
				SW_L[2] = 1;
                                cBuzzerOnOff = SET;
                                cBuzzerCount = BUZZER_TIME_COUNT;                                      
                                //printf("SW3 LONG KEY \r\n");                                          
			} 
                        else {
				SW_Push[2] = 1;
			}
		} 
                else {
			SW_Push[2] = 1;
		}
		
	} 
        else {
		if(SW_Push[2] == 1) {
			SW_Push[2] = 0;
			//SW_S[2] =1 ;
			SW_S[2] ++ ;
                        cBuzzerOnOff = SET;
                        cBuzzerCount = BUZZER_TIME_COUNT;                                 
		}
		SW_Cnt[2] =0;
		
	}
	
	
}

//uint8_t setting_mode(char* str0, char* str1)
void setting_mode(void)
{
	static uint16_t sw_long_add=0;
        //uint8_t enable = 0;
        //uint8_t changed = 0;
	//char str[100];
        //uint16_t tempdata = 0;
	
	if(Setting_delay !=0){
            Setting_delay--;
            //return enable;
            return;
        }

	if(Setting_enable == 1) {

		//다음으로 넘어가는거 방지
		if(SW1 == 1 && menu_key_wait == 0 ) 
		{   //설정버튼을 길게눌럿을때   	
			//Delay(300);				  
                    HAL_Delay(300);          //s 0.3초
                    menu_key_wait = 1; 
                    SW_L[0] = 0; 
                    SW_S[0] = 0;
                    //printf("SW1 == 1 && menu_key_wait == 0 \r\n");
		}
		else if(SW_L[0] != 0 && menu_key_wait == 1)             //s 메뉴 상태에서 엔터키  길게 입력하였을 경우     
		{
			//종료 이탈 
			SW_L[0] = 0;                                            
			Setting_enable = 0;                                                                   
			menu_key_wait = 0;
			//main_key_wait = 0;          
			Menu = 0;
			Setting_delay = 20;
                    //printf("SW_L[0] != 0 && menu_key_wait == 1\r\n");			
                    //return enable;
                    return;
		} 
		
		else if(SW_S[0] != 0 && menu_key_wait == 1)            //s 메뉴 상태에서 엔터키  짧게 입력하였을 경우     
		{
			SW_S[0] = 0;                                                        
			
			Setting_enable = 2;
			menu_key_wait = 0;			
			Setting_delay = 5;	
                        //printf("SW_S[0] != 0 && menu_key_wait == 1\r\n");		                        
                        //return enable;
                        return;
		}
	
		if(SW_S[1] != 0 || SW_L[1] != 0) {                      //s     Up 키 입력
			Menu++; 
                        SW_S[1] = 0; 
                        SW_L[1] = 0; 
                        //printf("SW_S[1] != 0 || SW_L[1] != 0\r\n");	                
                }  
		
		else if(SW_S[2] != 0 || SW_L[2] != 0) {                 //s   Down 키 입력
			Menu--; 
                        SW_S[2] = 0;
                        SW_L[2] = 0; 
                        //printf("SW_S[2] != 0 || SW_L[2] != 0\r\n");	                            
		}                      

		if(Menu <= 1){                   //s  처음 Menu  위치가 0일때 1로 표시
                    Menu = 1;
                    //printf("Menu <= 1\r\n");	                    
                }
		else if(Menu > MENU_CNT) {                       //s   Menu  위치가 10을 넘어서면 메뉴에서 빠져나옴                                                                              
		  Setting_enable = 0;
		  Menu = 0;
		  
		  //main_key_wait = 0;
                  menu_key_wait = 0;
		  SW_L[0] = 0;
		  SW_S[0] = 0;
//                  sprintf(str0, "IOT I/O Control "); 
//                  sprintf(str1, "                "); 
                  cReLcdDisplay = 0;                    
	  
                  MeunTitleCopy();
                  sprintf(buffGLcd2Line, "                "); 
                  //TM_HD44780_Puts(0,1,buffGLcd2Line);
                  cRelaySendState = RELAY_DATA_SEND;                        
                  //enable = 1;
                  //printf("Menu > MENU_CNT\r\n");

                  //return enable;
                  return;		  //종료 합니다. 

		} 
         
		//sprintf((char*)&str, "%02d>", Menu); 
		//TM_HD44780_Puts(0,0, str);
                cReLcdDisplay = 0;                  
                sprintf(buffGLcd1Line, "%02d>", Menu);         
                menu_list();
		TM_HD44780_Puts(0,0, buffGLcd1Line);
//		sprintf(str, "  Value : %03d  ", Config.Value[Menu-1]); //10
//		TM_HD44780_Puts(0,1, str);	
		sprintf(buffGLcd2Line, "  Value : %03d  ", Config.Value[Menu-1]); //10
		TM_HD44780_Puts(0,1, buffGLcd2Line);	
                
//		sprintf(str0, "%02d>", Menu); 
//		sprintf(str1, "  Value : %03d  ", Config.Value[Menu-1]); //10
//                enable = 1;

	} 
        else if(Setting_enable == 2 ) {       
		
		if(SW1 == 1 && menu_key_wait == 0 ) {   
			Setting_delay = 2;
			menu_key_wait = 1; 
                        SW_L[0] = 0; 
                        SW_S[0] = 0; 	
                        //printf("S2 SW1 == 1 && menu_key_wait == 0\r\n");                        
		} 
                else	if(SW_S[0] != 0 && menu_key_wait == 1){                 
			SW_S[0] = 0;                                                           
			SW_L[0] = 0;
			menu_key_wait = 0;
			Setting_enable = 1;                                                   
			save_all_eeprom();
			
			Setting_delay = 2;
			
			//해당값을 초기화 안해 놓으면 다음번에 메뉴가 바뀜
			SW_S[1] = 0; 
                        SW_L[1] = 0;
			SW_S[2] = 0; 
                        SW_L[2] = 0;
		
			Menu ++;                                                           
			if(Menu > MENU_CNT) {             //s menu 빠져 나옴            
			  Setting_enable = 0; 
			  Menu = 0;        
                          cReLcdDisplay = 0;                    
                  
                          MeunTitleCopy();
                          sprintf(buffGLcd2Line, "                "); 
                          //TM_HD44780_Puts(0,1,buffGLcd2Line);   
                          cRelaySendState = RELAY_DATA_SEND;                         
                          //printf("S2   Menu > MENU_CNT\r\n");                            
			  //dp_clear();			                                
                          //return enable;
                          return;                
			}
                        //printf("S2 SW_S[0] != 0 && menu_key_wait == 1\r\n");   	
		} 
                else {			
				
			if(SW2 == 1) //떨어졌으면 !
			{
				SW_L[1] = 0;	//길게 초기화
				SW_Cnt[1] = 0;
                                //printf("S2 SW2 == 1\r\n");   	                               
			} 
                        else {
                          uint8_t menutemp = Menu-1;
                          
				if(SW_L[1] != 0) {		//길게 눌렸으면 !!     
					SW_S[1] = 0;          // 짧은 초기화
					if(sw_long_add ++ > 2) {
						sw_long_add = 0;
						if(Config.Value[menutemp] < (MAX_Value[menutemp] + SW_LONG_ADD)){   
                                                //if(Config.Value[Menu-1] < tempdata){                                                   
							Config.Value[menutemp]+=SW_LONG_ADD;
                                                        //printf("S2 Config.Value[Menu-1] < (MAX_Value[Menu-1] + SW_LONG_ADD\r\n");       
                                                        //changed = 1;
                                                }		
						if(Config.Value[menutemp] >= MAX_Value[menutemp])   
						{
							Config.Value[menutemp] = MAX_Value[menutemp] ;
                                                        //printf("S2 Config.Value[Menu-1] >= MAX_Value[Menu-1]\r\n");                
                                                        //changed = 1;
						}
					}
                                        //printf("S2 SW_L[1] != 0  \r\n");   
				} 
                                else if(SW_S[1] != 0) {	
					SW_S[1]  = 0;
					if(Config.Value[menutemp] < MAX_Value[menutemp]){                   
                                            Config.Value[menutemp]++;
                                            //printf("S2 Config.Value[Menu-1] < MAX_Value[Menu-1]\r\n");         
                                            //changed = 1;          
                                        }
                                        //printf("S2 (SW_S[1] != 0\r\n");                                                  
				}
                                //printf("S2 else SW2 == 1   [%d]  [%d]\r\n", SW_Cnt[1], SW_L[1]);   

			}
			
				
			if(SW3 == 1)                                        
			{
				SW_L[2] = 0;	
				SW_Cnt[2] = 0;
                                //printf("S2 SW3 == 1\r\n");                                   
			} 
                        else {
				if(SW_L[2] != 0) {		 
					SW_S[2] = 0;            
					if(sw_long_add ++ > 2) {           
						sw_long_add = 0;               
						if(Config.Value[Menu-1] >= SW_LONG_ADD){   
                                                  Config.Value[Menu-1]-=SW_LONG_ADD;	  
                                                  //changed = 1;
                                                  //printf("S2 Config.Value[Menu-1] >= SW_LONG_ADD\r\n");                                                     
                                                }
                                                //printf("S2 sw_long_add ++ > 2\r\n");                                                  
					}
                                        //printf("S2 SW_L[2] != 0\r\n");    	
				} 
                                else if(SW_S[2] != 0) {	       //s 카운트 다운              
					SW_S[2] = 0;                   
					if(Config.Value[Menu-1] !=0){               
                                          Config.Value[Menu-1]--;	   
                                          //changed = 1;
                                            //printf("S2 Config.Value[Menu-1] !=0\r\n");                                                  
                                        }
                                        //printf("S2 (SW_S[1] != 0\r\n");                                                       
				}
                                //printf("S2 else SW3 == 1\r\n");
			}

                         cReLcdDisplay = 0;  

                        if(Menu == 2){
                                Device_list(Config.Value[Menu-1]);
                        }
                        else if(Menu == 4){
                          Rf_list(Config.Value[Menu-1]);
                        }
                        else{
                            sprintf(buffGLcd1Line, " Set Value:%02d   ", Menu); //          21
                            TM_HD44780_Puts(0,0, buffGLcd1Line);	
                        }

//			sprintf(str, "  Value : %03d   ", Config.Value[Menu-1]); 
//			TM_HD44780_Puts(0,1, str);
                        sprintf(buffGLcd2Line, "  Value : %03d   ", Config.Value[Menu-1]); //21
                        TM_HD44780_Puts(0,1, buffGLcd2Line);	                        
     
//                        if(changed){
//                        sprintf(str0, " Set Value:%02d   ", Menu);                             
//                        sprintf(str1, "  Value : %03d   ", Config.Value[Menu-1]);     
//                        enable = 1;
//                        }

		}

	}
        //return enable;

}


//#define COW_NUM	volatile uint8_t Menu = 0;  
void menu_list()
{	
	//TM_HD44780_Puts(3,1, "         ");
	 switch(Menu-1)
	 {
	 case 0: 
            sprintf(&buffGLcd1Line[3], "Farm Code    ");            
           //TM_HD44780_Puts(3,0, "Farm Code    "); 
           break;
	 case 1: 
           sprintf(&buffGLcd1Line[3], "Device Type  "); 
           //TM_HD44780_Puts(3,0, "Device Type  "); 
           break;
	 case 2: 
           sprintf(&buffGLcd1Line[3], "RF Address   "); 
           //TM_HD44780_Puts(3,0, "RF Address   "); 
           break;	
	 case 3: 
           sprintf(&buffGLcd1Line[3], "RF Type      "); 
           //TM_HD44780_Puts(3,0, "RF Type      "); 
           break;
	 case 4: 
           sprintf(&buffGLcd1Line[3], "Out1 Second  "); 
           //TM_HD44780_Puts(3,0, "Out1 Second  "); 
           break;
	 case 5: 
           sprintf(&buffGLcd1Line[3], "Out2 Second  "); 
           //TM_HD44780_Puts(3,0, "Out2 Second  ");
           break;
	 case 6: 
           sprintf(&buffGLcd1Line[3], "Out3 Second  "); 
           //TM_HD44780_Puts(3,0, "Out3 Second  "); 
           break;
	 case 7: 
           sprintf(&buffGLcd1Line[3], "Out4 Second  "); 
           //TM_HD44780_Puts(3,0, "Out4 Second  "); 
           break;
	 case 8: 
           sprintf(&buffGLcd1Line[3], "Out5 Second  "); 
           //TM_HD44780_Puts(3,0, "Out1 Second  "); 
           break;
	 case 9: 
           sprintf(&buffGLcd1Line[3], "Out6 Second  "); 
           //TM_HD44780_Puts(3,0, "Out2 Second  ");
           break;
	 case 10: 
           sprintf(&buffGLcd1Line[3], "Out7 Second  "); 
           //TM_HD44780_Puts(3,0, "Out3 Second  "); 
           break;
	 case 11: 
           sprintf(&buffGLcd1Line[3], "Out8 Second  "); 
           //TM_HD44780_Puts(3,0, "Out4 Second  "); 
           break;
	 case 12: 
           sprintf(&buffGLcd1Line[3], "Out9 Second  "); 
           //TM_HD44780_Puts(3,0, "Out1 Second  "); 
           break;
	 case 13: 
           sprintf(&buffGLcd1Line[3], "Out10 Second "); 
           //TM_HD44780_Puts(3,0, "Out2 Second  ");
           break;
	 case 14: 
           sprintf(&buffGLcd1Line[3], "Out11 Second "); 
           //TM_HD44780_Puts(3,0, "Out3 Second  "); 
           break;
	 case 15: 
           sprintf(&buffGLcd1Line[3], "Out12 Second "); 
           //TM_HD44780_Puts(3,0, "Out4 Second  "); 
           break;
	 case 16: 
           sprintf(&buffGLcd1Line[3], "Out13 Second "); 
           //TM_HD44780_Puts(3,0, "Out1 Second  "); 
           break;
	 case 17: 
           sprintf(&buffGLcd1Line[3], "Out14 Second "); 
           //TM_HD44780_Puts(3,0, "Out2 Second  ");
           break;
	 case 18: 
           sprintf(&buffGLcd1Line[3], "Out15 Second "); 
           //TM_HD44780_Puts(3,0, "Out3 Second  "); 
           break;
	 case 19: 
           sprintf(&buffGLcd1Line[3], "Out16 Second "); 
           //TM_HD44780_Puts(3,0, "Out4 Second  "); 
           break;
	 case 20: 
           sprintf(&buffGLcd1Line[3], "Moving Count  "); 
           //TM_HD44780_Puts(3,0, "Not use      "); 
           break;	
	 case 21: 
           sprintf(&buffGLcd1Line[3], "8CH Reverse "); 
           //TM_HD44780_Puts(3,0, "Not use      "); 
           break;	           
	 case 22: 
           sprintf(&buffGLcd1Line[3], "Reset Enable "); 
           //TM_HD44780_Puts(3,0, "Not use      "); 
           break;	          
	 case 23: 
           sprintf(&buffGLcd1Line[3], "RF AUTO ADDR1"); 
           //TM_HD44780_Puts(3,0, "Not use      "); 
           break;	           
	 case 24: 
           sprintf(&buffGLcd1Line[3], "RF AUTO ADDR2"); 
           //TM_HD44780_Puts(3,0, "Not use      "); 
           break;	
           
	 default: 
           sprintf(&buffGLcd1Line[3], "###########  "); 
           //TM_HD44780_Puts(3,0, "###########  "); 
         break;
	 }
}



void Device_list(uint8_t subMenu)
{
    //TM_HD44780_Puts(3,1, "         ");
    switch(subMenu) {
        case 0:
            TM_HD44780_Puts(0,0, "  Not use       ");
            break;			
        case 1:
            TM_HD44780_Puts(0,0, "  16CH          ");
            break;
        case 2:
            TM_HD44780_Puts(0,0, "  8CH           ");
            break;
        case 3:
            TM_HD44780_Puts(0,0, "  PANDING       ");
            break;
        case 4:
            TM_HD44780_Puts(0,0, "  CHICK         ");
            break;
        case 5:
            TM_HD44780_Puts(0,0, "  PANDING 8CH   ");
            break;
        case 6:
            TM_HD44780_Puts(0,0, "  8CH LIMIT     ");
            break;            
        default:
            TM_HD44780_Puts(0,0, "###########  ");
            break;
    }
}

void Rf_list(uint8_t subMenu)
{
    //TM_HD44780_Puts(3,1, "         ");
    switch(subMenu) {
        case 0:
            TM_HD44780_Puts(0,0, "  AUTO3025M     ");
            break;			
        case 1:
            TM_HD44780_Puts(0,0, "  AUTO3007B     ");
            break;
        default:
            TM_HD44780_Puts(0,0, "###########  ");
            break;
    }
}


/*


#define CHANNELwaiting_time			Config.Value[8] 
#define CHANNELwaiting_time_MAX	9000

#define CON_Water_eat					Config.Value[9] 
#define CON_Water_eat_MAX			1
*/











