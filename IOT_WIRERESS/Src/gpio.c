/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "sw_panel.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, RELAY1_Pin|RELAY2_Pin|RELAY3_Pin|RELAY5_Pin 
                          |RELAY6_Pin|RELAY7_Pin|RELAY4_Pin|RELAY16_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RELAY8_Pin|RELAY9_Pin|RELAY10_Pin|RELAY11_Pin 
                          |RELAY12_Pin|RELAY13_Pin|RELAY14_Pin|RELAY15_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IN_LOAD_GPIO_Port, IN_LOAD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, IN_CLK_Pin|LCD_E_Pin|LCD_D4_Pin|LCD_D5_Pin 
                          |LCD_D6_Pin|LCD_D7_Pin|OUT_LATCH_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OUT_MOSI_Pin|BUZ_Pin|OUT_CLK_Pin|OUT_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OUT_EN_GPIO_Port, OUT_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin 
                           PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = RELAY1_Pin|RELAY2_Pin|RELAY3_Pin|RELAY5_Pin 
                          |RELAY6_Pin|RELAY7_Pin|RELAY4_Pin|RELAY16_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin PAPin PAPin PAPin 
                           PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = RELAY8_Pin|RELAY9_Pin|RELAY10_Pin|RELAY11_Pin 
                          |RELAY12_Pin|RELAY13_Pin|RELAY14_Pin|RELAY15_Pin 
                          |OUT_MOSI_Pin|BUZ_Pin|OUT_CLK_Pin|OUT_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = IN_LOAD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IN_LOAD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = IN_MISO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IN_MISO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin 
                           PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = IN_CLK_Pin|LCD_E_Pin|LCD_D4_Pin|LCD_D5_Pin 
                          |LCD_D6_Pin|LCD_D7_Pin|OUT_LATCH_Pin|OUT_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin 
                           PBPin */
  GPIO_InitStruct.Pin = BOOT1_Pin|IN1_Pin|IN2_Pin|IN3_Pin 
                          |IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = IN5_Pin|IN6_Pin|IN7_Pin|IN8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin|SW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RS_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

