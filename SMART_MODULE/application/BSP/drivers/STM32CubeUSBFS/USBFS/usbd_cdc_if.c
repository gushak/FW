/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @brief          :
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
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
#include "usbd_cdc_if.h"
#include "main.h"

#include "serial_api.h"
#include "i2c_api.h"
#include "spi_api.h"
#if DEVICE_CAN
#include "can_api.h"
#endif

/* USER CODE BEGIN INCLUDE */
/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_CDC 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CDC_Private_TypesDefinitions
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_TYPES */
/* USER CODE END PRIVATE_TYPES */ 
/**
  * @}
  */ 

/** @defgroup USBD_CDC_Private_Defines
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_DEFINES */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  CDC_SERIAL_BUFFER_SIZE/4
/* USER CODE END PRIVATE_DEFINES */
/**
  * @}
  */ 

// The CDC line coding parameters
USBD_CDC_LineCodingTypeDef LineCoding = {
		115200, // baud rate
		0x00,   // stop bits: 1
		0x00,   // parity: none
		0x08    // number of bits: 8
};
/** @defgroup USBD_CDC_Private_Macros
  * @{
  */ 
/* USER CODE BEGIN PRIVATE_MACRO */
/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */ 
  
/** @defgroup USBD_CDC_Private_Variables
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/* Received Data over USB are stored in this buffer       */
__ALIGN_BEGIN uint8_t UserRxBuffer[APP_RX_DATA_SIZE] __ALIGN_END;/* Received Data over USB are stored in this buffer */


/* USER CODE BEGIN PRIVATE_VARIABLES */

uint8_t dtr_pin = 0; //DTR pin is disabled

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */ 
  
/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */ 
  
/** @defgroup USBD_CDC_Private_FunctionPrototypes
  * @{
  */
static int8_t CDC_Init_FS     (void);
static int8_t CDC_DeInit_FS   (void);
static int8_t CDC_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS  (uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */ 
  
USBD_CDC_ItfTypeDef USBD_CDC_fops = 
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,  
  CDC_Receive_FS,
  NULL
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CDC_Init_FS
  *         Initializes the CDC media low layer over the FS USB IP
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{ 
  /* USER CODE BEGIN 3 */ 
  /* Set Application Buffers */
	USBD_CDC_SetRxBuffer(USBD_CDC_fops.pDevice, UserRxBuffer, sizeof(UserRxBuffer));
  return (USBD_OK);
  /* USER CODE END 3 */ 
}

/**
  * @brief  CDC_DeInit_FS
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */ 
  return (USBD_OK);
  /* USER CODE END 4 */ 
}


/**
  * @brief  CDC_Control_FS
  *         Manage the CDC class requests
  * @param  cmd: Command code            
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
  /* USER CODE BEGIN 5 */
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
 
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
 
    break;

  case CDC_SET_COMM_FEATURE:

    break;

  case CDC_GET_COMM_FEATURE:

    break;

  case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */ 
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
  case CDC_SET_LINE_CODING:   
	  LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) | (pbuf[2] << 16) | (pbuf[3] << 24));
	  LineCoding.format     = pbuf[4];	// 0: 1bit Stop, 2: 2bits Stop
	  LineCoding.paritytype = pbuf[5];	// 0:None, 1:Odd, 2:Even, 3:Mark, 4:Space
	  LineCoding.datatype   = pbuf[6];	// 5:5bits, 6:6bits, 7:7bits, 8:8bits
	  
	  if (LineCoding.datatype != 8)	// Device direct mode
	  {
		  setDirectDevice(&LineCoding);
		  break;
	  }
#if defined(GPS_FW_UPDATE)
	  if (BitOpMode & eDOM_GPS)
		  break;
	  BitOpMode = eDOM_GPS;
	  resumeGPS();
#else
	  switch (LineCoding.bitrate)
	  {
	  case 1200:	//GPS pass-through mode
		  if (BitOpMode & eDOM_GPS)
			  break;
		  BitOpMode = eDOM_GPS;
		  resumeGPS();
		  closePCIF();
		  break;
	  case 2400:	//MCU register debugger mode
		  if (BitOpMode & eDOM_MCU)
			  break;
		  BitOpMode = eDOM_MCU;
		  suspendGPS();
		  closePCIF();
		  break;
	  case 4800:	//MCU serial debug mode
		  if (BitOpMode & eDOM_DEBUG)
			  break;
		  BitOpMode = eDOM_DEBUG;
		  setUSBProcessStep(true);		  
		  closePCIF();
		  break;
	  default:
		  if (BitOpMode & eDOM_PCIF)
			  break;
		  BitOpMode = eDOM_PCIF;
		  suspendGPS();
		  break;
	  }
