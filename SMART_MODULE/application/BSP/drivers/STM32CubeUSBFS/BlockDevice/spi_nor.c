/**
  ******************************************************************************
  * @file    stm32_adafruit_sd.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the SD card
  *          mounted on the Adafruit 1.8" TFT LCD shield (reference ID 802),
  *          that is used with the STM32 Nucleo board through SPI interface.
  *          It implements a high level communication layer for read and write 
  *          from/to this memory. The needed STM32XXxx hardware resources (SPI and 
  *          GPIO) are defined in stm32XXxx_nucleo.h file, and the initialization is 
  *          performed in IO_Init() function declared in stm32XXxx_nucleo.c 
  *          file.
  *          You can easily tailor this driver to any other development board, 
  *          by just adapting the defines for hardware resources and 
  *          IO_Init() function.
  *            
  *          +-------------------------------------------------------+
  *          |                     Pin assignment                    |
  *          +-------------------------+---------------+-------------+
  *          |  STM32XXxx SPI Pins     |     SD        |    Pin      |
  *          +-------------------------+---------------+-------------+
  *          | SD_SPI_CS_PIN           |   ChipSelect  |    1        |
  *          | SD_SPI_MOSI_PIN / MOSI  |   DataIn      |    2        |
  *          |                         |   GND         |    3 (0 V)  |
  *          |                         |   VDD         |    4 (3.3 V)|
  *          | SD_SPI_SCK_PIN / SCLK   |   Clock       |    5        |
  *          |                         |   GND         |    6 (0 V)  |
  *          | SD_SPI_MISO_PIN / MISO  |   DataOut     |    7        |
  *          +-------------------------+---------------+-------------+
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How to use this driver:
--------------------------
   - This driver does not need a specific component driver for the micro SD device
     to be included with.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the micro SD card using the BSP_SD_Init() function. 
     o Checking the SD card presence is not managed because SD detection pin is
       not physically mapped on the Adafruit shield.
     o The function BSP_SD_GetCardInfo() is used to get the micro SD card information 
       which is stored in the structure "SD_CardInfo".
  
  + Micro SD card operations
     o The micro SD card can be accessed with read/write block(s) operations once 
       it is ready for access. The access can be performed in polling 
       mode by calling the functions BSP_SD_ReadBlocks()/BSP_SD_WriteBlocks()
       
     o The SD erase block(s) is performed using the function BSP_SD_Erase() with 
       specifying the number of blocks to erase.
     o The SD runtime status is returned when calling the function BSP_SD_GetStatus().
     
------------------------------------------------------------------------------*/ 

