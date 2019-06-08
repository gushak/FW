#include "spi_dma.h"
#include "PeripheralPins.h"

DMA_HandleTypeDef hdma_spi_tx;
DMA_HandleTypeDef hdma_spi_rx;
SPI_HandleTypeDef SpiHandle;

static void spi_dma_error_handler(void)
{
  while(1)
  { 
	/* infinite loop */
  }
}

static void spi_dma_start( SPI_TypeDef *spi, uint8_t *txbuff, uint8_t *rxbuff, uint16_t count)
{
	(void)spi;
	if( HAL_SPI_TransmitReceive_DMA( &SpiHandle, (uint8_t *)txbuff, (uint8_t *)rxbuff, count) != HAL_OK) {
		spi_dma_error_handler();
	}
}

static void spi_dma_wait_end( SPI_TypeDef *spi )
{
	(void)spi;
	while (HAL_SPI_GetState( &SpiHandle ) != HAL_SPI_STATE_READY) {
		/* just wait */ 
	} 
}

static void spi_dma_deinit( SPI_TypeDef *spi)
{
	(void)spi;
	HAL_DMA_DeInit( &hdma_spi_tx);
	HAL_DMA_DeInit( &hdma_spi_rx);
}

SPI_TypeDef *spi_get_id( PinName mosi, PinName miso, PinName sclk, PinName ssel) {
    // Determine the SPI to use
    SPIName spi_mosi = (SPIName)pinmap_peripheral(mosi, PinMap_SPI_MOSI);
    SPIName spi_miso = (SPIName)pinmap_peripheral(miso, PinMap_SPI_MISO);
    SPIName spi_sclk = (SPIName)pinmap_peripheral(sclk, PinMap_SPI_SCLK);
    SPIName spi_ssel = (SPIName)pinmap_peripheral(ssel, PinMap_SPI_SSEL);

    SPIName spi_data = (SPIName)pinmap_merge(spi_mosi, spi_miso);
    SPIName spi_cntl = (SPIName)pinmap_merge(spi_sclk, spi_ssel);

    return (SPI_TypeDef *) ( (SPIName)pinmap_merge(spi_data, spi_cntl) );
}

void spi_dma_init( SPI_TypeDef *spi ) {
	spi_dma_irq_setup( spi );
	spi_dma_get_info(  spi );
}

void __spi_dma_read_write( SPI_TypeDef *spi, uint8_t *txbuff, uint8_t *rxbuff, uint16_t count, uint8_t mode)
{
	spi_dma_handle_setup( spi, mode );	
	spi_dma_start( spi, txbuff, rxbuff, count );
	spi_dma_wait_end( spi );
	spi_dma_deinit(spi);
}

