#include "hw_control.h"

RCC_ClocksTypeDef clocks;

volatile uint32_t sysTickUptime = 0;
volatile uint32_t Tflag_1ms = 0;
#define ADC1_DR_Address    ((u32)0x4001244C)
uint16_t ADCConvertedValue;

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
    } 
    else 
    {
        tick_10ms--;		
    }	

    if(tick_100ms == 0)
    {	
        tick_100ms = 99;
        timer_100ms_loop();
    }
    else
    {
        tick_100ms--;
    }   
    
    if(tick_1000ms == 0) 
    {	
        tick_1000ms = 999;
        timer_1000ms_loop();
    } 
    else
    {
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
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

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
    
    //Lora reset
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    LORA_SET;
      
    FET1_OFF;
    FET2_OFF;
    FET3_OFF;
    FET4_OFF;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;		
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    
    FET5_OFF;
    
    ADC_Config();
		
}

void ADC_Config(void)
{      
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    
        /* ADC1 regular channel14 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Enable ADC1 reset calibration register */   
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
       
    /* Start ADC1 Software Conversion */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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