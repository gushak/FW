#if defined(TARGET_STM32F0) || defined(TARGET_STM32L0)
/*

This file is licensed under Apache 2.0 license.
(C) 2016 dinau

*/
#include "spi_dma.h"

/* For SPI1 */
#define DMAx_CLK_ENABLE()     __DMA1_CLK_ENABLE()    
#define SPIx_TX_DMA_CHANNEL   DMA1_Channel3
#define SPIx_RX_DMA_CHANNEL   DMA1_Channel2

#if defined(TARGET_STM32F091RC) 
#define SPIx_DMA_IRQn         DMA1_Ch2_3_DMA2_Ch1_2_IRQn
#define DMA_SPI_IRQHandler    DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler
#else
#define SPIx_DMA_IRQn         DMA1_Channel2_3_IRQn    
#define DMA_SPI_IRQHandler    DMA1_Channel2_3_IRQHandler
#endif



#define readReg( reg, mask)  (  (reg) & (mask)  )    

void spi_dma_get_info( SPI_TypeDef *spi )
{
    SpiHandle.Instance               = spi;
    SpiHandle.Init.Mode              = readReg(spi->CR1, SPI_MODE_MASTER); 
    SpiHandle.Init.BaudRatePrescaler = readReg(spi->CR1, SPI_CR1_BR);
    SpiHandle.Init.Direction         = readReg(spi->CR1, SPI_CR1_BIDIMODE); 
    SpiHandle.Init.CLKPhase          = readReg(spi->CR1, SPI_CR1_CPHA);
    SpiHandle.Init.CLKPolarity       = readReg(spi->CR1, SPI_CR1_CPOL);
    SpiHandle.Init.CRCCalculation    = readReg(spi->CR1, SPI_CR1_CRCEN);
    SpiHandle.Init.CRCPolynomial     = spi->CRCPR & 0xFFFF;
#if defined(TARGET_STM32F0)
    SpiHandle.Init.DataSize          = readReg(spi->CR2, SPI_CR2_DS);
#endif
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = readReg(spi->CR1, SPI_CR1_SSM);
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;
    SpiHandle.State                  = HAL_SPI_STATE_READY;
}

void spi_dma_handle_setup( SPI_TypeDef *spi, uint8_t mode )
{
	(void)spi;
	static uint8_t dma_handle_inited = 0;
	/* Peripheral DMA init*/
	/* TX: */
	if( !dma_handle_inited ){
		hdma_spi_tx.Instance                 = SPIx_TX_DMA_CHANNEL;
		hdma_spi_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
		hdma_spi_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
		hdma_spi_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		hdma_spi_tx.Init.Mode                = DMA_NORMAL;
		hdma_spi_tx.Init.Priority            = DMA_PRIORITY_HIGH;
	}
	hdma_spi_tx.Init.MemInc = ( mode == DMA_SPI_READ) ? DMA_MINC_DISABLE : DMA_MINC_ENABLE;
	HAL_DMA_Init(&hdma_spi_tx);
	__HAL_LINKDMA( &SpiHandle,hdmatx,hdma_spi_tx);
	/* RX: */
	if( !dma_handle_inited ){
		hdma_spi_rx.Instance                 = SPIx_RX_DMA_CHANNEL;
		hdma_spi_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
		hdma_spi_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
		hdma_spi_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		hdma_spi_rx.Init.Mode                = DMA_NORMAL;
		hdma_spi_rx.Init.Priority            = DMA_PRIORITY_LOW;
	}
	hdma_spi_rx.Init.MemInc = (mode == DMA_SPI_READ) ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;
	HAL_DMA_Init(&hdma_spi_rx);
	__HAL_LINKDMA( &SpiHandle,hdmarx,hdma_spi_rx);

	dma_handle_inited = 1;
}

void spi_dma_irq_setup( SPI_TypeDef *spi) 
{
	(void)spi;
	/* DMA controller clock enable */
	DMAx_CLK_ENABLE();

	/* DMA interrupt init */
	HAL_NVIC_SetPriority(SPIx_DMA_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(  SPIx_DMA_IRQn);
}

void DMA_SPI_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_rx);
  HAL_DMA_IRQHandler(&hdma_spi_tx);
}


#endif /* TARGET_STM32F0 */