#endif
    break;

  case CDC_GET_LINE_CODING:     
	  pbuf[0] = (uint8_t)(LineCoding.bitrate);
	  pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
	  pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
	  pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
	  pbuf[4] = LineCoding.format;
	  pbuf[5] = LineCoding.paritytype;
	  pbuf[6] = LineCoding.datatype;
	  // Add your code here
    break;

  case CDC_SET_CONTROL_LINE_STATE:
    dtr_pin++; //DTR pin is enabled
               //TODO: basically this is not very correct. It needs to analyze data 0-bit to get DTR state
	  if (!pbuf[0] && !pbuf[1])
		  break;
	  else if (pbuf[0] == 0x02 && pbuf[1] == 0x20)
	  {
		  BitOpMode = 0;
		  suspendGPS();
		  closePCIF();
	  }
	  break;
  case CDC_SEND_BREAK:
 
    break;    
    
  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  CDC_Receive_FS
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS (uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
	USBD_CDC_HandleTypeDef *hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	if (_size(hcdc->Rx) < *Len || (hcdc->Rx._w & 0x3)) 
		hcdc->Rx._w = hcdc->Rx._r = 0;

	USBD_CDC_ReceivePacket(USBD_CDC_fops.pDevice);
	hcdc->Rx._w = _inc(hcdc, hcdc->Rx._w, *Len);
	
	return (USBD_OK);
  /* USER CODE END 6 */ 
}

/**
  * @brief  CDC_Transmit_FS
  *         Data send over USB IN endpoint are sent over CDC interface 
  *         through this function.           
  *         @note
  *         
  *                 
  * @param  Buf: Buffer of data to be send
  * @param  Len: Number of data to be send (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
int CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  /* USER CODE BEGIN 7 */ 
	USBD_CDC_HandleTypeDef *hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	
	if (hcdc->TxState == 1)
	    return -1;

	USBD_CDC_SetTxBuffer(USBD_CDC_fops.pDevice, Buf, Len);
	if (USBD_CDC_TransmitPacket(USBD_CDC_fops.pDevice, Len))
		return -1;
  /* USER CODE END 7 */ 
	return Len;
}

uint8_t * CDC_GetRxBuffer()
{
	USBD_CDC_HandleTypeDef *hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	return hcdc->Rx.Buffer + hcdc->Rx._r;
}
void CDC_FlushRx(int size)
{
	USBD_CDC_HandleTypeDef *hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	hcdc->Rx._r = _inc(hcdc, hcdc->Rx._r, size);
}
uint32_t CDC_RxDataSize(void)
{
	USBD_CDC_HandleTypeDef *hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	return _size(hcdc->Rx);
}

uint32_t CDC_GetTxState(void)
{
	USBD_CDC_HandleTypeDef *hcdc;
	if (USBD_CDC_fops.pDevice == NULL || USBD_CDC_fops.pDevice->pClassDataCDC == NULL)
	{
		hcdc->TxRespTick = HAL_GetTick();
		return -1;
	}
	hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	if (HAL_GetElapsedTick(hcdc->TxRespTick) > 200 && hcdc->TxState)	//PC application doesn't respond within 200ms, Then the device itself clears TxState.
		hcdc->TxState = 0;
	
	return hcdc->TxState;
}

char CDC_GetC(void)
{
	USBD_CDC_HandleTypeDef *hcdc = USBD_CDC_fops.pDevice->pClassDataCDC;
	if (hcdc->Rx._w == hcdc->Rx._r)
		return 0;

	int remaining = _size(hcdc->Rx);
	int todo = MIN(remaining, 1);
	if (todo <= 0)
		return 0;
	char ch = *(char*)(hcdc->Rx.Buffer + hcdc->Rx._r);
	hcdc->Rx._r = _inc(hcdc, hcdc->Rx._r, 1);
	return ch;
}

uint8_t CDC_PutC(char c) 
{
	uint8_t ce = (uint8_t)c;
	
	/* Send data over USB */
	return CDC_Transmit_FS(&ce, 1);
}
uint8_t CDC_Put(uint8_t* Buf, uint16_t Len)
{
	return CDC_Transmit_FS(Buf, Len);
}
/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

