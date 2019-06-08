#include "hw_control.h"

uint32_t pcnt = 0; //gpio인터럽트 :번호가 있음 pin5번이면 5번인터럽트를 선택해야 한다.
uint32_t pcnt2 = 0;


void EXTI9_0_Config(void) 
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;

    /* Enable GPIOB clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure PB.09 pin as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Enable AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* Connect EXTI9 Line to PB.09 pin */
    
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0 );
    
    /* Connect EXTI9 Line to PB.09 pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

    /* Configure EXTI9 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0 ;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* Configure EXTI9 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line1 ;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        pcnt2++;
        /* Clear the  EXTI line 5 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void) //유량센서 외부인터럽트
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        pcnt++;
        /* Clear the  EXTI line 5 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

uint32_t Water_pcnt1_read(void)
{	
    return pcnt;
}

void Water_pcnt1_clear(void)
{
    pcnt = 0;
}

uint32_t Water_pcnt2_read(void)
{	
    return pcnt2;
}

void Water_pcnt2_clear(void)
{
    pcnt2 = 0;
}


