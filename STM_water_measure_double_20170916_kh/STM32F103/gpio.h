#ifndef _GPIO_H_
#define _GPIO_H_

#define FET1_OFF    GPIOA->BRR = GPIO_Pin_6
#define FET1_ON     GPIOA->BSRR = GPIO_Pin_6
#define FET1_FL     GPIOA->ODR ^= GPIO_Pin_6

#define FET2_OFF    GPIOA->BRR = GPIO_Pin_7
#define FET2_ON     GPIOA->BSRR = GPIO_Pin_7
#define FET2_FL     GPIOA->ODR ^= GPIO_Pin_7

#define FET3_OFF    GPIOB->BRR = GPIO_Pin_0
#define FET3_ON    	GPIOB->BSRR = GPIO_Pin_0
#define FET3_FL     GPIOB->ODR ^= GPIO_Pin_0

#define FET4_OFF    GPIOB->BRR = GPIO_Pin_1
#define FET4_ON    	GPIOB->BSRR = GPIO_Pin_1
#define FET4_FL     GPIOB->ODR ^= GPIO_Pin_1

#define FET5_OFF    GPIOC->BRR = GPIO_Pin_3
#define FET5_ON    	GPIOC->BSRR = GPIO_Pin_3
#define FET5_FL     GPIOC->ODR ^= GPIO_Pin_3

#define LORA_RESET  GPIOA->BRR = GPIO_Pin_8
#define LORA_SET    GPIOA->BSRR = GPIO_Pin_8

#define INPUTM		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)    //s 사용안함

#define WCNT0		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define WCNT1		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)

#define LED_ON      GPIOB->BRR = GPIO_Pin_8
#define LED_OFF     GPIOB->BSRR = GPIO_Pin_8
#define LED_FL      GPIOB->ODR ^= GPIO_Pin_8

#define LED1_ON      GPIOB->BRR = GPIO_Pin_9
#define LED1_OFF     GPIOB->BSRR = GPIO_Pin_9
#define LED1_FL      GPIOB->ODR ^= GPIO_Pin_9

#define INPUT1		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)    //s 소입장 적외선 센서
#define INPUT2		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

#define SW1			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)
#define SW2			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)
#define SW3			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)

void EXTI9_0_Config(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);

void Water_pcnt1_clear(void);
uint32_t Water_pcnt1_read(void);

void Water_pcnt2_clear(void);
uint32_t Water_pcnt2_read(void);
#endif	