/*

This file is licensed under Apache 2.0 license.
(C) 2016 dinau

*/
#ifndef SPI_DMA_H_INCLUDED
#define SPI_DMA_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

#include "mbed_assert.h"
#include "spi_api.h"

#define DMA_SPI_READ				0
#define DMA_SPI_WRITE				1
void __spi_dma_read_write( SPI_TypeDef *spi, uint8_t *txbuff, uint8_t *rxbuff, uint16_t count, uint8_t mode);
#define spi_dma_read(pspi,txbuff,rxbuff,count )\
	  __spi_dma_read_write( pspi,txbuff,rxbuff,count,DMA_SPI_READ)
#define spi_dma_write(pspi,txbuff,rxbuff,count )\
	  __spi_dma_read_write( pspi,txbuff,rxbuff,count,DMA_SPI_WRITE)





extern DMA_HandleTypeDef hdma_spi_tx;
extern DMA_HandleTypeDef hdma_spi_rx;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern DMA_HandleTypeDef hdma_spi3_rx;
extern SPI_HandleTypeDef SpiHandle;
extern SPI_HandleTypeDef Spi3Handle;


void spi_dma_init( SPI_TypeDef *spi );
void spi_dma_irq_setup( SPI_TypeDef *spi);
void spi_dma_get_info(  SPI_TypeDef *spi );
void spi_dma_handle_setup( SPI_TypeDef *spi, uint8_t mode );
SPI_TypeDef *spi_get_id(PinName mosi, PinName miso, PinName sclk, PinName ssel);


#ifdef __cplusplus
}
#endif

#endif   /* SPI_DMA_H_INCLUDED */

