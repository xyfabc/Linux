/**
  ******************************************************************************
  * @file    platform_config.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Evaluation board specific configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Define the STM32F0xx hardware used from STM32072B_EVAL evaluation board */
#define SC_USART                           USART1
#define SC_USART_CLK                       RCC_APB2Periph_USART1
#define SC_USART_APBPERIPHCLOCK            RCC_APB2PeriphClockCmd
#define SC_USART_IRQn                      USART1_IRQn
#define SC_USART_IRQHandler                USART1_IRQHandler

#define SC_USART_TX_PIN                    GPIO_Pin_9                
#define SC_USART_TX_GPIO_PORT              GPIOA                       
#define SC_USART_TX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define SC_USART_TX_SOURCE                 GPIO_PinSource9
#define SC_USART_TX_AF                     GPIO_AF_1

#define SC_USART_CK_PIN                    GPIO_Pin_8                
#define SC_USART_CK_GPIO_PORT              GPIOA                    
#define SC_USART_CK_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define SC_USART_CK_SOURCE                 GPIO_PinSource8
#define SC_USART_CK_AF                     GPIO_AF_1


/* Smartcard Interface GPIO pins */
#define SC_3_5V_PIN                        GPIO_Pin_11
#define SC_3_5V_GPIO_PORT                  GPIOC
#define SC_3_5V_GPIO_CLK                   RCC_AHBPeriph_GPIOC

#define SC_RESET_PIN                       GPIO_Pin_10
#define SC_RESET_GPIO_PORT                 GPIOA
#define SC_RESET_GPIO_CLK                  RCC_AHBPeriph_GPIOA

#define SC_CMDVCC_PIN                      GPIO_Pin_2
#define SC_CMDVCC_GPIO_PORT                GPIOD
#define SC_CMDVCC_GPIO_CLK                 RCC_AHBPeriph_GPIOD

#define SC_OFF_PIN                         GPIO_Pin_12
#define SC_OFF_GPIO_PORT                   GPIOC
#define SC_OFF_GPIO_CLK                    RCC_AHBPeriph_GPIOC

#define SC_OFF_EXTI_LINE                   EXTI_Line12
#define SC_OFF_EXTI_PORT_SOURCE            EXTI_PortSourceGPIOC
#define SC_OFF_EXTI_PIN_SOURCE             EXTI_PinSource12

#define SC_OFF_EXTI_IRQn                   EXTI4_15_IRQn 
#define SC_OFF_EXTI_IRQHandler             EXTI4_15_IRQHandler

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
