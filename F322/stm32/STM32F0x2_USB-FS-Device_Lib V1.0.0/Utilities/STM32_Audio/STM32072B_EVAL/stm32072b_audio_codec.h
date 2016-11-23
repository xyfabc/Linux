/**
  ******************************************************************************
  * @file    stm32072b_audio_codec.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   This file contains all the functions prototypes for the audio codec
  *          low layer driver.
  *            
  *     *******************************************************************  
  *                           IMPORTANT NOTES
  *     *******************************************************************  
  *     This driver is intended for use ONLY in the scope of the current Audio 
  *     Example.
  *        
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
#ifndef __STM32072B_AUDIOCODEC_H
#define __STM32072B_AUDIOCODEC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/** @addtogroup STM32_Audio_Utilities
  * @{
  */

/** @defgroup STM32_AUDIO_CODEC
  * @brief This file is the header file for the stm32 audio player module 
  * @{
  */ 

/* Exported constants --------------------------------------------------------*/
 
/*------------------------------------
             CONFIGURATION: Audio Codec Driver Configuration parameters
                                      ----------------------------------------*/
/* Audio Transfer mode (DMA, Interrupt or Polling) */
#define AUDIO_MAL_MODE_NORMAL    /* Uncomment this line to enable the audio 
                                         Transfer using DMA */

/* For the codec DAC underrun condition can be detected */
/* #define AUDIO_MAL_DAC_IT_EN  Uncomment this line to enable DAC underrun interrupt */

/* For the DMA modes select the interrupt that will be used */
#define AUDIO_MAL_DMA_IT_TC_EN   /* Uncomment this line to enable DMA Transfer Complete interrupt */
#define AUDIO_MAL_DMA_IT_HT_EN   /* Uncomment this line to enable DMA Half Transfer Complete interrupt */
#define AUDIO_MAL_DMA_IT_TE_EN   /* Uncomment this line to enable DMA Transfer Error interrupt */

/* Select the interrupt priority for the DMA interrupt 
   This define may be set in a generic configuration file (ie. audio_app_conf.h) */
#define EVAL_AUDIO_IRQ_PRIO          0    /* Select the preemption priority level(0 is the highest) */

/* Uncomment the following line to use the default Codec_UNDERRUN_UserCallback() 
   function implemented in stm32072b_eval_audio_codec.c file.
   Codec_UNDERRUN_UserCallback() function is called whenever a underrun condition 
   occurs */
#ifdef AUDIO_MAL_DAC_IT_EN
  #define USE_DEFAULT_UNDERRUN_CALLBACK
#endif /* AUDIO_MAL_DAC_IT_EN */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
                    Hardware Configuration defines parameters
------------------------------------------------------------------------------*/
/* TIM peripheral configuration: Trigger timer is used to trigger the transfer 
   from memory to DAC data register
   In STM32F0x devices only the following timers can be used as DAC trigger:
   TIM6, TIM3, TIM7 (only STM32F072), TIM15, TIM2 */
#define TRIGGER_TIM                    TIM6  /* TIM6 is selected as trigger timer */
#define TRIGGER_TIM_CLOCK              RCC_APB1Periph_TIM6
 /* DAC_TRIGGER_SOURCE must correspond to TRIGGER_TIM: 
    e.g. if TRIGGER_TIM is set to TIM2, DAC_TRIGGER_SOURCE must be set to DAC_Trigger_T2_TRGO */
#define DAC_TRIGGER_SOURCE             DAC_Trigger_T6_TRGO
#define TRIGGER_TIM_IRQ                TIM6_DAC_IRQn

/* DAC peripheral configuration defines */
#define CODEC_DAC                      DAC
#define CODEC_DAC_CLK                  RCC_APB1Periph_DAC
#define CODEC_DAC_DATA_ADDRESS         DAC_DHR12LD_ADDRESS
#define CODEC_DAC_IRQ                  TIM6_DAC_IRQn
#define CODEC_DAC_GPIO_CLOCK           RCC_AHBPeriph_GPIOA
#define CODEC_DAC_CHANNEL1             GPIO_Pin_4
#define CODEC_DAC_CHANNEL2             GPIO_Pin_5
#define CODEC_DAC_GPIO                 GPIOA
#define CODEC_DAC_IRQ                  TIM6_DAC_IRQn
#define CODEC_DAC_IRQHandler           TIM6_DAC_IRQHandler

