/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
#include "rf_modules.h"
#include <stdlib.h> 
#include <string.h>
    
LightPacketStruct strLightPacket;
uint8_t  cLightRxDataCount = 0;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Channel3;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
#define PACKET_STX_LIGHT 0xab
#define LIGHT_DATA_SIZE 0x08            //s stx 제외
#define LIGHT_EX_SIZE 0x05
#define LIGHT_ETX     0x0d
void SendPacketLight(uint8_t id, uint8_t onoff, uint16_t dmsec, uint8_t percent)
{
  unsigned int crc;
  uint8_t Rfdata[20];
  uint8_t count = 0;
  
  Rfdata[count++] = PACKET_STX_LIGHT;                 //s  0xab

  Rfdata[count++] = LIGHT_DATA_SIZE;                     //s ETX 1, CRC 2
  Rfdata[count++] = id;         
  Rfdata[count++] = onoff;       
  Rfdata[count++] = (uint8_t)(dmsec >> 8);                     //s 디밍 2byte  
  Rfdata[count++] = (uint8_t)dmsec;
  Rfdata[count++] = percent;
       
  crc = PacketCRC16(&Rfdata[2], LIGHT_EX_SIZE);         //s onoff 부터 crc 계산
  
  Rfdata[count++] = (uint8_t)crc&0xFF;   
  Rfdata[count++] = (uint8_t)(crc>>8);   
  
  Rfdata[count++] = LIGHT_ETX;        
  //printf("count : %d \r\n",count);
  
  HAL_UART_Transmit(&huart1, Rfdata, count, 10000);  
}


uint8_t PorcessPacketLightCheck(uint8_t rx_data, uint8_t* buffdata)
{
  unsigned int crc = 0;
  uint8_t result = 0;
  
    //printf("rx[%02x]", rx_data);
    switch(strLightPacket.status)
    {
            case RXSTATUS_IDLE:
              if(rx_data == PACKET_STX_LIGHT){
                  strLightPacket.status = LIGHT_STATE_LEN; 
              }
              else{
                  PorcessPacketLightClear();                         
              }
              break;
            case LIGHT_STATE_LEN:
                strLightPacket.size = rx_data - 3;      //s crc 계산용                                                     
                strLightPacket.status = LIGHT_STATE_DATA;
                break;
            case LIGHT_STATE_DATA:
              if(strLightPacket.size > cLightRxDataCount){           //s 9byte data 수신
                strLightPacket.data[cLightRxDataCount] = rx_data;
                cLightRxDataCount++;
                //printf("RXSTATUS_DATA: %d\r\n", cLightRxDataCount);                          
                break;
              }
              else{
                    strLightPacket.status = LIGHT_STATE_CRC1;                //s break없이 case LIGHT_STATE_CRC1: 수행
              }
                    
              case LIGHT_STATE_CRC1:
                strLightPacket.status = LIGHT_STATE_CRC2;
                strLightPacket.crc_l = rx_data;
                cLightRxDataCount++;                            
                break;
                      
              case LIGHT_STATE_CRC2:			
                strLightPacket.status = LIGHT_STATE_ETX;                          
                strLightPacket.crc_h= rx_data;
                cLightRxDataCount++;    
                break;
                                    
              case LIGHT_STATE_ETX:	
                if(rx_data == LIGHT_ETX)
                {                       
                    uint16_t crcresult;
                    crc = (strLightPacket.crc_h<<8) | strLightPacket.crc_l;   
                    crcresult  = PacketCRC16(strLightPacket.data, strLightPacket.size);
                    cLightRxDataCount++;  
                    //CRC가 정상이면
                    if(crc == crcresult)						
                    {			                          
                        if(cLightRxDataCount == (strLightPacket.size + 3)){        //s 길이가 같은지 확인                                                                  
                            memcpy(buffdata, strLightPacket.data, strLightPacket.size);
                            result = LIGHT_SUB_ACK;
                                  
                            PorcessPacketLightClear();                                    
                            return result;
                        }
                        else{
                                //printf("cUartRxDataCount Erorr %d \r\n",  cUartRxDataCount);                                             
                                PorcessPacketLightClear();   
                                return result;
                        }
                    } 
                    else {
                            //CRC ERROR 
                          //crc = PacketCRC16(rx_packet_Rf_ch1.data, rx_packet_Rf_ch1.size);
                          //printf("RF CRC Erorr [%04x] [%04x]\r\n",  crc, crcresult);
                          PorcessPacketLightClear();                             
                    }
                  }
                  else{
                      //printf("RF ETX Erorr %x\r\n", rx_data);
                      PorcessPacketLightClear();                                  
                  }
              break;		
            default :
                      PorcessPacketLightClear();     
                break;
      
    }	
    return result;          


}


void PorcessPacketLightClear(void)
{
    cLightRxDataCount = 0;           
    memset(buffLightRxData, 0, sizeof(buffLightRxData));
    memset(&strLightPacket, 0, sizeof(strLightPacket));
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
