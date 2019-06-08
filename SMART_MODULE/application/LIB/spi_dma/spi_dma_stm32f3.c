#if defined(TARGET_STM32F3)

/*

This file is licensed under Apache 2.0 license.
(C) 2016 dinau

*/
#include "spi_dma.h"

/* For SPI1 */
#if defined(SPI1_BASE)
#define SPI1_DMAx_CLK_ENABLE()     __DMA1_CLK_ENABLE()    
#define SPI1_DMAx_TX_CHANNEL       DMA1_Channel3
#define SPI1_DMAx_RX_CHANNEL       DMA1_Channel2
#define SPI1_DMAx_TX_IRQn          DMA1_Channel3_IRQn    
#define SPI1_DMAx_RX_IRQn          DMA1_Channel2_IRQn    
#define SPI1_DMAx_TX_IRQHandler    DMA1_Channel3_IRQHandler
#define SPI1_DMAx_RX_IRQHandler    DMA1_Channel2_IRQHandler
#endif

/* For SPI2 */
#if defined(SPI2_BASE)
#define SPI2_DMAx_CLK_ENABLE()     __DMA1_CLK_ENABLE()    
#define SPI2_DMAx_TX_CHANNEL       DMA1_Channel5
#define SPI2_DMAx_RX_CHANNEL       DMA1_Channel4
#define SPI2_DMAx_TX_IRQn          DMA1_Channel5_IRQn    
#define SPI2_DMAx_RX_IRQn          DMA1_Channel4_IRQn    
#define SPI2_DMAx_TX_IRQHandler    DMA1_Channel5_IRQHandler
#define SPI2_DMAx_RX_IRQHandler    DMA1_Channel4_IRQHandler
#endif

/* For SPI3 */
#if defined(SPI3_BASE)
#if defined(TARGET_STM32F302R8)
#define SPI3_DMAx_CLK_ENABLE()     __DMA1_CLK_ENABLE()    
#define SPI3_DMAx_TX_CHANNEL       DMA1_Channel3
#define SPI3_DMAx_RX_CHANNEL       DMA1_Channel2
#define SPI3_DMAx_TX_IRQn          DMA1_Channel3_IRQn    
#define SPI3_DMAx_RX_IRQn          DMA1_Channel2_IRQn    
#define SPI3_DMAx_TX_IRQHandler    DMA1_Channel3_IRQHandler
#define SPI3_DMAx_RX_IRQHandler    DMA1_Channel2_IRQHandler
#else
#define SPI3_DMAx_CLK_ENABLE()     __DMA2_CLK_ENABLE()    
#define SPI3_DMAx_TX_CHANNEL       DMA2_Channel2
#define SPI3_DMAx_RX_CHANNEL       DMA2_Channel1
#define SPI3_DMAx_TX_IRQn          DMA2_Channel2_IRQn    
#define SPI3_DMAx_RX_IRQn          DMA2_Channel1_IRQn    
#define SPI3_DMAx_TX_IRQHandler    DMA2_Channel2_IRQHandler
#define SPI3_DMAx_RX_IRQHandler    DMA2_Channel1_IRQHandler
#endif
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
    SpiHandle.Init.DataSize          = readReg(spi->CR1, SPI_CR2_DS);
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
#if defined(SPI1_BASE)
		if( spi == SPI1 ){
			hdma_spi_tx.Instance             = SPI1_DMAx_TX_CHANNEL;
		} 
#endif
#if defined(SPI2_BASE)
        if( spi == SPI2 ){                           
			hdma_spi_tx.Instance             = SPI2_DMAx_TX_CHANNEL;
		} 
#endif
#if defined(SPI3_BASE)
        if( spi == SPI3 ){                           
			hdma_spi_tx.Instance             = SPI3_DMAx_TX_CHANNEL;
		}
#endif
        hdma_spi_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_spi_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_spi_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_spi_tx.Init.Mode                = DMA_NORMAL;
        hdma_spi_tx.Init.Priority            = DMA_PRIORITY_LOW;
    }
	hdma_spi_tx.Init.MemInc = ( mode == DMA_SPI_READ) ? DMA_MINC_DISABLE : DMA_MINC_ENABLE;
	HAL_DMA_Init(&hdma_spi_tx);
	__HAL_LINKDMA( &SpiHandle,hdmatx,hdma_spi_tx);
	/* RX: */
    if( !dma_handle_inited ){
#if defined(SPI1_BASE)
		if( spi == SPI1 ){
			hdma_spi_rx.Instance             = SPI1_DMAx_RX_CHANNEL;
		} 
#endif
#if defined(SPI2_BASE)
        if( spi == SPI2 ){                           
			hdma_spi_rx.Instance             = SPI2_DMAx_RX_CHANNEL;
		} 
#endif
#if defined(SPI3_BASE)
        if( spi == SPI3 ){                           
			hdma_spi_rx.Instance             = SPI3_DMAx_RX_CHANNEL;
		}
#endif
        hdma_spi_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_spi_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_spi_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_spi_rx.Init.Mode                = DMA_NORMAL;
        hdma_spi_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    } 
    hdma_spi_rx.Init.MemInc = (mode == DMA_SPI_READ) ? DMA_MINC_ENABLE : DMA_MINC_DISABLE;
	HAL_DMA_Init(&hdma_spi_rx);
	__HAL_LINKDMA( &SpiHandle,hdmarx,hdma_spi_rx);
}

void spi_dma_irq_setup( SPI_TypeDef *spi) 
{
    /* DMA controller clock enable */
#if defined(SPI1_BASE)
	if(  spi == SPI1 ){
		SPI1_DMAx_CLK_ENABLE();
		HAL_NVIC_SetPriority(SPI1_DMAx_TX_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(  SPI1_DMAx_TX_IRQn);
		HAL_NVIC_SetPriority(SPI1_DMAx_RX_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(  SPI1_DMAx_RX_IRQn);
	} 
#endif
#if defined(SPI2_BASE)
    if( spi == SPI2 ){
		SPI2_DMAx_CLK_ENABLE();
		HAL_NVIC_SetPriority(SPI2_DMAx_TX_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(  SPI2_DMAx_TX_IRQn);
		HAL_NVIC_SetPriority(SPI2_DMAx_RX_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(  SPI2_DMAx_RX_IRQn);
	}
#endif
#if defined(SPI3_BASE)
    if( spi == SPI3 ){
		SPI3_DMAx_CLK_ENABLE();
		HAL_NVIC_SetPriority(SPI3_DMAx_TX_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(  SPI3_DMAx_TX_IRQn);
		HAL_NVIC_SetPriority(SPI3_DMAx_RX_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(  SPI3_DMAx_RX_IRQn);
	}
#endif
    /* DMA interrupt init */
}

/* SPI1 */
#if defined(SPI1_BASE)
void SPI1_DMAx_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_tx);
}
void SPI1_DMAx_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_rx);
}
#endif

#if defined(SPI2_BASE)
/* SPI2 */
void SPI2_DMAx_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_tx);
}
void SPI2_DMAx_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_rx);
}
#endif

/* SPI3 */
#if defined(SPI3_BASE)
void SPI3_DMAx_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_tx);
}
void SPI3_DMAx_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi_rx);
}
#endif

#endif /* TARGET_STM32F3 */

