/**
  ******************************************************************************
  * @file    USB_Device/DFU_Standalone/Src/main.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    17-February-2017
  * @brief   USB device DFU demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef USBD_Device;
int nFWUpdatePending = 0;
void gotoNext();

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
uint32_t HAL_GetElapsedTick(__IO uint32_t baseTick);

#ifdef USE_WATCHDOG
__IO uint32_t uwLsiFreq = 0;
static uint32_t GetLSIFrequency(void);
#endif
/* Private functions ---------------------------------------------------------*/
void Error_Handler(void)
{
  /* Turn LED3 on */
	//BSP_LED_On(LED3);
	while (1)
	{
	}
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	USBD_DFU_HandleTypeDef   *hdfu;

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

	BSP_LED_Init(LED1);
	BSP_LED_On(LED1);
	
  /* Configure Key Button */
	BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_GPIO);
  
	nFWUpdatePending = isFWUpdatePending();
	updateBootVerNum();
	
  /* Check if the KEY Button is pressed */
	if (BSP_PB_GetState(BUTTON_WAKEUP) != 0x00	// function key is not pressed
		&& !nFWUpdatePending				// FW updated is not pended
		&& ((*(__IO uint32_t*)USBD_DFU_APP_DEFAULT_ADD) & 0x2FFE0000 ) == 0x20020000 )
    {
	    GotoApplication();
    }
  
  /* Otherwise enters DFU mode to allow user programing his application */
  /* Init Device Library */
  USBD_Init(&USBD_Device, &DFU_Desc, 0);
  
  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_DFU_CLASS);
  
  /* Add DFU Media interface */
  USBD_DFU_RegisterMedia(&USBD_Device, &USBD_DFU_Flash_fops);

  /* Start Device Process */
  USBD_Start(&USBD_Device);

#ifdef USE_WATCHDOG
  /*##-2- Get the LSI frequency: TIM5 is used to measure the LSI frequency ###*/
	uwLsiFreq = GetLSIFrequency();
#endif
	uint32_t baseTick = HAL_GetTick();
	while (!USBD_Device.pClassData && HAL_GetElapsedTick(baseTick) < 5000);
	
	if (HAL_GetElapsedTick(baseTick) >= 5000 
		&& ((*(__IO uint32_t*)USBD_DFU_APP_DEFAULT_ADD) & 0x2FFE0000) == 0x20020000)
	{
		/* Disconnect the USB device */
		USBD_Stop(&USBD_Device);

		/* DeInitilialize the MAL(Media Access Layer) */
		((USBD_DFU_MediaTypeDef *)USBD_Device.pUserData)->DeInit();

		GotoApplication();
	}
	
  /* Run Application (Interrupt mode) */
  while (1)
  {
	  hdfu = (USBD_DFU_HandleTypeDef*) USBD_Device.pClassData;
    	//TODO : You should modify the below 2 files for STM32 device driver
		//TODO : stm32f4xx_II_usb.c
		//HAL_StatusTypeDef  USB_DevDisconnect (USB_OTG_GlobalTypeDef *USBx)
		//{
		//	  USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;
		//	  //HAL_Delay(3U);
		//	  uint64_t timeout = 1000000;
		//	  while (timeout-- > 0);
		//	  return HAL_OK;  
		//}
		//TODO : usbd_dfu.c
		//static uint8_t  USBD_DFU_DeInit(USBD_HandleTypeDef *pdev, 
		//		uint8_t cfgidx)
		//{
		//..........
		//	USBD_free(pdev->pClassData);
		//	//pdev->pClassData = NULL;
		//} 
		//.............
		//void DFU_Leave(USBD_HandleTypeDef *pdev)
		//{
		//	  uint8_t bk_state, bk_manif;
		//	  ....
		//	  hdfu->manif_state = DFU_MANIFEST_COMPLETE;
		//	  bk_manif = hdfu->manif_state;
		//	  if ((USBD_DFU_CfgDesc[(11 + (9 * USBD_DFU_MAX_ITF_NUM))]) & 0x04)
		//	  {
		//		  hdfu->dev_state = DFU_STATE_MANIFEST_SYNC;
		//		  bk_state = hdfu->dev_state;
		//		  .....
		//		  return;
		//	  }
		//	  else
		//	  {
		//		  hdfu->dev_state = DFU_STATE_MANIFEST_WAIT_RESET;
		//		  bk_state = hdfu->dev_state;
		//        .....
		//		  /* DeInitilialize the MAL(Media Access Layer) */
		//		  ((USBD_DFU_MediaTypeDef *)pdev->pUserData)->DeInit();
		//		#if 0    
		//		  /* Generate system reset to allow jumping to the user code */
		//		  NVIC_SystemReset();
		//
		//		  		/* This instruction will not be reached (system reset) */
		//		  for (;;);
		//		#endif   
		//		  hdfu->manif_state = bk_manif;
		//		  hdfu->dev_state = bk_state;
		//	  }  
	  	//}  
	  if (hdfu && (hdfu->manif_state != DFU_MANIFEST_COMPLETE
			|| (hdfu->dev_state != DFU_STATE_MANIFEST_SYNC && hdfu->dev_state != DFU_STATE_MANIFEST_WAIT_RESET)))
			continue;

		/* Disconnect the USB device */
		USBD_Stop(&USBD_Device);

		/* DeInitilialize the MAL(Media Access Layer) */
		((USBD_DFU_MediaTypeDef *)USBD_Device.pUserData)->DeInit();

	   gotoNext();
  }
}

