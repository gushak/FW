//#include "hw_control.h"
#include "menu.h"
#include "lcd_hd44780.h"
#include "eeprom_4seri.h"

volatile uint8_t Setting_enable = 0;    
volatile uint8_t SW_S[3] = {0,};	//s   ª�� SW ���� ���� 
volatile uint8_t SW_L[3] = {0,};	//s   ��� 1�� SW ���� ���� 
volatile uint8_t SW_Push[3] = {0,};     //s ���� ���� üũ
volatile uint8_t SW_Cnt[3] = {0,}; 	//s ��� ���� Ű ī��Ʈ


#define MENU_CNT                MAX_EEPROM_MENU    

volatile uint8_t menu_key_wait=0;  
volatile uint8_t Menu = 0;  
volatile uint8_t setValue = 0;  


volatile uint16_t MAX_Value[MAX_EEPROM_ADDRESS]  = {  9999,           //s Farm Code  
        16,
        9,
        9999,
        9999,
	
        4,
        4,
        8,
        100,
        100,
	
        9999,		//s ���̽ð�
        9999,		//s �̼۽ð�
        9999,
        9999,
        9999,
	
        1440,	//s ���̽ð� 1
        1440,
        1440,
        1440,
        1440,				//s 20

        1440,
        1440,
        1440,
        1440,
        1440,
        
        1440,
        1440,
        1440,
        1440,
        1440,				//s 30

        1440,
        1440,
        1440,
        1440,
        1440,		//s ���̽ð� 20 	
        
        1440,		//s ���� on1 �ð�
        1440,		//s ���� off1 �ð� 
        1440,		//s ���� on2 �ð�
        1440,		//s ���� off2 �ð� 
        1440,		//s ���� on1 �ð�		//s 40
        
        1440,		//s ���� off1 �ð� 				
        1440,		//s ���� on2 �ð�
        1440,		//s ���� off2 �ð� 							
        1440,		//s ���� on3 �ð�
        1440		//s ���� off3 �ð� 		
                                };      


uint16_t MIN_Value[MAX_EEPROM_ADDRESS] = {0,1,1,0,0,0,0,0,1,1,
                                                                        0,0,0,0,0,0,0,0,0,0,
                                                                        0,0,0,0,0,0,0,0,0,0,
                                                                        0,0,0,0,0,0,0,0,0,0,
                                                                        0,0,0,0,0
};

uint16_t ADD_Value[MAX_EEPROM_ADDRESS] = {10,10,10,10,10,10,10,10,10,10,
                                                                        10,10,10,10,10};

volatile uint16_t Setting_delay = 0;

//extern char buf_menu_num[16]; 
//extern volatile uint16_t FET_value[10];

#define SW_LONG_ADD		10


