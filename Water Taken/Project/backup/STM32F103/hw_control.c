#include "hw_control.h"

RCC_ClocksTypeDef clocks;

volatile uint32_t sysTickUptime = 0;
volatile uint32_t Tflag_1ms = 0;

void SysTick_Handler(void)         
{
	sysTickUptime++;
	Tflag_1ms++;
	timer_check();
}

void cycleCounterInit(void)
{
	RCC_GetClocksFreq(&clocks);
	SysTick_Config(clocks.SYSCLK_Frequency / 1000);	
}

uint32_t micros(void)
{
    register uint32_t ms, cycle_cnt;
    do {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    } while (ms != sysTickUptime);
    return (ms * 1000) + (72000 - cycle_cnt) / 72;
}

uint32_t millis(void)
{
    return sysTickUptime;
}

void Delay(uint32_t ms)
{
	 uint32_t now = Tflag_1ms;
     while ((Tflag_1ms - now) < ms);
}

static void delay_t(uint32_t t)
{
	while(t--)
	{
		asm volatile ("nop");
		asm volatile ("nop");
	}
}

void delay_us(uint32_t t)
{
	while(t--) delay_t(11);
}

void delay_ms(uint32_t xx)
{
	while(xx--) delay_us(1000);
}

extern void timer_1000ms_loop(void);
extern void timer_100ms_loop(void);
extern void timer_10ms_loop(void);


void timer_check(void)
{
	//loop = 1ms
	static uint16_t tick_10ms = 10,  tick_100ms=100, tick_1000ms=1000;

	if(tick_10ms == 0) //10mS
	{		
		tick_10ms = 9;
		timer_10ms_loop();
	} else {
		tick_10ms--;		
	}	

	if(tick_100ms == 0)
	{	
		tick_100ms = 99;
		timer_100ms_loop();
	} else {
		tick_100ms--;
	}   
	
	if(tick_1000ms == 0) 
	{	
		tick_1000ms = 999;
		timer_1000ms_loop();
	} else {
		tick_1000ms--;
	}  
}

void NVIC_Configuration(void)               
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
}

void GPIO_Configuration(void)  
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	//GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
	//LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	LED_OFF;
	LED1_OFF;

	//SW
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5 ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//INPUT 5:sensor/6:input1,7:input2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//GPIO OUTPUT-A(FET)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIO OUTPUT-B(FET)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);

		
	FET1_OFF;
	FET2_OFF;
	FET3_OFF;
	FET4_OFF;
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;		
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
	FET5_OFF;
		
}


void setup()
{
	cycleCounterInit();
	RTC_Init();
	GPIO_Configuration();
	LCD_init();
	EXTI9_0_Config();		
	NVIC_Configuration();
	UART_init();	
	lll_eeprom_init();		//EEPROM
	SPI_FLASH_Init();		//MRAM

}