/* Includes ------------------------------------------------------------------*/
#include "spi_nor.h"
#include "spi_dma/spi_dma.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32_ADAFRUIT
  * @{
  */ 
  
/** @defgroup STM32_ADAFRUIT_SD
  * @{
  */ 

  
  
/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32_ADAFRUIT_SD_Private_Variables
  * @{
  */       
__IO uint8_t SdStatus = SD_NOT_PRESENT;
#if defined(__SPI_DMA__)
SPI_TypeDef *_spi_id;
#endif
/**
  * @}
  */ 

/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM32_ADAFRUIT_SD_Private_Function_Prototypes
  * @{
  */ 
/**
  * @}
  */
/* Private functions ---------------------------------------------------------*/
    
/** @defgroup STM32_ADAFRUIT_SD_Private_Functions
  * @{
  */ 
  
/**
  * @brief  Initializes the SD/SD communication.
  * @param  None
  * @retval The SD Response: 
  *         - MSD_ERROR: Sequence failed
  *         - MSD_OK: Sequence succeed
  */
uint8_t BSP_SD_Init(void)
{ 
  /* Configure IO functionalities for SD pin */
  IO_Init();

  /* SD detection pin is not physically mapped on the Adafruit shield */
  SdStatus = SD_PRESENT;
  
  /* SD initialized and set to SPI mode properly */
  return 0;
}

/**
  * @brief  Returns information about specific card.
  * @param  pCardInfo: Pointer to a SD_CardInfo structure that contains all SD 
  *         card information.
  * @retval The SD Response:
  *         - MSD_ERROR: Sequence failed
  *         - MSD_OK: Sequence succeed
  */
uint8_t BSP_SD_GetCardInfo(SD_CardInfo *pCardInfo)
{
	pCardInfo->LogBlockSize = V_SECTOR_SIZE;
	pCardInfo->CardBlockSize = V_SECTOR_SIZE;
	pCardInfo->CardCapacity = TOTAL_V_SECTORS * pCardInfo->LogBlockSize;
	pCardInfo->LogBlockNbr = TOTAL_V_SECTORS;

  return 0;
}

uint8_t BSP_SD_GetCardState(void) 
{ 
	return SD_TRANSFER_OK;	
}
/**
  * @brief  Reads block(s) from a specified address in the SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read. The address is counted 
  *                   in blocks of 512bytes
  * @param  NumOfBlocks: Number of SD blocks to read
  * @param  Timeout: This parameter is used for compatibility with BSP implementation
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
	for (int i = 0; i< 2*NumOfBlocks; i++){ //0,1
		IO_CSState(0);
		#if defined(ADDRESSING_4BYTES)
		IO_WriteByte(READ_4DATA); // READ
		IO_WriteByte(0xff & (ReadAddr >> 24));	//MSB
		#else		IO_WriteByte(READ_DATA); // READ
		#endif
		
		IO_WriteByte(0xff & (ReadAddr >> 16));	//
		IO_WriteByte(0xff & (ReadAddr >> 8));	//
		IO_WriteByte(0xff & (ReadAddr));		//LSB
		#if defined( __SPI_DMA__)
		uint8_t dummyByte = 0xFF;
		spi_dma_read(_spi_id, &dummyByte, (uint8_t *)&pData[PAGE_SIZE*i], PAGE_SIZE);
		#else
		for (int j = (PAGE_SIZE * i); j < (PAGE_SIZE << i); j++){
			pData[j] = IO_WriteByte(SD_DUMMY_BYTE);
		}
		#endif
		IO_CSState(1);
		ReadAddr += PAGE_SIZE;
	}

	return BSP_SD_OK;
}

/**
  * @brief  Writes block(s) to a specified address in the SD card, in polling mode. 
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written. The address is counted 
  *                   in blocks of 512bytes
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: This parameter is used for compatibility with BSP implementation
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
	// send the data block
	for (int i = 0; i< 2*NumOfBlocks; i++){ //0,1
		if (WREN_Sync(Timeout/10) != BSP_SD_OK)
			return BSP_SD_ERROR;
			
		IO_CSState(0);
		#if defined(ADDRESSING_4BYTES)
		IO_WriteByte(PAGE_4PROGRAM); //PAGE_PROGRAM
		IO_WriteByte(0xff & (WriteAddr >> 24)); //MSB
		#else		IO_WriteByte(PAGE_PROGRAM); //PAGE_PROGRAM
		#endif
		IO_WriteByte(0xff & (WriteAddr >> 16)); //
		IO_WriteByte(0xff & (WriteAddr >> 8)); //
		IO_WriteByte(0xff & (WriteAddr)); //LSB
		#if defined( __SPI_DMA__)
		uint8_t dummyByte;
		spi_dma_write(_spi_id, (uint8_t *)&pData[PAGE_SIZE*i], &dummyByte, PAGE_SIZE);
		#else
		for (int j = (PAGE_SIZE * i); j < (PAGE_SIZE << i); j++){
			IO_WriteByte(pData[j]);
		}
		#endif
		IO_CSState(1);
		if (IO_Sync(Timeout) != BSP_SD_OK)
			return BSP_SD_ERROR;
		WriteAddr += 256;
	}
	return BSP_SD_OK;
}

/**
  * @brief  Erases the specified memory area of the given SD card. 
  * @param  StartAddr: Start address in Blocks (Size of a block is 512bytes)
  * @param  EndAddr: End address in Blocks (Size of a block is 512bytes)
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
	uint32_t curAddr = StartAddr;

	while (1) {
		if (WREN_Sync(SPIF_TIMEOUT) != BSP_SD_OK)
			return BSP_SD_ERROR;
		IO_CSState(0);
		#if defined(ADDRESSING_4BYTES)
		IO_WriteByte(SECTOR_4ERASE4K);
		IO_WriteByte(0xff & (curAddr >> 24)); //MSB
		#else		IO_WriteByte(SECTOR_ERASE4K);
		#endif
		IO_WriteByte(0xff & (curAddr >> 16)); //
		IO_WriteByte(0xff & (curAddr >> 8)); //
		IO_WriteByte(0xff & (curAddr)); //LSB
		IO_CSState(1);
		if (IO_Sync(SPIF_TIMEOUT * 10) != BSP_SD_OK)
			return BSP_SD_ERROR;
		curAddr += 4096;	// increase 4K at once.
		if (curAddr >= EndAddr)
			break;
	}
	return BSP_SD_OK;
}


/**
  * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @retval Returns if SD is detected or not
  */
uint8_t BSP_SD_IsDetected(void)
{
	return SD_PRESENT;
}

/**
  * @}
  */  

/**
  * @}
  */ 
uint32_t SpixTimeout = 1000; /*<! Value of Timeout when SPI communication fails */
static SPI_HandleTypeDef _Spi; 
static void       SPIx_Init(void);
static void       SPIx_Write(uint8_t Value);
static void       SPIx_Error(void);
static void       SPIx_MspInit(SPI_HandleTypeDef *hspi);

/**
  * @}
  */ 
/**
  * @brief  Initializes SPI MSP.
  * @param  None
  * @retval None
  */
static void SPIx_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  
  
	/*** Configure the GPIOs ***/  
	/* Enable GPIO clock */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  
	/* Configure SPI SCK */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	  /* Configure SPI MISO and MOSI */ 
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*** Configure the SPI peripheral ***/ 
	/* Enable SPI clock */
	__HAL_RCC_SPI3_CLK_ENABLE();
}
/**
  * @brief  Initializes SPI HAL.
  * @param  None
  * @retval None
  */
static void SPIx_Init(void)
{
	SPI_HandleTypeDef _Spi;

	if (HAL_SPI_GetState(&_Spi) == HAL_SPI_STATE_RESET)
	{
	  /* SPI Config */
		_Spi.Instance = SPI3;
		  /* SPI baudrate is set to 13,5 MHz maximum (APB2/SPI_BaudRatePrescaler = 108/8 = 13,5 MHz) 
		   to verify these constraints:
		      - ST7735 LCD SPI interface max baudrate is 15MHz for write and 6.66MHz for read
		        Since the provided driver doesn't use read capability from LCD, only constraint 
		        on write baudrate is considered.
			  - SD card SPI interface max baudrate is 25MHz for write/read
			  - PCLK2 max frequency is 108 MHz 
		   */ 
		_Spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; //PCLK1(42MHz)/2 = 21MHz
		_Spi.Init.Direction = SPI_DIRECTION_2LINES;
		_Spi.Init.CLKPhase = SPI_PHASE_2EDGE;
		_Spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
		_Spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		_Spi.Init.CRCPolynomial = 7;
		_Spi.Init.DataSize = SPI_DATASIZE_8BIT;
		_Spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
		_Spi.Init.NSS = SPI_NSS_SOFT;
		_Spi.Init.TIMode = SPI_TIMODE_DISABLE;
		_Spi.Init.Mode = SPI_MODE_MASTER;

		SPIx_MspInit(&_Spi);
		HAL_SPI_Init(&_Spi);
	}
#if defined(__SPI_DMA__)
	_spi_id = spi_get_id(EF_MOSI, EF_MISO, EF_SCK, NC);
	spi_dma_init(_spi_id);
#endif
}

/**
  * @brief  SPI Write a byte to device
  * @param  Value: value to be written
  * @retval None
  */
static void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLegnth)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_SPI_TransmitReceive(&_Spi, (uint8_t*) DataIn, DataOut, DataLegnth, SpixTimeout);
    
	/* Check the communication status */
	if (status != HAL_OK)
	{
	  /* Execute user timeout callback */
		SPIx_Error();
	}
}

