#include "hw_control.h"

volatile uint8_t Setting_enable = 0;    
volatile uint8_t SW_S[3] = {0,};	
volatile uint8_t SW_L[3] = {0,};	
volatile uint8_t SW_Push[3] = {0,}; 
volatile uint8_t SW_Cnt[3] = {0,}; 	

#define SW_LONG_CNT 100	

void view_test(void);

void SW_check(void)
{
	if(SW1==0) {
		if(SW_L[0]==0) {	
			if(SW_Cnt[0]++ >= SW_LONG_CNT)	{ 
				SW_Push[0] = 0;	
				SW_Cnt[0] = 0;		
				SW_L[0] = 1;
				
			} else {
				SW_Push[0] = 1; 
			}
		} else {
			SW_Push[0] = 1; 
		}
	} else {
		if(SW_Push[0] == 1) {
			SW_Push[0] = 0; //����
			//SW_S[0] = 1;	
			SW_S[0] ++;	
		}
		SW_Cnt[0] = 0;	
	}
		
	if(SW2==0) {
		if(SW_L[1] == 0) {
			if(SW_Cnt[1]++>= SW_LONG_CNT) {
				SW_Push[1] = 0;
				SW_Cnt[1] = 0;
				SW_L[1] = 1;
			} else {
				SW_Push[1] = 1;
			}
		} else {
			SW_Push[1] = 1;
		}
		
	} else {
		if(SW_Push[1] == 1) {
			SW_Push[1] = 0;
			SW_S[1] ++ ;
		}
		SW_Cnt[1] =0;
	} 
	
	if(SW3==0) {
		if(SW_L[2] == 0) {
			if(SW_Cnt[2]++>= SW_LONG_CNT) {
				SW_Push[2] = 0;
				SW_Cnt[2] = 0;
				SW_L[2] = 1;
			} else {
				SW_Push[2] = 1;
			}
		} else {
			SW_Push[2] = 1;
		}
		
	} else {
		if(SW_Push[2] == 1) {
			SW_Push[2] = 0;
			//SW_S[2] =1 ;
			SW_S[2] ++ ;
		}
		SW_Cnt[2] =0;
		
	}
	
	
}

#define	MENU_CNT		MAX_EEPROM_ADDRESS          

volatile uint8_t menu_key_wait=0;  
volatile uint8_t main_key_wait=0;
volatile uint8_t Menu = 0;  
volatile uint8_t setValue = 0;  


volatile uint16_t MAX_Value[MAX_EEPROM_ADDRESS]  = { 		//11
												CON_ADDRESS_MAX, 
												CON_WATER_WAIT_DIFF_MAX, 
												CON_WATER_WAIT_TIME_MAX, 
												9999,
												9999,
												COM_RE_ENTER_CNT_MAX,
												9999,
												9999,
												9999,
												9999
												
												};      

volatile uint16_t MIN_Value[MAX_EEPROM_ADDRESS] = {1,0,0,0,0,0,0,0,0,0,0};
volatile uint16_t Setting_delay = 0;

extern char buf_menu_num[16]; 
extern volatile uint16_t FET_value[10];

#define SW_LONG_ADD		50

char str[100];
void menu_list();
extern volatile uint8_t SW_mode_flag;