void MeunTitleCopy(void)
{
//  if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_16CH){
//    sprintf(buffGLcd1Line, "IOT RF 16CH     ");                  
//    TM_HD44780_Puts(0,0,buffGLcd1Line);	 
//  }
//  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH){
//    sprintf(buffGLcd1Line, "IOT RF 8CH      ");                  
//    TM_HD44780_Puts(0,0,buffGLcd1Line);	 
//  }
//  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){
//    sprintf(buffGLcd1Line, "IOT RF PENDING  ");                  
//    TM_HD44780_Puts(0,0,buffGLcd1Line);	 
//  }  
//  else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_CONTROL){
//    sprintf(buffGLcd1Line, "IOT RF CHICK      ");                  
//    TM_HD44780_Puts(0,0,buffGLcd1Line);	 
//  }    
//  
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
			SW_Push[0] = 0; //����
			//SW_S[0] = 1;	
			SW_S[0] ++;
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
                                cBuzzerCount = BUZZER_TIME_COUNT;                                 
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
                                cBuzzerCount = BUZZER_TIME_COUNT;                                     
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

		//�������� �Ѿ�°� ����
		if(SW1 == 1 && menu_key_wait == 0 ) 
		{   //������ư�� ��Դ�������   	 
                    HAL_Delay(300);          //s 0.3��
                    menu_key_wait = 1; 
                    SW_L[0] = 0; 
                    SW_S[0] = 0;
		}
		else if(SW_L[0] != 0 && menu_key_wait == 1)             //s �޴� ���¿��� ����Ű  ��� �Է��Ͽ��� ���     
		{
			//���� ��Ż 
			SW_L[0] = 0;                                            
			Setting_enable = 0;                                                                   
			menu_key_wait = 0;
			//main_key_wait = 0;          
			Menu = 0;
			Setting_delay = 20;
                    return;
		} 
		
		else if(SW_S[0] != 0 && menu_key_wait == 1)            //s �޴� ���¿��� ����Ű  ª�� �Է��Ͽ��� ���     
		{
			SW_S[0] = 0;                                                        
			
			Setting_enable = 2;
			menu_key_wait = 0;			
			Setting_delay = 5;
                        return;
		}
	
		if(SW_S[1] != 0 || SW_L[1] != 0) {                      //s     Up Ű �Է�
			Menu++; 
                        SW_S[1] = 0; 
                        SW_L[1] = 0;                 
                }  
		
		else if(SW_S[2] != 0 || SW_L[2] != 0) {                 //s   Down Ű �Է�
			Menu--; 
                        SW_S[2] = 0;
                        SW_L[2] = 0; 
		}                      

		if(Menu <= 1){                   //s  ó�� Menu  ��ġ�� 0�϶� 1�� ǥ��
                    Menu = 1;

                }
		else if(Menu > MENU_CNT) {                       //s   Menu  ��ġ�� 10�� �Ѿ�� �޴����� ��������                                                                              
		  Setting_enable = 0;
		  Menu = 0;

                  menu_key_wait = 0;
		  SW_L[0] = 0;
		  SW_S[0] = 0;

                  cReLcdDisplay = 0;                    
	  
                  MeunTitleCopy();
                  sprintf(buffGLcd1Line, "SET END       ");                        
                  sprintf(buffGLcd2Line, "PLZ RESET       "); 
                  TM_HD44780_Puts_State(0,0, buffGLcd1Line); 	                  
                  TM_HD44780_Puts_State(0,1, buffGLcd2Line); 	
                  return;		  //���� �մϴ�. 

		} 
         
                cReLcdDisplay = 0;                  
                sprintf(buffGLcd1Line, "%02d>", Menu);         
                menu_list();
		TM_HD44780_Puts(0,0, buffGLcd1Line);

                sprintf(buffGLcd2Line, "  Value : %03d  ", Config.Value[Menu-1]); //10
		TM_HD44780_Puts(0,1, buffGLcd2Line);	
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
			
			//�ش簪�� �ʱ�ȭ ���� ������ �������� �޴��� �ٲ�
			SW_S[1] = 0; 
                        SW_L[1] = 0;
			SW_S[2] = 0; 
                        SW_L[2] = 0;
		
			Menu ++;                                                           
			if(Menu > MENU_CNT) {             //s menu ���� ����            
			  Setting_enable = 0; 
			  Menu = 0;        
                          cReLcdDisplay = 0;                    
                  
                          MeunTitleCopy();
                          sprintf(buffGLcd1Line, "                ");                           
                          sprintf(buffGLcd2Line, "                "); 
                          return;                
			}
		} 
                else {			
				
			if(SW2 == 1) //���������� !
			{
				SW_L[1] = 0;	//��� �ʱ�ȭ
				SW_Cnt[1] = 0;                              
			} 
                        else {
                          uint8_t menutemp = Menu-1;
                          
				if(SW_L[1] != 0) {		//��� �������� !!     
					SW_S[1] = 0;          // ª�� �ʱ�ȭ
					if(sw_long_add ++ > 2) {
						sw_long_add = 0;
						if(Config.Value[menutemp] < (MAX_Value[menutemp] + ADD_Value[menutemp])){                                    
							Config.Value[menutemp]+=ADD_Value[menutemp];
                                                }		
						if(Config.Value[menutemp] >= MAX_Value[menutemp])   
						{
							Config.Value[menutemp] = MAX_Value[menutemp] ;
						}
					}
				} 
                                else if(SW_S[1] != 0) {	
					SW_S[1]  = 0;
					if(Config.Value[menutemp] < MAX_Value[menutemp]){                   
                                            Config.Value[menutemp]++;
                                        }                                
				}  

			}
			
				
			if(SW3 == 1)                                        
			{
				SW_L[2] = 0;	
				SW_Cnt[2] = 0;                                 
			} 
                        else {
				if(SW_L[2] != 0) {		 
					SW_S[2] = 0;            
					if(sw_long_add ++ > 2) {           
						sw_long_add = 0;               
						if(Config.Value[Menu-1] >= ADD_Value[Menu-1]){   
                                                  Config.Value[Menu-1]-=ADD_Value[Menu-1];                                                    
                                                }                                                
					}	
				} 
                                else if(SW_S[2] != 0) {	       //s ī��Ʈ �ٿ�              
					SW_S[2] = 0;                   
					if(Config.Value[Menu-1] !=0){               
                                          Config.Value[Menu-1]--;                                            
                                        }                                                       
				}
			}

                        if(Menu == 3){
                                Device_list(Config.Value[Menu-1]);
                        }
                        else{
                            sprintf(buffGLcd1Line, " Set Value:%02d   ", Menu); //          21
                            TM_HD44780_Puts(0,0, buffGLcd1Line);	
                        }

                        cReLcdDisplay = 0;  
                        sprintf(buffGLcd2Line, "  Value : %03d   ", Config.Value[Menu-1]); //21
                        TM_HD44780_Puts(0,1, buffGLcd2Line);	

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
            TM_HD44780_Puts(3,0, "FCODE        ");
            break;
        case 1:
            TM_HD44780_Puts(3,0, "DCODE        ");
            break;
        case 2:
            TM_HD44780_Puts(3,0, "DEVICE TYPE  ");
            break;
        case 3:
            TM_HD44780_Puts(3,0, "PASSWORD1    ");
            break;
        case 4:
            TM_HD44780_Puts(3,0, "PASSWORD2    ");
            break;
        case 5:
            TM_HD44780_Puts(3,0, "Lora S_DCODE ");
            break;
        case 6:
            TM_HD44780_Puts(3,0, "Lora WA_DCODE");
            break;
        case 7:
            TM_HD44780_Puts(3,0, "Lora WE_DCODE");
            break;
        case 8:
            TM_HD44780_Puts(3,0, "AUTO GR(0-3) ");
            break;
        case 9:
            TM_HD44780_Puts(3,0, "AUTO CH(0-24)");
            break;
        case 10:
            TM_HD44780_Puts(3,0, "FEED ON TIME ");
            break;
        case 11:
            TM_HD44780_Puts(3,0, "LIGHT TIME   ");
            break;
        case 12:
            TM_HD44780_Puts(3,0, "MOVING TIME  ");
            break;
        case 13:
            TM_HD44780_Puts(3,0, "Not use       ");
            break;
        case 14:
            TM_HD44780_Puts(3,0, "Not use       ");
            break;					
        default:
            TM_HD44780_Puts(3,0, "###########  ");
            break;
    }
}

void Device_list(uint8_t subMenu)
{
    //TM_HD44780_Puts(3,1, "         ");
    switch(subMenu) {
        case 0:
            TM_HD44780_Puts(0,0, " Not use         ");
            break;			
        case 1:
            TM_HD44780_Puts(0,0, "AUTO 3205M + SEN");
            break;
        case 2:
            TM_HD44780_Puts(0,0, "AUTO 3007B + SEN");
            break;
        case 3:
            TM_HD44780_Puts(0,0, "BEAN DISTANCE   ");
            break;
        case 4:
            TM_HD44780_Puts(0,0, "AUTOMAN+WATER   ");
            break;
        case 5:
            TM_HD44780_Puts(0,0, "SENSOR+WA+WEIT  ");
            break;
        case 6:
            TM_HD44780_Puts(0,0, "AUTO 3205M + CHI");
            break;
        case 7:
            TM_HD44780_Puts(0,0, "AUTO 3007B + CHI");
            break;	
        case 8:
            TM_HD44780_Puts(0,0, "AUTO 3205M + BE ");
            break;
        case 9:
            TM_HD44780_Puts(0,0, "AUTO 3007B + BE ");
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











