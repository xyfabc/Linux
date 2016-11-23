/**
  ******************************************************************************
  * @file    audio_app_conf.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   USB Device configuration file
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
#ifndef __AUDIO_APP_CONF__H__
#define __AUDIO_APP_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

#ifdef __WAV_ADDON__
 #include "wavaddon.h"
#endif /* __WAV_ADDON__ */

/* Exported defines ----------------------------------------------------------*/
/*-----------------------------------------------------------------------------
   Defines from usbd_conf.h file 
------------------------------------------------------------------------------*/

/* Default audio frequency in Hz. If SUPPORTED_FREQ_NBR > 1, make sure that the default frequency is
   always the higher one in all supported frequencies (because the allocation of data buffer is done 
   based on this frequency). */
#define DEFAULT_OUT_AUDIO_FREQ                     48000 

/* Number of supported audio frequencies (Min = 1, Max = 4).
   Note: when supported number of frequencies is > 1 and the Feedback synchronization method is
   used (AUDIO_SYNCHRO_FEEDBACK_ENABLED) then the behavior of the host may be unpredictable:
   In some cases, the host may use the dynamic audio freq switching while playing file which
   may lead (when switching frequency is too high) to glitches on audio output (due to delay
   caused by the dynamic clock switching). */
#define SUPPORTED_FREQ_NBR                         1

#define DEFAULT_VOLUME                           30    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 conversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/

/* Number of sub-packets in the audio transfer buffer. You can modify this value but always make sure
  that it is an even number and higher than 3 */
#define OUT_PACKET_NUM                             4


/* Total size of the audio Out transfer buffer */
#define OUT_BUF_MARGIN                             4 /* Margin for each sub-buffer */
#define TOTAL_OUT_BUF_SIZE                         ((uint32_t)((AUDIO_OUT_PACKET + OUT_BUF_MARGIN) * OUT_PACKET_NUM))


#define AUDIO_OUT_BUFF_XFER_CPLT                   usbd_audio_BuffXferCplt

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void  usbd_audio_BuffXferCplt (uint8_t** pbuf, uint32_t* pSize);

#endif /* __AUDIO_APP_CONF__H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