void setting_mode(void)
{
	static uint16_t sw_long_add=0;
	char str[100];
	
	if(Setting_delay !=0) return;
	
	if(Setting_enable == 1) {

		//�������� �Ѿ�°� ����
		if(SW1 == 1 && menu_key_wait == 0 ) 
		{   //������ư�� ��Դ�������   	
			Delay(300);				  
			menu_key_wait = 1; SW_L[0] = 0; SW_S[0] = 0;

		}
		else if(SW_L[0] != 0 && menu_key_wait == 1)
		{
			//���� ��Ż 
			SW_L[0] = 0;                                            
			Setting_enable = 0;                                                                   
			menu_key_wait = 0;
			main_key_wait = 0;          
			Menu = 0;
			Setting_delay = 20;
			
			return;
		} 
		
		else if(SW_S[0] != 0 && menu_key_wait == 1)                 
		{
			SW_S[0] = 0;                                                        
			
			Setting_enable = 2;
			menu_key_wait = 0;			
			Setting_delay = 5;			
			return;
		}
	
		if(SW_S[1] != 0 || SW_L[1] != 0) { 
			Menu++; SW_S[1] = 0; SW_L[1] = 0; }  
		
		else if(SW_S[2] != 0 || SW_L[2] != 0) { 
			Menu--; SW_S[2] = 0; SW_L[2] = 0; 
		}                      

		if(Menu <= 1) Menu = 1;                                                                                                  
		else if(Menu > MENU_CNT) {                                                                                  
		  Setting_enable = 0;
		  Menu = 0;
		  
		  main_key_wait = 0;
		  SW_L[0] = 0;
		  SW_S[0] = 0;

		  
		  return;		//���� �մϴ�. 

		} 

		sprintf((char*)&str, "%02d>", Menu); 
		TM_HD44780_Puts(0,0, str);
		menu_list();

		sprintf(str, "  Value : %03d  ", Config.Value[Menu-1]); //10
		TM_HD44780_Puts(0,1, str);	

	} else if(Setting_enable == 2 ) {       
		
		if(SW1 == 1 && menu_key_wait == 0 ) {   
			
			Setting_delay = 2;
			menu_key_wait = 1; SW_L[0] = 0; SW_S[0] = 0; 	
		} else	if(SW_S[0] != 0 && menu_key_wait == 1){                 
			SW_S[0] = 0;                                                           
			SW_L[0] = 0;
			menu_key_wait = 0;
			Setting_enable = 1;                                                   
			save_all_eeprom();
			
			Setting_delay = 2;
			
			//�ش簪�� �ʱ�ȭ ���� ������ �������� �޴��� �ٲ�
			SW_S[1] = 0; SW_L[1] = 0;
			SW_S[2] = 0; SW_L[2] = 0;
			
			Menu ++;                                                           
			if(Menu > MENU_CNT) {             
			  Setting_enable = 0; 
			  Menu = 0;                                                        
			  //dp_clear();			                                
			  return;
			}
		} else {			
				
			if(SW2 == 1) //���������� !
			{
				SW_L[1] = 0;	//���ʱ�ȭ
				SW_Cnt[1] = 0;   
			} else {
				if(SW_L[1] != 0) {		//��� �������� !!     
					SW_S[1] = 0;          // ª �ʱ�ȭ
					if(sw_long_add ++ > 2) {
						sw_long_add = 0;                 
						if(Config.Value[Menu-1] < (MAX_Value[Menu-1] + SW_LONG_ADD))   
							Config.Value[Menu-1]+=SW_LONG_ADD;
						
						if(Config.Value[Menu-1] >= MAX_Value[Menu-1])   
						{
							Config.Value[Menu-1] = MAX_Value[Menu-1] ;
						}
					}
				} else if(SW_S[1] != 0) {	
					SW_S[1]  = 0;
					if(Config.Value[Menu-1] < MAX_Value[Menu-1])                   
							Config.Value[Menu-1]++;                                      
				}

			}
			
				
			if(SW3 == 1)                                        
			{
				SW_L[2] = 0;	
				SW_Cnt[2] = 0;  
			} else {
				if(SW_L[2] != 0) {		 
					SW_S[2] = 0;            
					if(sw_long_add ++ > 2) {           
						sw_long_add = 0;               
						if(Config.Value[Menu-1] >= SW_LONG_ADD)   
						Config.Value[Menu-1]-=SW_LONG_ADD;	     
					}
					
				} else if(SW_S[2] != 0) {	                     
					SW_S[2] = 0;                                   
					if(Config.Value[Menu-1] !=0)               
					Config.Value[Menu-1]--;	               
				}

			}
			sprintf(str, " Set Value:%02d   ", Menu); 
			TM_HD44780_Puts(0,0, str);
			
			sprintf(str, "  Value : %03d   ", Config.Value[Menu-1]); 
			TM_HD44780_Puts(0,1, str);

		}

	}

}


//#define COW_NUM	volatile uint8_t Menu = 0;  
void menu_list()
{	
	//TM_HD44780_Puts(3,1, "         ");
	 switch(Menu-1)
	 {
	 case 0: TM_HD44780_Puts(3,0, "Address      "); break;
	 case 1: TM_HD44780_Puts(3,0, "WaterDiff    "); break;
	 case 2: TM_HD44780_Puts(3,0, "WaterDiffT   "); break;	
	 case 3: TM_HD44780_Puts(3,0, "Not use       "); break;
	 case 4: TM_HD44780_Puts(3,0, "Not use       "); break;
	 case 5: TM_HD44780_Puts(3,0, "Re_Enter_T   "); break;
	 case 6: TM_HD44780_Puts(3,0, "Not use       "); break;
	 case 7: TM_HD44780_Puts(3,0, "Not use       "); break;
	 case 8: TM_HD44780_Puts(3,0, "Not use       "); break;
	 case 9: TM_HD44780_Puts(3,0, "Not use       "); break;	
	 case 10: TM_HD44780_Puts(3,0, "Not use       "); break;	
	 default: TM_HD44780_Puts(3,0, "###########  "); break;
	 }
}



/*


#define CHANNELwaiting_time			Config.Value[8] 
#define CHANNELwaiting_time_MAX	9000

#define CON_Water_eat					Config.Value[9] 
#define CON_Water_eat_MAX			1
*/











