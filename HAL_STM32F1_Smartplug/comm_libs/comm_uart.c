#include <string.h>
#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx.h"
#include "comm_uart.h"
#include "comm_sys.h"

#if UART1_INTERRUPT_TX

#define UART1_TX_BUF_SIZE (512)

static uint8_t debug_buf[UART1_TX_BUF_SIZE + 1];

CirQ_t debug_tx_queue;

#endif

#if UART2_INTERRUPT_TX

#define UART2_TX_BUF_SIZE 256

static uint8_t plc_buf[UART2_TX_BUF_SIZE + 1];

CirQ_t plc_tx_queue;
#endif

#if UART4_INTERRUPT_TX

#define UART4_TX_BUF_SIZE 256

static uint8_t psu_buf[UART4_TX_BUF_SIZE +1];

CirQ_t psu_tx_queue;
#endif

static uint8_t _output_redirect = 1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;

static void UART1_sync_putc(uint8_t data);

void  uart1_init(uint32_t baudrate)
{
#if UART1_INTERRUPT_TX
	cirq_init(&debug_tx_queue, debug_buf, sizeof(debug_buf));
#endif
        
        HAL_UART_DeInit(&huart1);
  
        huart1.Instance = USART1;
        huart1.Init.BaudRate = baudrate;
        huart1.Init.WordLength = UART_WORDLENGTH_8B;
        huart1.Init.StopBits = UART_STOPBITS_1;
        huart1.Init.Parity = UART_PARITY_NONE;
        huart1.Init.Mode = UART_MODE_TX_RX;
        huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart1.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart1) != HAL_OK)
        {
           _Error_Handler(__FILE__, __LINE__);
        }
        
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void  uart2_init(uint32_t baudrate)
{
#if UART2_INTERRUPT_TX
	cirq_init(&plc_tx_queue, plc_buf, sizeof(plc_buf));
#endif
        HAL_UART_DeInit(&huart2);
        
        huart2.Instance = USART2;
        huart2.Init.BaudRate = baudrate;
        huart2.Init.WordLength = UART_WORDLENGTH_8B;
        huart2.Init.StopBits = UART_STOPBITS_1;
        huart2.Init.Parity = UART_PARITY_NONE;
        huart2.Init.Mode = UART_MODE_TX_RX;
        huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart2.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart2) != HAL_OK)
        {
          _Error_Handler(__FILE__, __LINE__);
        }
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}

  
void  uart4_init(uint32_t baudrate)
{
#if UART4_INTERRUPT_TX
	cirq_init(&psu_tx_queue, psu_buf, sizeof(psu_buf));
#endif
        
        HAL_UART_DeInit(&huart4);
  
        huart4.Instance = UART4;
        huart4.Init.BaudRate = baudrate;
        huart4.Init.WordLength = UART_WORDLENGTH_8B;
        huart4.Init.StopBits = UART_STOPBITS_1;
        huart4.Init.Parity = UART_PARITY_NONE;
        huart4.Init.Mode = UART_MODE_TX_RX;
        huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart4.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&huart4) != HAL_OK)
        {
           _Error_Handler(__FILE__, __LINE__);
        }
        
        __HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
}
static void UART1_putc(uint8_t data)
{
#if UART1_INTERRUPT_TX
  USART_TypeDef*port = _output_redirect ? USART2:USART1;
  
            cirq_enque(&debug_tx_queue, data);
      
      USART_ITConfig(port, USART_IT_TXE, ENABLE);
      
#else
        uint8_t buffer[4];
        
        buffer[0] = data;
        HAL_UART_Transmit(&huart1, (uint8_t *)buffer, 1, 1000);
#endif
}

static void UART1_sync_putc(uint8_t data)
{
        uint8_t buffer[4];
	
        buffer[0] = data;
        HAL_UART_Transmit(&huart1, (uint8_t *)buffer, 1, 1000);
}

static void UART2_putc(uint8_t data)
{
#if UART2_INTERRUPT_TX
	cirq_enque(&plc_tx_queue, data);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
#else
        uint8_t buffer[4] = {0,};
	
        buffer[0] = data;
        HAL_UART_Transmit(&huart2, (uint8_t *)buffer, 1, 1000);
#endif
}

static void UART3_putc(uint8_t data)
{
}

static void UART4_putc(uint8_t data)
{
#if UART4_INTERRUPT_TX
	cirq_enque(&psu_tx_queue, data);
	USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
#else
        uint8_t buffer[4];
	
        buffer[0] = data;
        HAL_UART_Transmit(&huart4, (uint8_t *)buffer, 1, 1000);
#endif
}

void uart_putc(uint8_t port, uint8_t ch)
{
        switch(port)
        {
        case 1:
            UART1_putc(ch);
             break;
        case 2:
             UART2_putc(ch);
             break;
        case 4:
             UART4_putc(ch);
             break;
        }
}

void uart_write(uint8_t port, const uint8_t* data, size_t data_len)
{
	typedef void (*UART_PUTC_func)(uint8_t data);

	static const UART_PUTC_func func_tables[] = {
		UART1_putc,
                UART2_putc,
                UART3_putc,
                UART4_putc
	};
	
	const UART_PUTC_func putc_func = func_tables[port - 1];
	size_t i;
        
	for(i = 0; i < data_len;  i++)
	{
		putc_func(data[i]);
	}
}

void uart_puts(uint8_t port, const char*str)
{
        size_t len = strlen(str);
        uart_write(port, (const uint8_t*)str,len);
}

void uart_puti(uint8_t port, int val)
{
        char buf[20];
        
        sprintf(buf, "%d", val);

        uart_puts(port, buf); 
}

void uart_wait_for_flush(uint8_t port)
{
       switch(port)
       {
       case 1:
#if UART1_INTERRUPT_TX
              while(!cirq_is_empty(&debug_tx_queue))
                ;
              delay_1ms();
#endif
              break;
        case 2:
#if UART2_INTERRUPT_TX
              while(!cirq_is_empty(&plc_tx_queue))
                ;
              delay_1ms();
#endif
              break;
        case 4:
#if UART4_INTERRUPT_TX
              while (!cirq_is_empty(&psu_tx_queue))
                ;
              delay_1ms();
#endif
              break;
       }     
}

void uart_debug_output(const char* str)
{
        while(*str)
        {
               UART1_sync_putc(*str++);
        }
}

void usat_set_output_redirect(bool_t enable)
{
        _output_redirect = (uint8_t)enable;
}

bool_t uart_is_output_redirected()
{
         return _output_redirect;
}