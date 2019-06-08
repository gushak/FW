#ifndef PLL_VALUES_H
#define PLL_VALUES_H

#ifdef __cplusplus
extern "C" {
#endif

/************************* RTC PLL Parameters *************************************/
#define RTC_LSI		1

/************************* PLL Parameters *************************************/
#undef HSE_VALUE
#define HSE_VALUE	((uint32_t)16000000)

#define PLL_M      16

typedef struct
{
	uint16_t		n;		//PLL_N
	uint16_t		p;		//PLL_P
	uint16_t		q;		//PLL_Q
	uint32_t		h;		//PLL_H
	uint32_t		pclk1;	//PCLK1
	uint32_t		pclk2;	//PCLK1
} PLL_t;
/* 1. VCO		: PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
/* 2. SYSCLK	: SYSCLK = PLL_VCO / PLL_P */
/* 3. PCLK		: USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
/* 4. HCLK		: The below blocks use HCLK same as SYSCLK, Because SYSCLK is not devided and becomes HCLK.
	*					To control HCLK different from SYSCLK, Please set RCC_ClkInitStruct.AHBCLKDivider to other value rather than RCC_SYSCLK_DIV1.
	*					AHB3 - FSMC
	*					AHB2 - RNG / HASH / CRYP / DCMI / USB_OTG_FS
	*					AHB1 - USB_OTG_HS / DMA2D / ETHERNET_MAC / DMA2 / DMA1 / BKPSRAM / Flash_Interface / RCC / CRC / GPIOA...GPIOK
	*/
/* 5. PCLK1		: APB1 - UART7...8 / DAC / PWR / CAN1...2 / I2C1...3 / UART2...5 / I2S3ext / SPI2...3(I2S2...3) /
	*					I2S2ext / IWDG / WWDB / RTC & BKP / TIM12...14 / TIM2...7 
	*/
/* 6. PCLK2		: APB2 - LCD-TFT / SAI1 / SPI5...6 / TIM9...11 / EXTI / SYSCFG / SPI4 / SPI1 /
	*					SDIO / ADC1...3 / USART6 / USART1 / TIM8 / TIM1
	*/
//#define		PLL_MODE_DEFAULT		336, 2, 7, RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV4, RCC_HCLK_DIV2		//VCO=336MHz, SYSCLK=168MHz, PCLK=48MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
//#define		PLL_MODE_ALTERNATIVE	336, 2, 7, RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV4, RCC_HCLK_DIV2		//VCO=336MHz, SYSCLK=168MHz, PCLK=48MHz, HCLK=168MHz, PCLK1=42MHz, PCLK2=84MHz
//#define	PLL_MODE_DEFAULT	192, 4, 4,	RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1	//VCO=288MHz, SYSCLK=48MHz, PCLK=48MHz, HCLK=48MHz, PCLK1=24MHz, PCLK2=48MHz
#define	PLL_MODE_DEFAULT		288, 4, 6,	RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1	//VCO=288MHz, SYSCLK=72MHz, PCLK=48MHz, HCLK=72MHz, PCLK1=36MHz, PCLK2=72MHz
#define	PLL_MODE_ALTERNATIVE	288, 4, 6,	RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1	//VCO=288MHz, SYSCLK=72MHz, PCLK=48MHz, HCLK=72MHz, PCLK1=36MHz, PCLK2=72MHz
//#define		PLL_MODE_DEFAULT		336, 8, 7, RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1		//VCO=336MHz, SYSCLK=42MHz, PCLK=48MHz, HCLK=42MHz, PCLK1=21MHz, PCLK2=42MHz
//#define		PLL_MODE_DEFAULT		288, 6, 6,	RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1	//VCO=288MHz, SYSCLK=48MHz, PCLK=48MHz, HCLK=48MHz, PCLK1=24MHz, PCLK2=48MHz
//#define 	PLL_MODE_ALTERNATIVE	288, 6, 6,	RCC_CFGR_HPRE_DIV1, RCC_HCLK_DIV2, RCC_HCLK_DIV1	//VCO=288MHz, SYSCLK=48MHz, PCLK=48MHz, HCLK=48MHz, PCLK1=24MHz, PCLK2=48MHz
	
#define PLL_MODE_NORMAL		0	//PLL_MODE_DEFAULT
#define PLL_MODE_SPEEDUP	1	//PLL_MODE_ALTERNATIVE
/******************************************************************************/
/* Select the clock sources (other than HSI) to start with (0=OFF, 1=ON) */
//#define USE_PLL_HSE_EXTC (1) /* Use external clock */
#define USE_PLL_HSE_XTAL (1) /* Use external xtal */

#ifdef __cplusplus
}
#endif

#endif