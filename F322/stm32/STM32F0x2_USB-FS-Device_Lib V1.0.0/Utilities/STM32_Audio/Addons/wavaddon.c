/**
  ******************************************************************************
  * @file    wavaddon.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   This file includes WAV Add-On layer layer routines.
  *          It is used only for STM32F0xx devices when playing audio using the
  *          Digital-to-Analog Converter which is the case for STM32072B_EVAL.  
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

/* Includes ------------------------------------------------------------------*/
#include "wavaddon.h"

#if defined( __WAV_ADDON__)  /* Don't build if not configured for use in audio_app_conf.h */

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @defgroup WavAddon
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint8_t NumberOfChannels;/* 1: mono; 2: stereo */
  uint8_t BitsPerSample; /*8: 8-bit; 16: 16-bit */
}WavParam_TypeDef;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* By default: use stereo (2) and 16 bits per sample */
WavParam_TypeDef WavParam = {2, 16};
__IO float  AudioVolume = 0.25;
__IO float  SavedAudioVol = 0.25;
/* Private function prototypes -----------------------------------------------*/
static uint32_t SignProcessing(void* pBuffer, uint32_t BufferSize);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Initialize the Audio data processing: number of channels and bits per sample.
  * @param NChannelBitsSample: the number of channels concatenated to the number of bits per sample.
  * @param Option: Not used.
  * @retval none
  */
void WAVADDON_Init(uint32_t NChannelBitsSample, uint32_t Option)
{
  WavParam.NumberOfChannels = (uint8_t) (NChannelBitsSample & 0xC0)>>6;
  WavParam.BitsPerSample = (uint8_t) (NChannelBitsSample & 0x3f);
}

/**
  * @brief Process Audio data (sign and volume) before transmit it to DAC
  * @param pBuffer: pointer at the audio buffer
  * @param BufferSize: the size of the buffer in bytes
  * @retval none
  */
void WAVADDON_AudioProcessing(void* pBuffer, uint32_t BufferSize)
{
  /* Process the sign: It is used in case of 16-bit data */
  SignProcessing(pBuffer, BufferSize);
}

/**
  * @brief Toggle sign bit of input buffer.
  * @param pBuffer: pointer at the audio buffer. It must be 16 bit-aligned.
  * @param BufferSize: the size of the buffer in bytes.
  * @retval 0 if correct configuration, otherwise wrong configuration.
  */
static uint32_t SignProcessing(void* pBuffer, uint32_t BufferSize)
{
  uint32_t readdata = (uint32_t)pBuffer;
  uint32_t loopcounter = 0;
  
  /* Check if data is 8-bit or 16-bit */
  if(WavParam.BitsPerSample == 8)
  {
    /* No sign bit inversion in case of 8-bit samples: data are always unsigned */
  }
  else if(WavParam.BitsPerSample == 16)
  {
    /* Invert sign bit: PCM format is 16-bit signed and DAC is 12-bit unsigned */
    for(loopcounter = 0; loopcounter < BufferSize/2; loopcounter++)
    {
      *(uint16_t*)readdata ^= 0x8000;
      readdata+=2;
    }
  }
  else
  {
    return 1;
  }
  return 0;
}

/**
  * @brief Control the global variable AudioVolume.
  * @param Volume: The volume of audio stream. It should be lower than DEFAULT_VOLMAX.
  * @retval None.
  */
void VolumeControl(uint8_t Volume)
{
  float vol;
  
  /* Convert the volume */
  vol = (Volume/200.0f);
  
  /* Check if the volume exceeds the max value */
  if(vol < DEFAULT_VOLMAX)
  {
    AudioVolume = vol;
  }
  else
  {
    AudioVolume = DEFAULT_VOLMAX;
  }
}

/**
  * @brief Control the mute of the audio stream.
  * @param Command: AUDIO_MUTE_ON to mute sound
  *                 or AUDIO_MUTE_OFF to restore previous volume level.
  * @retval None.
  */
void MuteControl(uint32_t Cmd)
{
  /* Check if AUDIO_MUTE_ON */
  if(Cmd == AUDIO_MUTE_ON)
  {
    /* Save the current volume */
    SavedAudioVol = AudioVolume;
    /* Mute: Set volume to 0 */
    AudioVolume = 0;
  }
  else /* Check if AUDIO_MUTE_OFF */
  {
    /* Unmute: Set volume to the previous saved value */
    AudioVolume = SavedAudioVol;
  }
}

#endif /*  __WAV_ADDON__ */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