void gotoNext()
{
	if (nFWUpdatePending)
		clearFWUpdatePending();

	if (((*(__IO uint32_t*)USBD_DFU_APP_DEFAULT_ADD) & 0x2FFE0000) == 0x20020000)
		GotoApplication();
	else
		BSP_LED_Off(LED1);
}
void GotoApplication()
{
	pFunction JumpToApplication;
	uint32_t JumpAddress;

    /* Jump to user application */
	JumpAddress = *(__IO uint32_t*)(USBD_DFU_APP_DEFAULT_ADD + 4);
	JumpToApplication = (pFunction) JumpAddress;
      
	/* Initialize user application's Stack Pointer */
	__set_MSP(*(__IO uint32_t*) USBD_DFU_APP_DEFAULT_ADD);
	JumpToApplication();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;   

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Configure RCC Oscillators: All parameters can be changed according to user’s needs */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* RCC Clocks: All parameters can be changed according to user’s needs */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_HCLK |RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  
  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

#ifdef USE_WATCHDOG
static uint32_t GetLSIFrequency(void)
{
	TIM_HandleTypeDef  TimInputCaptureHandle;

	__IO uint32_t uwCaptureNumber = 0;
	__IO uint32_t uwPeriodValue = 0;
	__IO uint32_t uwMeasurementDone = 0;

	uint32_t pclk1 = 0;
	TIM_IC_InitTypeDef timinputconfig;  
  
	/* Enable the LSI oscillator */
	__HAL_RCC_LSI_ENABLE();
  
	/* Wait till LSI is ready */
	while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
	{
	}
  
	/* Configure the TIM peripheral */ 
	/* Set TIMx instance */  
	TimInputCaptureHandle.Instance = TIM5;
  
	/* TIM5 configuration: Input Capture mode ---------------------
	The LSI oscillator is connected to TIM5 CH4.
	The Rising edge is used as active edge.
	The TIM5 CCR4 is used to compute the frequency value. 
	------------------------------------------------------------ */
	TimInputCaptureHandle.Init.Prescaler         = 0; 
	TimInputCaptureHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;  
	TimInputCaptureHandle.Init.Period            = 0xFFFF; 
	TimInputCaptureHandle.Init.ClockDivision     = 0;     
	TimInputCaptureHandle.Init.RepetitionCounter = 0; 
	if (HAL_TIM_IC_Init(&TimInputCaptureHandle) != HAL_OK)
	{
	  /* Initialization Error */
		Error_Handler();
	}
	/* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
	HAL_TIMEx_RemapConfig(&TimInputCaptureHandle, TIM_TIM5_LSI);
  
	/* Configure the Input Capture of channel 4 */
	timinputconfig.ICPolarity  = TIM_ICPOLARITY_RISING;
	timinputconfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	timinputconfig.ICPrescaler = TIM_ICPSC_DIV8;
	timinputconfig.ICFilter    = 0;
  
	if (HAL_TIM_IC_ConfigChannel(&TimInputCaptureHandle, &timinputconfig, TIM_CHANNEL_4) != HAL_OK)
	{
	  /* Initialization Error */
		Error_Handler();
	}
  
	/* Reset the flags */
	TimInputCaptureHandle.Instance->SR = 0;
  
	/* Start the TIM Input Capture measurement in interrupt mode */
	if (HAL_TIM_IC_Start_IT(&TimInputCaptureHandle, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}
  
	/* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in 
	stm32f4xx_it.c file) */
	while (uwMeasurementDone == 0)
	{
	}
	uwCaptureNumber = 0;
  
	/* Deinitialize the TIM5 peripheral registers to their default reset values */
	HAL_TIM_IC_DeInit(&TimInputCaptureHandle);
  
	/* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
	/* Get PCLK1 frequency */
	pclk1 = HAL_RCC_GetPCLK1Freq();
  
	/* Get PCLK1 prescaler */
	if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
	{ 
	  /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
		return ((pclk1 / uwPeriodValue) * 8);
	}
	else
	{ /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
		return (((2 * pclk1) / uwPeriodValue) * 8);
	}
}

extern void ResetSystemByWDG()
{
	IWDG_HandleTypeDef   IwdgHandle;
  
	/*##-3- Configure & Start the IWDG peripheral #########################################*/
	/* Set counter reload value to obtain 1 sec. IWDG TimeOut.
	   IWDG counter clock Frequency = uwLsiFreq
	   Set Prescaler to 32 (IWDG_PRESCALER_32)
	   Timeout Period = (Reload Counter Value * 32) / uwLsiFreq
	   So Set Reload Counter Value = (1 * uwLsiFreq) / 32 */
	IwdgHandle.Instance = IWDG;

	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
	IwdgHandle.Init.Reload    = uwLsiFreq / 128;

	if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
	{
	}
}
#endif
uint32_t HAL_GetElapsedTick(__IO uint32_t baseTick)
{
	uint32_t curTick = HAL_GetTick();
	if (curTick < baseTick)
		return (0xffffffff - baseTick + curTick + 1);
	else
		return curTick - baseTick;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
