 #pragma once

#include "comm_def.h"

#include "circularq.h"

// debug message
#define UART1_INTERRUPT_TX 0

//plc communatcation
#define UART2_INTERRUPT_TX 0

// MCU communication
#define UART4_INTERRUPT_TX 0

#if UART1_INTERRUPT_TX
extern CirQ_t debug_tx_queue;
#endif

#if UART2_INTERRUPT_TX
extern CirQ_t plc_tx_queue;
#endif

#if UART4_INTERRUPT_TX
extern CirQ_t mcu_tx_queue;
#endif

void  uart1_init(uint32_t baudrate);
void  uart2_init(uint32_t baudrate);
void  uart3_init(uint32_t baudrate);
void  uart4_init(uint32_t baudrate);
void  uart6_init(uint32_t baudrate);

void uart_putc(uint8_t port, uint8_t ch);
void uart_write(uint8_t port, const uint8_t* data, size_t data_len);
void uart_puts(uint8_t port, const char* str);
void uart_puti(uint8_t port, int val);
void uart_wait_for_flush(uint8_t port);

// none interrupt synchronous debug output function.
void uart_debug_output(const char* str);

void uart_set_output_redirect(bool_t enable);

bool_t uart_is_output_redirected();
