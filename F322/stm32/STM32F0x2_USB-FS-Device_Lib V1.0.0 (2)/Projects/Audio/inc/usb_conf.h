/**
  ******************************************************************************
  * @file    usb_conf.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   General low level driver configuration
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
#ifndef __USB_CONF__H__
#define __USB_CONF__H__

/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx.h"
#include "stm32072b_eval.h"
#include "stm32f0xx_dma.h"

/* Exported defines ----------------------------------------------------------*/
/* Select D+ pullup: internal or external */
#ifdef USE_STM32072B_EVAL
 /* When using STM32072B_EVAL board the infernal pullup must be enabled */
 #define INTERNAL_PULLUP
#endif

/* Define if Low power mode is enabled; it allows entering the device into STOP mode
    following USB Suspend event, and wakes up after the USB wakeup event is received. */
/* #define USB_DEVICE_LOW_PWR_MGMT_SUPPORT */

/* Configure the USB clock source as HSI48 with Clock Recovery System(CRS)*/
#define USB_CLOCK_SOURCE_CRS

#define EP_NUM     (2) /* EP0 + EP1 for Audio IN */

/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0, RX/TX buffers base address */
#define ENDP0_RX_ADDRESS        (0x10)
#define ENDP0_TX_ADDRESS        (0x50)

/* EP1, TX buffer base address */
#define Audio_IN_TX_ADRESS        (ENDP1_BUF0Addr | (ENDP1_BUF1Addr <<16) )

#define ENDP1_BUF0Addr      (0x90)
#define ENDP1_BUF1Addr      (0x154)

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __USB_CONF__H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
