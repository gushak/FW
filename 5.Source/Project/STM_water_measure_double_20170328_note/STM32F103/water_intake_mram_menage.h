#ifndef _WATER_IMTAKE_MRAM_MENAGE_H_
#define _WATER_IMTAKE_MRAM_MENAGE_H_

#define MRAM_SPI                       			SPI2
#define MRAM_SPI_CLK                   		RCC_APB1Periph_SPI2

#define MRAM_SPI_SCK_PIN               	GPIO_Pin_13
#define MRAM_SPI_SCK_GPIO_PORT      	GPIOB
#define MRAM_SPI_SCK_GPIO_CLK        	RCC_APB2Periph_GPIOB

#define MRAM_SPI_MISO_PIN             	GPIO_Pin_14
#define MRAM_SPI_MISO_GPIO_PORT  	GPIOB
#define MRAM_SPI_MISO_GPIO_CLK     	RCC_APB2Periph_GPIOB

#define MRAM_SPI_MOSI_PIN              	GPIO_Pin_15
#define MRAM_SPI_MOSI_GPIO_PORT    	GPIOB
#define MRAM_SPI_MOSI_GPIO_CLK       	RCC_APB2Periph_GPIOB

#define MRAM_CS_PIN                    		GPIO_Pin_12		//chip select
#define MRAM_CS_GPIO_PORT            	GPIOB
#define MRAM_CS_GPIO_CLK             	RCC_APB2Periph_GPIOB

#define MRAM_HOLD_PIN                 		GPIO_Pin_6
#define MRAM_HOLD_PORT             		GPIOC
#define MRAM_HOLD_CLK               		RCC_APB2Periph_GPIOC


uint16_t Intake_Head(void);
void Intake_head_increment();

void Intake_data_save(uint8_t *data, uint16_t address, uint16_t size);
extern volatile uint16_t Mram_intake_address[MRAM_INTAKE_MAX_ADDRESS];
extern void Intake_data_load(uint8_t *data, uint16_t address);
void Intake_page_all_enable();

#endif


