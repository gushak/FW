#if defined(TARGET_STM32F4) || defined(TARGET_STM32L4)
/* Mainly aim at 
	NUCLEO-F401RE 
	NUCLEO-F411RE 
	NUCLEO-F410RB 
	NUCLEO-F446RE 
*/

/*

This file is licensed under Apache 2.0 license.
(C) 2016 dinau

*/
#include "spi_dma.h"

#if defined(USBMSC_SDIO_SD)
#define SPI1_DMAx_CLK_ENABLE()    __DMA2_CLK_ENABLE()    
#define SPI1_DMAx_TX_STREAM       DMA2_Stream5
#define SPI1_DMAx_RX_STREAM       DMA2_Stream2
#define SPI1_DMAx_TX_CHANNEL      DMA_CHANNEL_3
#define SPI1_DMAx_RX_CHANNEL      DMA_CHANNEL_3
#define SPI1_DMAx_TX_IRQn         DMA2_Stream5_IRQn    
#define SPI1_DMAx_RX_IRQn         DMA2_Stream2_IRQn    
#define SPI1_DMAx_TX_IRQHandler   DMA2_Stream5_IRQHandler
#define SPI1_DMAx_RX_IRQHandler   DMA2_Stream2_IRQHandler
#else
/* For SPI1 */
#define SPI1_DMAx_CLK_ENABLE()    __DMA2_CLK_ENABLE()    
#define SPI1_DMAx_TX_STREAM       DMA2_Stream3
#define SPI1_DMAx_RX_STREAM       DMA2_Stream2
#define SPI1_DMAx_TX_CHANNEL      DMA_CHANNEL_3
#define SPI1_DMAx_RX_CHANNEL      DMA_CHANNEL_3
#define SPI1_DMAx_TX_IRQn         DMA2_Stream3_IRQn    
#define SPI1_DMAx_RX_IRQn         DMA2_Stream2_IRQn    
#define SPI1_DMAx_TX_IRQHandler   DMA2_Stream3_IRQHandler
#define SPI1_DMAx_RX_IRQHandler   DMA2_Stream2_IRQHandler
#endif
/* For SPI3 */
#define SPI3_DMAx_CLK_ENABLE()     __DMA1_CLK_ENABLE()    
#define SPI3_DMAx_TX_STREAM        DMA1_Stream5
#define SPI3_DMAx_RX_STREAM        DMA1_Stream2
#define SPI3_DMAx_TX_CHANNEL       DMA_CHANNEL_0
#define SPI3_DMAx_RX_CHANNEL       DMA_CHANNEL_0
#define SPI3_DMAx_TX_IRQn          DMA1_Stream5_IRQn    
#define SPI3_DMAx_RX_IRQn          DMA1_Stream2_IRQn    
#define SPI3_DMAx_TX_IRQHandler    DMA1_Stream5_IRQHandler
#define SPI3_DMAx_RX_IRQHandler    DMA1_Stream2_IRQHandler

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
    SpiHandle.Init.DataSize          = readReg(spi->CR1, SPI_CR1_DFF);
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = readReg(spi->CR1, SPI_CR1_SSM);
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLED;
    SpiHandle.State                  = HAL_SPI_STATE_READY;
}

void spi_dma_handle_setup( SPI_TypeDef *spi, uint8_t mode )
{
	static uint8_t dma_handle_inited = 0;
	/* Peripheral DMA init*/
	/* TX: */
	if( !dma_handle_inited ){
		if( spi == SPI1 ){
			hdma_spi_tx.Instance             = SPI1_DMAx_TX_STREAM;
			hdma_spi_tx.Init.Channel         = SPI1_DMAx_TX_CHANNEL; 
		} else if( spi == SPI3 ) {
			hdma_spi_tx.Instance             = SPI3_DMAx_TX_STREAM;
			hdma_spi_tx.Init.Channel         = SPI3_DMAx_TX_CHANNEL; 
		}
		hdma_spi_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
		hdma_spi_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
		hdma_spi_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		hdma_spi_tx.Init.Mode                = DMA_NORMAL;
		hdma_spi_tx.Init.Priority            = DMA_PRIORITY_HIGH;
		/**/
		hdma_spi_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
		hdma_spi_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
		hdma_spi_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
		hdma_spi_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;
		/**/
	}
	hdma_spi_tx.Init.MemInc = ( mode == DMA_SPI_READ) ? DMA_MINC_DISABLE : DMA_MINC_ENABLE;
	HAL_DMA_Init(&hdma_spi_tx);
	__HAL_LINKDMA( &SpiHandle,hdmatx,hdma_spi_tx);

	/* RX: */
	if( !dma_handle_inited ){        
		if( spi == SPI1 ){
			hdma_spi_rx.Instance             = SPI1_DMAx_RX_STREAM;
			hdma_spi_rx.Init.Channel         = SPI1_DMAx_RX_CHANNEL; 
		} else if( spi == SPI3 ) {
			hdma_spi_rx.Instance             = SPI3_DMAx_RX_STREAM;
			hdma_spi_rx.Init.Channel         = SPI3_DMAx_RX_CHANNEL; 
		}
		hdma_spi_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
		hdma_spi_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
		hdma_spi_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
		hdma_spi_rx.Init.Mode                = DMA_NORMAL;
		hdma_spi_rx.Init.Priority            = DMA_PRIORITY_LOW;
		/**/
		hdma_spi_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
		hdma_spi_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
		hdma_spi_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
		hdma_spi_rx.Init.PeriphBurst         = DMA_PBURST_SINGLE; 
		/**/
	}
	hdma_spi_rx.Init.MemInc = (mode == DMA_SPI_READ) ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;
	HAL_DMA_Init(&hdma_spi_rx);
	__HAL_LINKDMA( &SpiHandle,hdmarx,hdma_spi_rx);

	dma_handle_inited = 1;
}

void spi_dma_irq_setup( SPI_TypeDef *spi) 
{
	/* DMA controller clock enable */
	if( spi == SPI1 ){
		SPI1_DMAx_CLK_ENABLE();
		/* DMA interrupt init */
		HAL_NVIC_SetPriority(SPI1_DMAx_TX_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(  SPI1_DMAx_TX_IRQn);
		HAL_NVIC_SetPriority(SPI1_DMAx_RX_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(  SPI1_DMAx_RX_IRQn);
	} else if( spi == SPI3 ){
		SPI3_DMAx_CLK_ENABLE();
		/* DMA interrupt init */
		HAL_NVIC_SetPriority(SPI3_DMAx_TX_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(  SPI3_DMAx_TX_IRQn);
		HAL_NVIC_SetPriority(SPI3_DMAx_RX_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(  SPI3_DMAx_RX_IRQn);
	}
}

/* For SPI1 */
void SPI1_DMAx_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_tx);
}

void SPI1_DMAx_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_rx);
}

/* For SPI3 */
void SPI3_DMAx_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_tx);
}

void SPI3_DMAx_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_rx);
}


#endif /* TARGET_STM32F4 */

