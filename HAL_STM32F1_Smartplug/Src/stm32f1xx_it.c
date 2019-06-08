/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"

/* USER CODE BEGIN 0 */
#include "smartplug_it.h"
#include "smartplug_hw.h"
#include "smartplug_impl.h"
#include "smartplug_cli.h"
#include "SM_NMS.h"
#include "SM_V9871.h"
#include <assert.h>
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern uint32_t usart2_last_rx_tick;
extern uint32_t usart4_last_rx_tick;
extern NMS_StateMachine_t nms_rx_sm;
extern V9871_StateMachine_t v9871_rx_sm;


/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
      NVIC_SystemReset();
  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
        DSYNCOUT("reboot");
	DSYNCOUT(reason);
	DSYNCOUT("\r\n");
        NVIC_SystemReset();
  /* USER CODE END HardFault_IRQn 0 */
        while (1)
        {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
        }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
        NVIC_SystemReset();
        /* USER CODE END MemoryManagement_IRQn 0 */
        while (1)
        {
          /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
          /* USER CODE END W1_MemoryManagement_IRQn 0 */
        }
        /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
      NVIC_SystemReset();
  /* USER CODE END BusFault_IRQn 0 */
      while (1)
      {
        /* USER CODE BEGIN W1_BusFault_IRQn 0 */
        /* USER CODE END W1_BusFault_IRQn 0 */
      }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
      NVIC_SystemReset();
  /* USER CODE END UsageFault_IRQn 0 */
      while (1)
      {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
      }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
      HAL_IncTick();
      HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */
      sys_tick_inc();
      impl_on_hw_tick();
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
      uint8_t ch = 0;
      uint8_t buffer[4] = {0,};
      
      if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
      {
            __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
            ch = (uint8_t)(huart1.Instance->DR & (uint8_t)0x00FF); 
#if _USE_CLI
            cli_put_input(ch);
#endif  
      }
      
      if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) != RESET)
      {
#if UART1_INTERRUPT_TX
            if(cirq_dequeue(&debug_tx_queue, &ch) == CIRQ_SUCCESS) 
            {
                  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
                  buffer[0] = ch;
                  HAL_UART_Transmit(&huart1, (uint8_t *)buffer, 1, 1000);
                  //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
            }
            else 
            {
                  /* Disable the USART3 Transmit interrupt */
                  __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
            }
        #else
            __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_TXE);
        #endif 
      }
      __HAL_UART_CLEAR_PEFLAG(&huart1);
  /* USER CODE END USART2_IRQn 0 */
      HAL_UART_IRQHandler(&huart1);
}

/**
* @brief This function handles USART2 global interrupt.
*/
void USART2_IRQHandler(void)
{
      /* USER CODE BEGIN USART3_IRQn 0 */
      uint8_t ch = 0;
      uint8_t buffer[4] = {0,};
      
      if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
      {
            uint32_t cur_tick = sys_tick_count();
            __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
            ch = (uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF);
            sm_nms_input(&nms_rx_sm, ch);
            usart2_last_rx_tick = cur_tick;
      }
      if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET)
      {
#if UART2_INTERRUPT_TX
            if(!uart_is_output_redirected())
            {
                  if(cirq_dequeue(&plc_tx_queue, &ch) == CIRQ_SUCCESS) 
                  {
                        buffer[0] = ch;
                        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, 1, 1000);
                  }
                  else 
                  {
                    // Disable the USART1 Transmit interrupt
                        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
                  }
            }
            else
            {
                  __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
            }
#else
      __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TXE);
#endif 
      }
      
      __HAL_UART_CLEAR_PEFLAG(&huart2);
      /* USER CODE END USART3_IRQn 0 */
      HAL_UART_IRQHandler(&huart2);
      /* USER CODE BEGIN USART3_IRQn 1 */
}

/**
* @brief This function handles USART4 global interrupt.
*/