uint8_t GpioRelayControlTask(Out_struct* relaydata, key_stat *keydata, uint8_t task)
{
  uint8_t ret = 0;
  if(task == RELAY_TASK_UPDATA){ //s relay time check and pin reset
    if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){
      //s relay 1
      if(relaydata->pin[0]){
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
      }        
      //s relay 2
      if(relaydata->pin[1]){
        HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
      } 
      //s relay 3
      if(relaydata->pin[2]){            //s 이송라인 역방향
        HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);      
      } 
      //s relay 4
      if(relaydata->pin[3]){
        HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
      } 
      //s relay 5
      if(relaydata->pin[4]){
        HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
      } 
      //s relay 6
      if(relaydata->pin[5]){
        HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
      } 
      //s relay 7
      if(relaydata->pin[6]){            //s 이송라인 2역방향
        HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);      
      } 
      //s relay 8
      if(relaydata->pin[7]){
        HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
      }
      //s relay 9
      if(relaydata->pin[8]){
            HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET);      
      }
      else{
            HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);      
      } 
      //s relay 10
      if(relaydata->pin[9]){
            HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET);      
      }
      else{
            HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);      
      } 
      //s relay 11
      if(relaydata->pin[10]){           // 이송라인 3 역방향
            HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);      
      }
      else{
            HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);      
      } 
      //s relay 12
      if(relaydata->pin[11]){
            HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_RESET);      
      }
      else{
            HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);      
      } 
      //s relay 13
      if(relaydata->pin[12]){
            HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_RESET);      
      }
      else{
            HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);      
      } 
      //s relay 14
      if(relaydata->pin[13]){
            HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_RESET);      
      }
      else{
            HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);      
      }       
      //s rleay port 15,16
      if(relaydata->pin[14] == SET){
        HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_RESET);
      }
      else if(relaydata->pin[15] == SET){
        HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_RESET);      
      }
      
      if(relaydata->pin[14] == RESET){
        HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);      
      }
      if(relaydata->pin[15] == RESET){
        HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);      
      }     
    }
    else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){      
      //s relay 1
      if(relaydata->pin[0]){
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
      }        
      //s relay 2
      if(relaydata->pin[1]){
        HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
      } 
      //s relay 3
      if(relaydata->pin[2]){
        HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
      } 
      //s relay 4
      if(relaydata->pin[3]){
        HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
      } 
      //s relay 5
      if(relaydata->pin[4]){
        HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
      } 
      //s relay 6
      if(relaydata->pin[5]){
        HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
      } 
      //s relay 7
      if(relaydata->pin[6]){
        HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
      } 
      //s relay 8
      if(relaydata->pin[7]){
        HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
      }  
    }    
    else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH){
      
      if(CONFIG_8CH_REVERSE){        
        //s rleay port 1,2
        if(relaydata->pin[0] == SET){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[1] == SET){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[0] == RESET){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[1] == RESET){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 3,4
        if(relaydata->pin[2] == SET){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[3] == SET){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[2] == RESET){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[3] == RESET){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 5,6
        if(relaydata->pin[4] == SET){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[5] == SET){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);      
        }

        if(relaydata->pin[4] == RESET){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[5] == RESET){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 7,8    
        if(relaydata->pin[6] == SET){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[7] == SET){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[6] == RESET){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[7] == RESET){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 9,10
        if(relaydata->pin[8] == SET){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[9] == SET){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[8] == RESET){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[9] == RESET){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 11,12
        if(relaydata->pin[10] == SET){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[11] == SET){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[10] == RESET){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[11] == RESET){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 13,14
        if(relaydata->pin[12] == SET){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[13] == SET){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[12] == RESET){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[13] == RESET){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 15,16
        if(relaydata->pin[14] == SET){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[15] == SET){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[14] == RESET){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[15] == RESET){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);      
        }
      }
      else{
        //s rleay port 1,2
        if(relaydata->pin[0] == SET){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[1] == SET){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[0] == RESET){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[1] == RESET){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 3,4
        if(relaydata->pin[2] == SET){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[3] == SET){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[2] == RESET){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[3] == RESET){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 5,6
        if(relaydata->pin[4] == SET){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[5] == SET){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);      
        }

        if(relaydata->pin[4] == RESET){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[5] == RESET){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 7,8    
        if(relaydata->pin[6] == SET){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[7] == SET){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[6] == RESET){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[7] == RESET){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 9,10
        if(relaydata->pin[8] == SET){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[9] == SET){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[8] == RESET){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[9] == RESET){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 11,12
        if(relaydata->pin[10] == SET){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[11] == SET){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[10] == RESET){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[11] == RESET){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 13,14
        if(relaydata->pin[12] == SET){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[13] == SET){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[12] == RESET){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[13] == RESET){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 15,16
        if(relaydata->pin[14] == SET){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[15] == SET){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[14] == RESET){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[15] == RESET){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);      
        }
      }
    }
    else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
      
      if(CONFIG_8CH_REVERSE){        
        //s rleay port 1,2
        if(relaydata->pin[0] == SET && relaydata->time[0] != 0){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[1] == SET && relaydata->time[1] != 0){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[0] == RESET){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[1] == RESET){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 3,4
        if(relaydata->pin[2] == SET && relaydata->time[2] != 0){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[3] == SET && relaydata->time[3] != 0){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[2] == RESET){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[3] == RESET){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 5,6
        if(relaydata->pin[4] == SET && relaydata->time[4] != 0){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[5] == SET && relaydata->time[5] != 0){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);      
        }

        if(relaydata->pin[4] == RESET){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[5] == RESET){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 7,8    
        if(relaydata->pin[6] == SET && relaydata->time[6] != 0){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[7] == SET && relaydata->time[7] != 0){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[6] == RESET){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[7] == RESET){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 9,10
        if(relaydata->pin[8] == SET && relaydata->time[9] != 0){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[9] == SET && relaydata->time[10] != 0){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[8] == RESET){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[9] == RESET){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 11,12
        if(relaydata->pin[10] == SET && relaydata->time[10] != 0){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[11] == SET && relaydata->time[11] != 0){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[10] == RESET){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[11] == RESET){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 13,14
        if(relaydata->pin[12] == SET && relaydata->time[12] != 0){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[13] == SET && relaydata->time[13] != 0){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[12] == RESET){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[13] == RESET){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 15,16
        if(relaydata->pin[14] == SET && relaydata->time[14] != 0){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[15] == SET && relaydata->time[15] != 0){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[14] == RESET){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[15] == RESET){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);      
        }
      }
      else{
        //s rleay port 1,2
        if(relaydata->pin[0] == SET && relaydata->time[0] != 0){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[1] == SET && relaydata->time[1] != 0){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[0] == RESET){
          HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[1] == RESET){
          HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 3,4
        if(relaydata->pin[2] == SET && relaydata->time[2] != 0){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[3] == SET && relaydata->time[3] != 0){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[2] == RESET){
          HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[3] == RESET){
          HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 5,6
        if(relaydata->pin[4] == SET && relaydata->time[4] != 0){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[5] == SET && relaydata->time[5] != 0){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);      
        }

        if(relaydata->pin[4] == RESET){
          HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[5] == RESET){
          HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 7,8    
        if(relaydata->pin[6] == SET && relaydata->time[6] != 0){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[7] == SET && relaydata->time[7] != 0){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[6] == RESET){
          HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[7] == RESET){
          HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 9,10
        if(relaydata->pin[8] == SET && relaydata->time[8] != 0){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[9] == SET && relaydata->time[9] != 0){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[8] == RESET){
          HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[9] == RESET){
          HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 11,12
        if(relaydata->pin[10] == SET && relaydata->time[10] != 0){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[11] == SET && relaydata->time[11] != 0){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[10] == RESET){
          HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[11] == RESET){
          HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);      
        }    
        //s rleay port 13,14
        if(relaydata->pin[12] == SET && relaydata->time[12] != 0){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[13] == SET && relaydata->time[13] != 0){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[12] == RESET){
          HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[13] == RESET){
          HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);      
        }        
        //s rleay port 15,16
        if(relaydata->pin[14] == SET && relaydata->time[14] != 0){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_RESET);
        }
        else if(relaydata->pin[15] == SET && relaydata->time[15] != 0){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_RESET);      
        }
        
        if(relaydata->pin[14] == RESET){
          HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);      
        }
        if(relaydata->pin[15] == RESET){
          HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);      
        }
      }
    }    
    else{
      //s relay 1
      if(relaydata->pin[0]){
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);      
      }        
      //s relay 2
      if(relaydata->pin[1]){
        HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);      
      } 
      //s relay 3
      if(relaydata->pin[2]){
        HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);      
      } 
      //s relay 4
      if(relaydata->pin[3]){
        HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);      
      } 
      //s relay 5
      if(relaydata->pin[4]){
        HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);      
      } 
      //s relay 6
      if(relaydata->pin[5]){
        HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);      
      } 
      //s relay 7
      if(relaydata->pin[6]){
        HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);      
      } 
      //s relay 8
      if(relaydata->pin[7]){
        HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_RESET);      
      }
      else{
        HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);      
      }
      //s relay 9
		if(CONFIG_DEVIEC_TPYE != DEVICE_TYPE_CONTROL){	  
		  if(relaydata->pin[8]){
			HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 10
		  if(relaydata->pin[9]){
			HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 11
		  if(relaydata->pin[10]){
			HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 12
		  if(relaydata->pin[11]){
			HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 13
		  if(relaydata->pin[12]){
			HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 14
		  if(relaydata->pin[13]){
			HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 15
		  if(relaydata->pin[14]){
			HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);      
		  } 
		  //s relay 16
		  if(relaydata->pin[15]){
			HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_RESET);      
		  }
		  else{
			HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);      
		  }      
		}
    }
      
  }
  else if(task == RELAY_DATA_TIMER_1S){ //s relay time check and pin reset
	if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING){    //s RELAY 1
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_RESET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_RESET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 9 
		if(relaydata->time[8] != 0 && relaydata->pin[8] == SET){
		  relaydata->time[8]--;
		  if(relaydata->time[8] == 0)
		  {
			relaydata->pin[8] = RESET;
			HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);  
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 10 
		if(relaydata->time[9] != 0 && relaydata->pin[9] == SET){
		  relaydata->time[9]--;
		  if(relaydata->time[9] == 0)
		  {
			relaydata->pin[9] = RESET;
			HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET); 
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 11 
		if(relaydata->time[10] != 0 && relaydata->pin[10] == SET){
		  relaydata->time[10]--;
		  if(relaydata->time[10] == 0)
		  {
			relaydata->pin[10] = RESET;
			HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);  
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_RESET);           
		}    
		//s RELAY 12 
		if(relaydata->time[11] != 0 && relaydata->pin[11] == SET){
		  relaydata->time[11]--;
		  if(relaydata->time[11] == 0)
		  {
			relaydata->pin[11] = RESET;
			HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 13 
		if(relaydata->time[12] != 0 && relaydata->pin[12] == SET){
		  relaydata->time[12]--;
		  if(relaydata->time[12] == 0)
		  {
			relaydata->pin[12] = RESET;
			HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 14 
		if(relaydata->time[13] != 0 && relaydata->pin[13] == SET){
		  relaydata->time[13]--;
		  if(relaydata->time[13] == 0)
		  {
			relaydata->pin[13] = RESET;
			HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 15 
		if(relaydata->time[14] != 0 && relaydata->pin[14] == SET){
		  relaydata->time[14]--;
		  if(relaydata->time[14] == 0)
		  {
			relaydata->pin[14] = RESET;
			HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);         
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);           
		}        
		//s RELAY 16 
		if(relaydata->time[15] != 0 && relaydata->pin[15] == SET){
		  relaydata->time[15]--;
		  if(relaydata->time[15] == 0)
		  {
			relaydata->pin[15] = RESET;
			HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);            
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);           
		}    
	}
	else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_PENDING_8CH){    
                //s RELAY 1
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}     
	}
        else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH){
          if(CONFIG_8CH_REVERSE){  
		//s RELAY 1            
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 9 
                if(relaydata->time[8] != 0 && relaydata->pin[8] == SET){
                  relaydata->time[8]--;
                  if(relaydata->time[8] == 0)
                  {
                        relaydata->pin[8] = RESET;
                        HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 10 
                if(relaydata->time[9] != 0 && relaydata->pin[9] == SET){
                  relaydata->time[9]--;
                  if(relaydata->time[9] == 0)
                  {
                        relaydata->pin[9] = RESET;
                        HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET); 
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 11 
                if(relaydata->time[10] != 0 && relaydata->pin[10] == SET){
                  relaydata->time[10]--;
                  if(relaydata->time[10] == 0)
                  {
                        relaydata->pin[10] = RESET;
                        HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 12 
                if(relaydata->time[11] != 0 && relaydata->pin[11] == SET){
                  relaydata->time[11]--;
                  if(relaydata->time[11] == 0)
                  {
                        relaydata->pin[11] = RESET;
                        HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);    
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 13 
                if(relaydata->time[12] != 0 && relaydata->pin[12] == SET){
                  relaydata->time[12]--;
                  if(relaydata->time[12] == 0)
                  {
                        relaydata->pin[12] = RESET;
                        HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 14 
                if(relaydata->time[13] != 0 && relaydata->pin[13] == SET){
                  relaydata->time[13]--;
                  if(relaydata->time[13] == 0)
                  {
                        relaydata->pin[13] = RESET;
                        HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 15 
                if(relaydata->time[14] != 0 && relaydata->pin[14] == SET){
                  relaydata->time[14]--;
                  if(relaydata->time[14] == 0)
                  {
                        relaydata->pin[14] = RESET;
                        HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);         
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);           
                }        
                //s RELAY 16 
                if(relaydata->time[15] != 0 && relaydata->pin[15] == SET){
                  relaydata->time[15]--;
                  if(relaydata->time[15] == 0)
                  {
                        relaydata->pin[15] = RESET;
                        HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);            
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);           
                }   
                
            }
            else{
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 9 
                if(relaydata->time[8] != 0 && relaydata->pin[8] == SET){
                  relaydata->time[8]--;
                  if(relaydata->time[8] == 0)
                  {
                        relaydata->pin[8] = RESET;
                        HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 10 
                if(relaydata->time[9] != 0 && relaydata->pin[9] == SET){
                  relaydata->time[9]--;
                  if(relaydata->time[9] == 0)
                  {
                        relaydata->pin[9] = RESET;
                        HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET); 
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 11 
                if(relaydata->time[10] != 0 && relaydata->pin[10] == SET){
                  relaydata->time[10]--;
                  if(relaydata->time[10] == 0)
                  {
                        relaydata->pin[10] = RESET;
                        HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 12 
                if(relaydata->time[11] != 0 && relaydata->pin[11] == SET){
                  relaydata->time[11]--;
                  if(relaydata->time[11] == 0)
                  {
                        relaydata->pin[11] = RESET;
                        HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);    
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 13 
                if(relaydata->time[12] != 0 && relaydata->pin[12] == SET){
                  relaydata->time[12]--;
                  if(relaydata->time[12] == 0)
                  {
                        relaydata->pin[12] = RESET;
                        HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 14 
                if(relaydata->time[13] != 0 && relaydata->pin[13] == SET){
                  relaydata->time[13]--;
                  if(relaydata->time[13] == 0)
                  {
                        relaydata->pin[13] = RESET;
                        HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 15 
                if(relaydata->time[14] != 0 && relaydata->pin[14] == SET){
                  relaydata->time[14]--;
                  if(relaydata->time[14] == 0)
                  {
                        relaydata->pin[14] = RESET;
                        HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);         
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);           
                }        
                //s RELAY 16 
                if(relaydata->time[15] != 0 && relaydata->pin[15] == SET){
                  relaydata->time[15]--;
                  if(relaydata->time[15] == 0)
                  {
                        relaydata->pin[15] = RESET;
                        HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);            
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);           
                }                 
            }

        }
        else if(CONFIG_DEVIEC_TPYE == DEVICE_TYPE_8CH_LIMIT){
          if(CONFIG_8CH_REVERSE){  
		//s RELAY 1            
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			//relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			//relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			//relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			//relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			//relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			//relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			//relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			//relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 9 
                if(relaydata->time[8] != 0 && relaydata->pin[8] == SET){
                  relaydata->time[8]--;
                  if(relaydata->time[8] == 0)
                  {
                        //relaydata->pin[8] = RESET;
                        HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 10 
                if(relaydata->time[9] != 0 && relaydata->pin[9] == SET){
                  relaydata->time[9]--;
                  if(relaydata->time[9] == 0)
                  {
                        //relaydata->pin[9] = RESET;
                        HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET); 
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 11 
                if(relaydata->time[10] != 0 && relaydata->pin[10] == SET){
                  relaydata->time[10]--;
                  if(relaydata->time[10] == 0)
                  {
                       // relaydata->pin[10] = RESET;
                        HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 12 
                if(relaydata->time[11] != 0 && relaydata->pin[11] == SET){
                  relaydata->time[11]--;
                  if(relaydata->time[11] == 0)
                  {
                        //relaydata->pin[11] = RESET;
                        HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);    
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 13 
                if(relaydata->time[12] != 0 && relaydata->pin[12] == SET){
                  relaydata->time[12]--;
                  if(relaydata->time[12] == 0)
                  {
                        //relaydata->pin[12] = RESET;
                        HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 14 
                if(relaydata->time[13] != 0 && relaydata->pin[13] == SET){
                  relaydata->time[13]--;
                  if(relaydata->time[13] == 0)
                  {
                        //relaydata->pin[13] = RESET;
                        HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 15 
                if(relaydata->time[14] != 0 && relaydata->pin[14] == SET){
                  relaydata->time[14]--;
                  if(relaydata->time[14] == 0)
                  {
                        //relaydata->pin[14] = RESET;
                        HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);         
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);           
                }        
                //s RELAY 16 
                if(relaydata->time[15] != 0 && relaydata->pin[15] == SET){
                  relaydata->time[15]--;
                  if(relaydata->time[15] == 0)
                  {
                        //relaydata->pin[15] = RESET;
                        HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);            
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);           
                }   
                
            }
            else{
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			//relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			//relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			//relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			//relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			//relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			//relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			//relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			//relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 9 
                if(relaydata->time[8] != 0 && relaydata->pin[8] == SET){
                  relaydata->time[8]--;
                  if(relaydata->time[8] == 0)
                  {
                        //relaydata->pin[8] = RESET;
                        HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 10 
                if(relaydata->time[9] != 0 && relaydata->pin[9] == SET){
                  relaydata->time[9]--;
                  if(relaydata->time[9] == 0)
                  {
                        //relaydata->pin[9] = RESET;
                        HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET); 
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 11 
                if(relaydata->time[10] != 0 && relaydata->pin[10] == SET){
                  relaydata->time[10]--;
                  if(relaydata->time[10] == 0)
                  {
                        //relaydata->pin[10] = RESET;
                        HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);  
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 12 
                if(relaydata->time[11] != 0 && relaydata->pin[11] == SET){
                  relaydata->time[11]--;
                  if(relaydata->time[11] == 0)
                  {
                        //relaydata->pin[11] = RESET;
                        HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);    
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 13 
                if(relaydata->time[12] != 0 && relaydata->pin[12] == SET){
                  relaydata->time[12]--;
                  if(relaydata->time[12] == 0)
                  {
                        //relaydata->pin[12] = RESET;
                        HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 14 
                if(relaydata->time[13] != 0 && relaydata->pin[13] == SET){
                  relaydata->time[13]--;
                  if(relaydata->time[13] == 0)
                  {
                        //relaydata->pin[13] = RESET;
                        HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);           
                }    
                //s RELAY 15 
                if(relaydata->time[14] != 0 && relaydata->pin[14] == SET){
                  relaydata->time[14]--;
                  if(relaydata->time[14] == 0)
                  {
                        //relaydata->pin[14] = RESET;
                        HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);         
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);           
                }        
                //s RELAY 16 
                if(relaydata->time[15] != 0 && relaydata->pin[15] == SET){
                  relaydata->time[15]--;
                  if(relaydata->time[15] == 0)
                  {
                        //relaydata->pin[15] = RESET;
                        HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);            
                        ret = SET;        
                  }
                }
                else{
                  HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);           
                }                 
            }

        }        
	else{    //s RELAY 1
		if(relaydata->time[0] != 0 && relaydata->pin[0] == SET){
		  relaydata->time[0]--;
		  if(relaydata->time[0] == 0)
		  {
			relaydata->pin[0] = RESET;
			HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY1_GPIO_Port, RELAY1_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 2
		if(relaydata->time[1] != 0 && relaydata->pin[1] == SET){
		  relaydata->time[1]--;
		  if(relaydata->time[1] == 0)
		  {
			relaydata->pin[1] = RESET;
			HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY2_GPIO_Port, RELAY2_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 3
		if(relaydata->time[2] != 0 && relaydata->pin[2] == SET){
		  relaydata->time[2]--;
		  if(relaydata->time[2] == 0)
		  {
			relaydata->pin[2] = RESET;
			HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY3_GPIO_Port, RELAY3_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 4 
		if(relaydata->time[3] != 0 && relaydata->pin[3] == SET){
		  relaydata->time[3]--;
		  if(relaydata->time[3] == 0)
		  {
			relaydata->pin[3] = RESET;
			HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY4_GPIO_Port, RELAY4_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 5 
		if(relaydata->time[4] != 0 && relaydata->pin[4] == SET){
		  relaydata->time[4]--;
		  if(relaydata->time[4] == 0)
		  {
			relaydata->pin[4] = RESET;
			HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);    
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY5_GPIO_Port, RELAY5_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 6 
		if(relaydata->time[5] != 0 && relaydata->pin[5] == SET){
		  relaydata->time[5]--;
		  if(relaydata->time[5] == 0)
		  {
			relaydata->pin[5] = RESET;
			HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);   
			ret = SET;
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY6_GPIO_Port, RELAY6_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 7 
		if(relaydata->time[6] != 0 && relaydata->pin[6] == SET){
		  relaydata->time[6]--;
		  if(relaydata->time[6] == 0)
		  {
			relaydata->pin[6] = RESET;
			HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);   
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY7_GPIO_Port, RELAY7_Pin, GPIO_PIN_SET);           
		}    
		//s RELAY 8 
		if(relaydata->time[7] != 0 && relaydata->pin[7] == SET){
		  relaydata->time[7]--;
		  if(relaydata->time[7] == 0)
		  {
			relaydata->pin[7] = RESET;
			HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);        
			ret = SET;        
		  }
		}
		else{
		  HAL_GPIO_WritePin(RELAY8_GPIO_Port, RELAY8_Pin, GPIO_PIN_SET);           
		}
		//s RELAY 9 
		if(CONFIG_DEVIEC_TPYE != DEVICE_TYPE_CONTROL){
			if(relaydata->time[8] != 0 && relaydata->pin[8] == SET){
			  relaydata->time[8]--;
			  if(relaydata->time[8] == 0)
			  {
				relaydata->pin[8] = RESET;
				HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);  
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY9_GPIO_Port, RELAY9_Pin, GPIO_PIN_SET);           
			}    
			//s RELAY 10 
			if(relaydata->time[9] != 0 && relaydata->pin[9] == SET){
			  relaydata->time[9]--;
			  if(relaydata->time[9] == 0)
			  {
				relaydata->pin[9] = RESET;
				HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET); 
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY10_GPIO_Port, RELAY10_Pin, GPIO_PIN_SET);           
			}    
			//s RELAY 11 
			if(relaydata->time[10] != 0 && relaydata->pin[10] == SET){
			  relaydata->time[10]--;
			  if(relaydata->time[10] == 0)
			  {
				relaydata->pin[10] = RESET;
				HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);  
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY11_GPIO_Port, RELAY11_Pin, GPIO_PIN_SET);           
			}    
			//s RELAY 12 
			if(relaydata->time[11] != 0 && relaydata->pin[11] == SET){
			  relaydata->time[11]--;
			  if(relaydata->time[11] == 0)
			  {
				relaydata->pin[11] = RESET;
				HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);    
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY12_GPIO_Port, RELAY12_Pin, GPIO_PIN_SET);           
			}    
			//s RELAY 13 
			if(relaydata->time[12] != 0 && relaydata->pin[12] == SET){
			  relaydata->time[12]--;
			  if(relaydata->time[12] == 0)
			  {
				relaydata->pin[12] = RESET;
				HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY13_GPIO_Port, RELAY13_Pin, GPIO_PIN_SET);           
			}    
			//s RELAY 14 
			if(relaydata->time[13] != 0 && relaydata->pin[13] == SET){
			  relaydata->time[13]--;
			  if(relaydata->time[13] == 0)
			  {
				relaydata->pin[13] = RESET;
				HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY14_GPIO_Port, RELAY14_Pin, GPIO_PIN_SET);           
			}    
			//s RELAY 15 
			if(relaydata->time[14] != 0 && relaydata->pin[14] == SET){
			  relaydata->time[14]--;
			  if(relaydata->time[14] == 0)
			  {
				relaydata->pin[14] = RESET;
				HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);         
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY15_GPIO_Port, RELAY15_Pin, GPIO_PIN_SET);           
			}        
			//s RELAY 16 
			if(relaydata->time[15] != 0 && relaydata->pin[15] == SET){
			  relaydata->time[15]--;
			  if(relaydata->time[15] == 0)
			  {
				relaydata->pin[15] = RESET;
				HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);            
				ret = SET;        
			  }
			}
			else{
			  HAL_GPIO_WritePin(RELAY16_GPIO_Port, RELAY16_Pin, GPIO_PIN_SET);           
			}   
		}
	}	
  }
  return ret;    
  
}

void GpioInputTask(InputsStruct *indata)
{
  uint8_t i;
//  uint8_t temp;
  
          
  if(CONFIG_MOVING_COUNT >= 1){          
    if(HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin))
    {
            indata->Input[1].Key = RESET;
    }
    else{
            indata->Input[1].Key = SET;	  
    }
  }
  else{
    indata->Input[1].Key = SET;	  
  }  
//  indata->Input[1].Key = HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin);
  
  if(CONFIG_MOVING_COUNT >= 2){      

    if(HAL_GPIO_ReadPin(IN4_GPIO_Port, IN4_Pin))
    {
            indata->Input[3].Key = RESET;
    }
    else{
            indata->Input[3].Key = SET;	  
    }
  }
  else{
    indata->Input[3].Key = SET;	  
  }  
//  indata->Input[3].Key = HAL_GPIO_ReadPin(IN4_GPIO_Port, IN4_Pin);

  if(CONFIG_MOVING_COUNT >= 3){
    if(HAL_GPIO_ReadPin(IN6_GPIO_Port, IN6_Pin))
    {
            indata->Input[5].Key = RESET;
    }
    else{
            indata->Input[5].Key = SET;	  
    }  
  }
  else{
    indata->Input[5].Key = SET;	  
  }
    
  indata->Input[0].Key = HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin);  
  indata->Input[2].Key = HAL_GPIO_ReadPin(IN3_GPIO_Port, IN3_Pin); 
  indata->Input[4].Key = HAL_GPIO_ReadPin(IN5_GPIO_Port, IN5_Pin);  
    
//  indata->Input[5].Key = HAL_GPIO_ReadPin(IN6_GPIO_Port, IN6_Pin);    
  indata->Input[6].Key = HAL_GPIO_ReadPin(IN7_GPIO_Port, IN7_Pin);
  indata->Input[7].Key = HAL_GPIO_ReadPin(IN8_GPIO_Port, IN8_Pin);
  
  
  
  for(i = 0; i < 8; i++)
  {
    if(indata->Input[i].Key == RESET && indata->Input[i].OldKey == RESET){
      indata->Input[i].Time = 0;
      indata->Input[i].OldKey = SET;
      indata->KeyData |= 0x01 << i;                          //s 계류식 task에 사용될 input값 생성
      indata->Event |= 0x01 << i;      
    }
    else if(indata->Input[i].Key == RESET && indata->Input[i].OldKey == SET)
    {
      indata->Input[i].Time++;    
    }
    else if(indata->Input[i].Key == SET && indata->Input[i].OldKey == SET)
    {
      indata->KeyData &= ~(0x01 << i);        
      indata->Event |= 0x01 << i;
      indata->Input[i].OldKey = RESET;
    }    
  } 
}




/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