/* DMA channel definitions */
#define AUDIO_MAL_DMA_CLOCK            RCC_AHBPeriph_DMA1
#define AUDIO_MAL_DMA_CHANNEL          DMA1_Channel3
#define AUDIO_MAL_DMA_IRQ              DMA1_Channel2_3_IRQn
#define AUDIO_MAL_DMA_FLAG_TC          DMA1_FLAG_TC3
#define AUDIO_MAL_DMA_FLAG_HT          DMA1_FLAG_HT3
#define AUDIO_MAL_DMA_FLAG_TE          DMA1_FLAG_TE3
#define AUDIO_MAL_DMA_FLAG_ALL         (uint32_t)(DMA1_FLAG_TE3 | DMA1_FLAG_HT3 |\
                                                   DMA1_FLAG_TC3)
/* Peripheral and memory data size (word, half-word and byte) depend on number
   of channels (mono or stereo): The settings below correspond to stereo mode */
#define AUDIO_MAL_DMA_PERIPH_DATA_SIZE DMA_PeripheralDataSize_Word
#define AUDIO_MAL_DMA_MEM_DATA_SIZE    DMA_MemoryDataSize_Word
#define DMA_MAX_SZE                    0xFFFF
#define AUDIO_MAL_DMA                  DMA1
#define AUDIO_MAL_DMA_IFCR             IFCR

#define Audio_MAL_IRQHandler           DMA1_Channel2_3_IRQHandler

/**************************** DAC Data registers ******************************/
/* 12-bit Left alignment (dual channels) used in case of stereo and 16-bit data */
#define DAC_DHR12LD_ADDRESS     0x40007424
/* 8-bit right alignment (dual channels) used in case of stereo and 8-bit data */
#define DAC_DHR8RD_ADDRESS      0x40007428
/* 12-bit left alignment (channel1) used in case of mono and 16-bit data */
#define DAC_DHR12L1_ADDRESS     0x4000740C
/* 8-bit right alignment (channel1) used in case of mono and 8-bit data */
#define DAC_DHR8R1_ADDRESS      0x40007410

/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
                        Audio Codec User defines
------------------------------------------------------------------------------*/
/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0
/*----------------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define DMA_MAX(x)                (((x) <= DMA_MAX_SZE)? (x):DMA_MAX_SZE)

/* Exported functions ------------------------------------------------------- */ 

/*------------------------------------------------------------------------------
                           Generic functions 
------------------------------------------------------------------------------*/
uint32_t EVAL_AUDIO_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq, uint32_t Option);
uint32_t EVAL_AUDIO_DeInit(void);
uint32_t EVAL_AUDIO_Play(uint16_t* pBuffer, uint32_t Size);
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size);
uint32_t EVAL_AUDIO_Stop(uint32_t CodecPowerDown_Mode);
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t Volume);
uint32_t EVAL_AUDIO_Mute(uint32_t Command);
uint32_t EVAL_AUDIO_XferCpltFlag(void);
uint32_t EVAL_AUDIO_SwitchSamplingRate (uint32_t NewSamplingRate);
uint32_t EVAL_AUDIO_SamplingRateTrimm(int32_t Cycles);
uint32_t Codec_SwitchOutput(uint8_t Output);

/*------------------------------------------------------------------------------
                           Audio Codec functions 
------------------------------------------------------------------------------*/
/* High Layer codec functions */
uint32_t Codec_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
uint32_t Codec_DeInit(void);
uint32_t Codec_PauseResume(uint32_t Cmd);
uint32_t Codec_Stop(uint32_t Cmd);
uint32_t Codec_VolumeCtrl(uint8_t Volume);
uint32_t Codec_Mute(uint32_t Cmd);
void     Codec_AudioInterface_Init(uint32_t AudioFreq);

/*------------------------------------------------------------------------------
                   MAL (Media Access Layer) functions 
------------------------------------------------------------------------------*/
void     Audio_MAL_Init(void);
void     Audio_MAL_DeInit(void);
void     Audio_MAL_Play(uint32_t Addr, uint32_t Size);
void     Audio_MAL_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size);
void     Audio_MAL_Stop(void);
uint32_t Audio_MAL_GetRemCount(void);

/* User Callbacks: user has to implement these functions in his code if
  they are needed. -----------------------------------------------------------*/

/* This function is called when the requested data has been completely transferred.
   In Normal mode (when  the define AUDIO_MAL_MODE_NORMAL is enabled) this function
   is called at the end of the whole audio file. */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size);

/* This function is called when half of the requested buffer has been transferred */
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size);

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void EVAL_AUDIO_Error_CallBack(void* pData);

/* Codec_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occurs during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...) on the Codec control interface.
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm32072b_audio_codec.h file. */
uint32_t Codec_TIMEOUT_UserCallback(void);

 
/**
  * @}
  */

/**
  * @}
  */  
#endif /* __STM32072B_AUDIOCODEC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
