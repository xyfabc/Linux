/**
  ******************************************************************************
  * @file    wavaddon.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   This file includes WAV Add-On layer headers.
  *          It is used only for STM32F0xx devices when playing audio using the
  *          Digital-to-Analog Converter which is the case for STM32072B_EVAL.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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
#ifndef __WAVADDON_H_
#define __WAVADDON_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef STM32F072
 #include "stm32f0xx.h"
 #include "stm32072b_audio_codec.h"
#else
 #error wavaddon is used only for STM32F0xx devices
#endif /* STM32F072 */

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @addtogroup WavAddon
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Maximum and minimum levels of volume value */
#define DEFAULT_VOLMIN                0.0f
#define DEFAULT_VOLMAX                0.5f

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void WAVADDON_Init(uint32_t NChannelBitsSample, uint32_t Option);
void WAVADDON_AudioProcessing(void* pBuffer, uint32_t BufferSize);
void VolumeControl(uint8_t Volume);
void MuteControl(uint32_t Cmd);

#ifdef __cplusplus
}
#endif

#endif /* __WAVADDON_H_ */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
