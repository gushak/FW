#include "hw_control.h"
#include "uart.h"

void Uart_putchar(USART_TypeDef* USARTx, char d);

#ifdef __GNUC__
 #define PUTCHAR_PROTYPE int__io_putchar(int ch)
#else
 #define PUTCHAR_PROTYPE int fputc(int ch, FILE *f)
#endif 

PUTCHAR_PROTYPE
{
	Uart_putchar(USART1, ch);
	return ch;  
}

//UART1 Q Buffer
#define USART_BUFFER_SIZE  1024
#define USART_BUFFER_SIZE2  1024
#define USART_BUFFER_SIZE3  1024
__IO u8 U1_rx_buffer[USART_BUFFER_SIZE];
uint32_t U1_rx_point_head=0, U1_rx_point_tail=0;

__IO u8 U2_rx_buffer[USART_BUFFER_SIZE2];
uint32_t U2_rx_point_head=0, U2_rx_point_tail=0;

__IO u8 U3_rx_buffer[USART_BUFFER_SIZE3];
uint32_t U3_rx_point_head=0, U3_rx_point_tail=0;

void increase_point_value(u32 *p)
{
	(*p)++;
	if(USART_BUFFER_SIZE == (*p))
	{
		(*p) = 0; 
	}
}

//------------------------------------------------------------------
// UART1
//------------------------------------------------------------------
uint8_t uart1_is_empty(void)
{
	if(U1_rx_point_head == U1_rx_point_tail) {
		return 1; 
	}
	return 0;
}

u8 uart1_dequeue(void)
{
	u8 rev = U1_rx_buffer[U1_rx_point_tail];
	increase_point_value(&U1_rx_point_tail);
	return rev;
}

void Uart1_enqueue(u8 data)
{
	U1_rx_buffer[U1_rx_point_head] = data;
	increase_point_value(&U1_rx_point_head);  
}
//------------------------------------------------------------------
// UART2
//------------------------------------------------------------------
uint8_t uart2_is_empty(void)
{
	if(U2_rx_point_head == U2_rx_point_tail) {
		return 1; 
	}
	return 0;
}

u8 uart2_dequeue(void)
{
	u8 rev = U2_rx_buffer[U2_rx_point_tail];
	increase_point_value(&U2_rx_point_tail);
	return rev;
}

void Uart2_enqueue(u8 data)
{
	U2_rx_buffer[U2_rx_point_head] = data;
	increase_point_value(&U2_rx_point_head);  
}
//------------------------------------------------------------------
// UART3
//------------------------------------------------------------------
uint8_t uart3_is_empty(void)
{
	if(U3_rx_point_head == U3_rx_point_tail) {
		return 1; 
	}
	return 0;
}

u8 uart3_dequeue(void)
{
	u8 rev = U3_rx_buffer[U3_rx_point_tail];
	increase_point_value(&U3_rx_point_tail);
	return rev;
}

void Uart3_enqueue(u8 data)
{
	U3_rx_buffer[U3_rx_point_head] = data;
	increase_point_value(&U3_rx_point_head);  
}

//------------------------------
// IRQ Handler
//------------------------------
extern void USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(uint8_t ch);
void USART1_IRQHandler(void)	
{
	u8 cc=0;	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {  
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
		cc = USART_ReceiveData(USART1);
		Uart1_enqueue(cc);
		
		//USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(cc);
	}  
}
void USART2_IRQHandler(void)	
{
	//Àú¿ï !!
	u8 cc=0;	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {  
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); 
		cc = USART_ReceiveData(USART2);
		
		//Uart2_enqueue(cc);
		parse_weight_interrupt(cc);
		
		//USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(cc);
	}  
}

void USART3_IRQHandler(void)
{
	u8 cc=0;	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {  
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
		cc = USART_ReceiveData(USART3);
		Uart3_enqueue(cc);
		
	}  
}
#define UART2_ENABLE 
#define UART3_ENABLE
void UART_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	USART_InitTypeDef USART_InitStructure;

	// UART 1 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	// UART 1,2 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
#ifdef UART2_ENABLE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
	
#ifdef UART3_ENABLE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b ;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_InitStructure.USART_BaudRate = 9600;			
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
	
#ifdef UART2_ENABLE
	USART_InitStructure.USART_BaudRate = 19200;			
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);
#endif
	
#ifdef UART3_ENABLE
	USART_InitStructure.USART_BaudRate = 9600;			
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
#endif
}



void Uart1_putchar(u8 c)
{
	USART_SendData(USART1,(uint8_t) c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}

void Uart_putchar(USART_TypeDef* USARTx, char d)
{
	USART_SendData(USARTx,(uint16_t) d);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
}

void Uart_puts(USART_TypeDef* USARTx, char *d)
{
	while (*d)
		Uart_putchar(USARTx, *(d++));	
}

void Uart_puts_length(USART_TypeDef* USARTx, uint8_t *d, uint16_t cnt)
{
	while (cnt--)
		Uart_putchar(USARTx, *(d++));	
}
