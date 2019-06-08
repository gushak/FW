#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "platform_env.h"
#include "comm_dbg.h"
#include "comm_uart.h"
#include "comm_str.h"
#include "comm_sys.h"


#define DEBUG_STR_MAX (1024)
static char dbg_buf[DEBUG_STR_MAX];

void cfileinfo(const char* file, int line, const char *tag)
{
        const char* file_name_only = file;
        const char* p = file;
        
        while (*p){
                if (*p++ == '\\')
                        file_name_only = p;
        }
#if 1
        {
                 char info[64], tag_buf[20], line_buf[92];
                 size_t info_len;
                 
                 strncpy(tag_buf, tag, 19); tag_buf[19] = 0;
                 sprintf(info, "%s(%d)", file_name_only, line);
                 info_len = strlen(info);
                 if (info_len > 16)
                 {
                        sprintf(line_buf, "%s [%-19s] ", &info[info_len-16], tag_buf);
                 }
                 else
                 {
                        sprintf(line_buf, "%-16s [%-19s] ", info, tag_buf);
                 }
                 uart_puts(DEBUG_UART_PORT, line_buf);
          }
#else
	uart_puts(DEBUG_UART_PORT, (char *)file_name_only);
	uart_putc(DEBUG_UART_PORT, '(');
	uart_puti(DEBUG_UART_PORT, line);
	uart_puts(DEBUG_UART_PORT, ") ");
	uart_puts(DEBUG_UART_PORT, (char *)tag);
	uart_putc(DEBUG_UART_PORT, ' ');
#endif
}

void chexdump(const char* title, const uint8_t* data, size_t len)
{
        char result[3];
        size_t i;
        
        uart_puts(DEBUG_UART_PORT, (char *)title);
	uart_putc(DEBUG_UART_PORT, '(');
	uart_puti(DEBUG_UART_PORT, len);
	uart_puts(DEBUG_UART_PORT, ") ");
        
        for (i = 0; i < len; i++){
                hex_str_from_bin(data + i, 1, result, sizeof(result));
                uart_putc(DEBUG_UART_PORT, result[0]);
                delay_1ms();
                uart_putc(DEBUG_UART_PORT, result[1]);
                delay_1ms();
#if 1                   
                uart_putc(DEBUG_UART_PORT, ' ');
#else
                if ((i + 1) % 5 == 0)
                        uart_putc(DEBUG_UART_PORT, '');
#endif
                delay_1ms();
         }
         uart_putc(DEBUG_UART_PORT, '\r');
         uart_putc(DEBUG_UART_PORT, '\n');
}

void cprintf(const char *fmt, ...)
{
         va_list ap;
         
         va_start(ap, fmt);
         vsprintf(dbg_buf, fmt, ap);
         va_end(ap);
         
         uart_puts(DEBUG_UART_PORT, (char *)dbg_buf);
         uart_puts(DEBUG_UART_PORT,"\r\n");
}

void csync_puts(const char *str)
{
        uart_debug_output(str);
        uart_puts(DEBUG_UART_PORT, str);
        uart_wait_for_flush(DEBUG_UART_PORT);
}



