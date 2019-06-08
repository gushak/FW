#ifndef _UART_H_
#define _UART_H_

extern void USART1_init(uint32_t baud);
extern void UART_init(void);
extern void Uart_puts(USART_TypeDef* USARTx, char *d);
extern void Uart1_putchar(u8 c);
extern void Uart2_putchar(u8 c);
extern void Uart3_putchar(u8 c);
extern void Uart4_putchar(u8 c);

extern uint8_t uart1_is_empty(void);
extern uint8_t uart2_is_empty(void);
extern uint8_t uart3_is_empty(void);
extern uint8_t uart4_is_empty(void);

extern u8 uart1_dequeue(void);
extern u8 uart2_dequeue(void);
extern u8 uart3_dequeue(void);
extern u8 uart4_dequeue(void);

extern __IO u8 U2_tx_buffer[];
extern uint8_t U2_tx_point_head, U2_tx_point_tail;
extern void Uart_puts_length(USART_TypeDef* USARTx, uint8_t *d, uint16_t cnt);

#endif