/**
  * @brief  SPI Write a byte to device.
  * @param  Value: value to be written
  * @retval None
  */
static void SPIx_Write(uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t data;

	status = HAL_SPI_TransmitReceive(&_Spi, (uint8_t*) &Value, &data, 1, SpixTimeout);
  
	/* Check the communication status */
	if (status != HAL_OK)
	{
	  /* Execute user timeout callback */
		SPIx_Error();
	}
}

/**
  * @brief  SPI error treatment function
  * @param  None
  * @retval None
  */
static void SPIx_Error(void)
{
  /* De-initialize the SPI communication BUS */
	HAL_SPI_DeInit(&_Spi);
  
	/* Re-Initiaize the SPI communication BUS */
	SPIx_Init();
}

/**
  * @}
  */ 
/********************************* LINK SD ************************************/
/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for 
  *         data transfer).
  * @param  None
  * @retval None
  */
void IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	uint8_t counter;

	  /* SD_CS_GPIO Periph clock enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	  /* Configure SD_CS_PIN pin: SD Card CS pin */
	GPIO_InitStruct.Pin = SD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStruct);


	/*------------Put SD in SPI mode--------------*/
	/* SD SPI Config */
	SPIx_Init();
  
	/* SD chip select high */
	SD_CS_HIGH();
}

