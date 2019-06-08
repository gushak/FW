/**
 * @file debug.c
 * @brief Debugging facilities
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Eval.
 *
 * This software is provided in source form for a short-term evaluation only. The
 * evaluation license expires 90 days after the date you first download the software.
 *
 * If you plan to use this software in a commercial product, you are required to
 * purchase a commercial license from Oryx Embedded SARL.
 *
 * After the 90-day evaluation period, you agree to either purchase a commercial
 * license or delete all copies of this software. If you wish to extend the
 * evaluation period, you must contact sales@oryx-embedded.com.
 *
 * This evaluation software is provided "as is" without warranty of any kind.
 * Technical support is available as an option during the evaluation period.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.8.2
 **/

//Dependencies
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "debug.h"

//Variable declaration
static UART_HandleTypeDef UART_Handle;


/**
 * @brief Debug UART initialization
 * @param[in] baudrate UART baudrate
 **/

void debugInit(uint32_t baudrate)
{
   GPIO_InitTypeDef GPIO_InitStructure;

   //Enable GPIOD clock
   __HAL_RCC_GPIOD_CLK_ENABLE();
   //Enable USART3 clock
   __HAL_RCC_USART3_CLK_ENABLE();

   //Configure USART3_TX (PD8)
   GPIO_InitStructure.Pin = GPIO_PIN_8;
   GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStructure.Pull = GPIO_PULLUP;
   GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
   GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
   HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

   //Configure USART3_RX (PD9)
   GPIO_InitStructure.Pin = GPIO_PIN_9;
   GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStructure.Pull = GPIO_PULLUP;
   GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
   GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
   HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

   //Configure USART3
   UART_Handle.Instance = USART3;
   UART_Handle.Init.BaudRate = baudrate;
   UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
   UART_Handle.Init.StopBits = UART_STOPBITS_1;
   UART_Handle.Init.Parity = UART_PARITY_NONE;
   UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
   UART_Handle.Init.Mode = UART_MODE_TX_RX;
   HAL_UART_Init(&UART_Handle);
}


/**
 * @brief Display the contents of an array
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @param[in] prepend String to prepend to the left of each line
 * @param[in] data Pointer to the data array
 * @param[in] length Number of bytes to display
 **/

void debugDisplayArray(FILE *stream,
   const char_t *prepend, const void *data, size_t length)
{
   uint_t i;

   for(i = 0; i < length; i++)
   {
      //Beginning of a new line?
      if((i % 16) == 0)
         fprintf(stream, "%s", prepend);
      //Display current data byte
      fprintf(stream, "%02" PRIX8 " ", *((uint8_t *) data + i));
      //End of current line?
      if((i % 16) == 15 || i == (length - 1))
         fprintf(stream, "\r\n");
   }
}


/**
 * @brief Write character to stream
 * @param[in] c The character to be written
 * @param[in] stream Pointer to a FILE object that identifies an output stream
 * @return On success, the character written is returned. If a writing
 *   error occurs, EOF is returned
 **/

int_t fputc(int_t c, FILE *stream)
{
   //Standard output or error output?
   if(stream == stdout || stream == stderr)
   {
      //Character to be written
      uint8_t ch = c;

      //Transmit data
      HAL_UART_Transmit(&UART_Handle, &ch, 1, HAL_MAX_DELAY);

      //On success, the character written is returned
      return c;
   }
   //Unknown output?
   else
   {
      //If a writing error occurs, EOF is returned
      return EOF;
   }
}
