#include "smartplug_cli.h"
#include "circularq.h"
#include "smartplug_hw.h"

extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

#if _USE_CLI

#define CLI_BUF_SIZE 16

static uint8_t cli_buf[CLI_BUF_SIZE +1];

CirQ_t cli_rx_queue;

void cli_init()
{
      memset(cli_buf, 0 , sizeof(cli_buf));  
      cirq_init(&cli_rx_queue, cli_buf, sizeof(cli_buf));
}

void cli_put_input(uint8_t ch)
{
      cirq_enque(&cli_rx_queue, ch);
}

static void cli_help()
{
      static const char* cmds[] = {
        "r: rtc time",
      };
      size_t i;
      
      uart_puts(DEBUG_UART_PORT, "** CLI command ** \r\n");
      
      for (i = 0; i < sizeof(cmds) / sizeof(cmds[0]); ++i)
      {
            uart_puts(DEBUG_UART_PORT, " ");
            uart_puts(DEBUG_UART_PORT, cmds[i]);
            uart_puts(DEBUG_UART_PORT, "\r\n");
            uart_wait_for_flush(DEBUG_UART_PORT);
      }
}


void cli_handle_command()
{
     bool_t b;
     uint8_t ch;
     
     while (!cirq_is_empty(&cli_rx_queue))
     {
          ch = cirq_direct_dequeue(&cli_rx_queue);
          
          switch (ch)
          {
                case 'r':
                          while(1)
                          {
                                ch = cirq_direct_dequeue(&cli_rx_queue);
                                if (ch == 'e')
                                {
                                      break;
                                }
                                printf("%02x,%02x,%02x %02x,%02x,%02x \r\n",sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
                          }
                          break;
                case 'h':
                        cli_help();
                        break;
                default:
                        break;
          }
     }
     
}

#endif