/**
  * @brief  Set the SD_CS pin.
  * @param  pin value.
  * @retval None
  */
void IO_CSState(uint8_t val)
{
	if (val == 1) 
	{
		SD_CS_HIGH();
	}
	else
	{
		SD_CS_LOW();
	}
}

/**
  * @brief  Write a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
void IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  /* Send the byte */
	SPIx_WriteReadData(DataIn, DataOut, DataLength);
}

/**
  * @brief  Writes a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
uint8_t IO_WriteByte(uint8_t Data)
{
	uint8_t tmp;
	/* Send the byte */
	SPIx_WriteReadData(&Data, &tmp, 1);
	return tmp;
}

uint8_t IO_Sync(uint32_t timeout)
{
	// Read status register until write not-in-progress
	uint8_t status;
	for (int i = 0; i < timeout; i++) {
		SD_CS_LOW();
		// Use it to check the chip configuration.
		IO_WriteByte(READ_STATUS_REG); // read Status Register
		status = IO_WriteByte(SD_DUMMY_BYTE);
		SD_CS_HIGH();

		// Check WIP bit
		if (!(status & SPIF_WIP)) {
			return BSP_SD_OK;
		}

		wait_ms(1);
	}

	return BSP_SD_ERROR;
}
uint8_t WREN_Sync(uint32_t timeout)
{
	uint8_t status;
	
	SD_CS_LOW();
	IO_WriteByte(WRITE_ENABLE);
	SD_CS_HIGH();

	for (int i = 0; i < timeout; i++) {
		SD_CS_LOW();
		// Use it to check the chip configuration.
		IO_WriteByte(READ_STATUS_REG); // read Status Register
		status = IO_WriteByte(SD_DUMMY_BYTE);
		SD_CS_HIGH();

		// Check WIP bit
		if (status & SPIF_WEL) {
			return BSP_SD_OK;
		}

		wait_ms(1);
	}

	return BSP_SD_ERROR;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
