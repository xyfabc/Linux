/**
  ******************************************************************************
  * @file    stm32072b_audio_codec.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   This file includes the low layer driver for Audio Codec.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32072b_audio_codec.h"
#ifdef __WAV_ADDON__
 #include "wavaddon.h"
#endif /* __WAV_ADDON__ */

/** @addtogroup STM32_Audio_Utilities
  * @{
  */


/** @defgroup STM32_AUDIO_CODEC 
  * @brief STM32 Audio Player module
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* This structure is declared global because it is handled by two different functions */
static DMA_InitTypeDef DMA_InitStructure; 
static uint32_t AudioFrequency = 0;
uint32_t AudioTotalSize = 0xFFFF; /* This variable holds the total size of the audio file */
uint32_t AudioRemSize   = 0xFFFF; /* This variable holds the remaining data in audio file */
uint16_t *CurrentPos;             /* This variable holds the current position of audio pointer */
uint32_t DataSize = 4;
  
/* Private function prototypes -----------------------------------------------*/ 
static uint32_t Audio_MAL_SetAudioConfig(uint8_t NumberOfChannels, uint8_t BitsPerSample);
void CODEC_DAC_IRQHandler(void);
/*------------------------------------------------------------------------------
                           Audio Codec functions 
------------------------------------------------------------------------------*/
/* Low layer codec functions */
static void     Codec_AudioInterface_DeInit(void);
static void     Codec_GPIO_Init(void);
static void     Codec_GPIO_DeInit(void);
static void     TRIGGER_FrequencyIncrement(void);
static void     TRIGGER_FrequencyDecrement(void);

/*----------------------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
  
/**
  * @brief  Configure the audio peripherals.
  * @param  OutputDevice: OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO .
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to paly the audio stream.
  * @param  ExtClk: This parameter is not used.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq, uint32_t Option)
{
  /* Perform low layer Codec initialization */
  if (Codec_Init(OutputDevice, Volume, AudioFreq) != 0)
  {
    return 1;
  }
  else
  {
    /* DMA data transfer preparation:
    Prepare the Media to be used for the audio transfer from memory to DAC peripheral */
    Audio_MAL_Init();
    
    /* Set audio parameters: mono/stereo, 8-bit/16-bit data. This function must
       be called after Audio_MAL_Init
       Option is coded as following:
        bit7 and bit 6 for number of channels
        bit5 to bit 0 for number of bits per sample */
    Audio_MAL_SetAudioConfig((uint8_t) (Option & 0xC0)>>6, (uint8_t) (Option & 0x3f));
    /* Return 0 when all operations are OK */
    return 0;
  }
}

/**
  * @brief Denitializes all the resources used by the codec (those initialized 
  *        by EVAL_AUDIO_Init() function). 
  * @param None.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_DeInit(void)
{ 
  /* DeInitialize the Media layer */
  Audio_MAL_DeInit();
  
  /* DeInitialize Codec */  
  Codec_DeInit();  
  
  return 0;
}

/**
  * @brief Starts playing audio stream from a data buffer for a determined size. 
  * @param pBuffer: Pointer to the buffer 
  * @param Size: Number of audio data BYTES.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Play(uint16_t* pBuffer, uint32_t Size)
{
  /* Set the total number of data to be played (count in word) */
  AudioTotalSize = Size/DataSize;
  
  /* Update the Media layer and enable it for play */  
  Audio_MAL_Play((uint32_t)pBuffer, (uint32_t)(DMA_MAX(AudioTotalSize / DataSize)));
  
  /* Update the remaining number of data to be played */
  AudioRemSize = (Size/DataSize) - DMA_MAX(AudioTotalSize);
  
  /* Update the current audio pointer position */
  CurrentPos = pBuffer + DMA_MAX(AudioTotalSize);
  
  return 0;
}

/**
  * @brief This function Pauses or Resumes the audio file stream. 
  * @param Cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
  *        from 0) to resume. 
  * @param Addr: Address from/at which the audio stream should resume/pause.
  * @param Size: Number of data to be configured for next resume.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size)
{    
  if (Cmd != AUDIO_PAUSE)
  {
    /* Call the Media layer pause/resume function */
    Audio_MAL_PauseResume(Cmd, Addr, Size);
    
    /* Call the Audio Codec Pause/Resume function */
    if (Codec_PauseResume(Cmd) != 0)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    /* Call the Media layer pause/resume function */
    Audio_MAL_PauseResume(Cmd, Addr, Size);
    
    /* Call the Audio Codec Pause/Resume function */
    if (Codec_PauseResume(Cmd) != 0)
    {
      return 1;
    }
    else
    {      
      /* Return 0 if all operations are OK */
      return 0;
    }
  }
}

/**
  * @brief Stops audio playing. 
  * @param Option: could be one of the following parameters 
  * @arg   CODEC_PDWN_SW for software power off (by writing registers) Then no 
  *        need to reconfigure the Codec after power on.
  * @arg   ODEC_PDWN_HW completely shut down the codec (physically). Then need 
  *        to reconfigure the Codec after power on.  
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Stop(uint32_t Option)
{
  /* Call Audio Codec Stop function */
  if (Codec_Stop(Option) != 0)
  {
    return 1;
  }
  else
  {
    /* Call Media layer Stop function */
    Audio_MAL_Stop();
    
    /* Update the remaining data number */
    AudioRemSize = AudioTotalSize;    
    
    /* Return 0 when all operations are correctly done */
    return 0;
  }
}

/**
  * @brief Controls the current audio volume level. 
  * @param Volume: Volume level to be set in percentage from 0% to 100% (0 for 
  *        Mute and 100 for Max volume level).
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t Volume)
{
#ifdef __WAV_ADDON__
  /* Call VolumeControl() from wavaddon.c */
  VolumeControl(Volume);
#endif /* __WAV_ADDON__ */
  return (0);
}

/**
  * @brief Enable or disable the MUTE mode by software 
  * @param Command: could be AUDIO_MUTE_ON to mute sound or AUDIO_MUTE_OFF to 
  *        unmute the codec and restore previous volume level.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_Mute(uint32_t Cmd)
{
  if(Cmd == AUDIO_MUTE_ON)
  {
#ifdef __WAV_ADDON__
    MuteControl(AUDIO_MUTE_ON);
#endif /* __WAV_ADDON__ */
  }
  else if(Cmd == AUDIO_MUTE_OFF)
  {
#ifdef __WAV_ADDON__
    MuteControl(AUDIO_MUTE_OFF);
#endif /* __WAV_ADDON__ */
  }
  else
  {
    return 1;
  }
  return (0);
}

/**
  * @brief  Manage the DMA transfer complete event.
  * @param  None
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t EVAL_AUDIO_XferCpltFlag(void)
{
  /* Clear DMA interrupt flags */
  if (DMA_GetFlagStatus(AUDIO_MAL_DMA_FLAG_TC) != RESET)
  {
    /* Clear the Interrupt flag */
    DMA_ClearFlag(AUDIO_MAL_DMA_FLAG_ALL);
    return 0;
  }
  return 1;
}

/*==============================================================================
                        Audio Codec Control Functions
==============================================================================*/
/**
* @brief Initializes the audio codec (DAC)
  * @param OutputDevice: can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *                       OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO.
  * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param  AudioFreq: Audio frequency used to paly the audio stream.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t Codec_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{
  /* Configure the Codec related IOs */
  Codec_GPIO_Init();
    
  /* Configure the DAC peripheral */
  Codec_AudioInterface_Init(AudioFreq);  
  
  /* Return communication control value */
  return 0;
}

/**
  * @brief Restore the audio codec state to default state and free all used 
  *        resources.
  * @param None.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t Codec_DeInit(void)
{  
  /* Deinitialize all used GPIOs */
  Codec_GPIO_DeInit();
  
  /* Deinitialize the Codec audio interface (DAC) */
  Codec_AudioInterface_DeInit();
  
  /* Return communication control value */
  return 0;
}

/**
  * @brief Pauses and resumes playing on the audio codec.
  * @param Cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
  *        from 0) to resume. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t Codec_PauseResume(uint32_t Cmd)
{
  uint32_t counter = 0;   
  
  /* Pause the audio file playing */
  if (Cmd == AUDIO_PAUSE)
  { 
    /* Disable TIM counter */
    TIM_Cmd(TRIGGER_TIM, DISABLE);     
  }
  else /* AUDIO_RESUME */
  {   
    /* Enable TIM counter */
    TIM_Cmd(TRIGGER_TIM, ENABLE);
  }

  return counter;
}

/**
  * @brief Stops audio Codec playing
  * @param CodecPdwnMode: this parameter is with sense in STM32072B-EVAL.
  * @retval This function return always 0.
  */
uint32_t Codec_Stop(uint32_t CodecPdwnMode)
{
  /* Mute the output first */
  TIM_Cmd(TRIGGER_TIM, DISABLE);
  
  return 0;    
}

/**
* @brief  Switch audio digital streaming to new sampling rate
* @param  NewSamplingRate: Requested Sample rate in Hz.
* @retval 0 if correct communication, else wrong communication
*/
uint32_t EVAL_AUDIO_SwitchSamplingRate(uint32_t NewSamplingRate)
{
  RCC_ClocksTypeDef RCC_Clocks;
  
  /* Get misc clock frequencies: APB1 clock frequency is needed for timer configuration */
  RCC_GetClocksFreq(&RCC_Clocks);
  /* Set the timer period */
  TIM_SetAutoreload(TRIGGER_TIM, (RCC_Clocks.PCLK_Frequency / NewSamplingRate) - 1);
  return 0;
}

/**
  * @brief This function is not used in this codec but kept for compatibility.
  * @param None.
  * @retval This function always return 0.
  */
uint32_t Codec_SwitchOutput(uint8_t Output)
{  
  return 0;
}

/**
  * @brief  Initializes the Audio Codec audio interface (DAC)  
  * @param  AudioFreq: Audio frequency to be configured for the DAC peripheral. 
  * @retval None.
  */
void Codec_AudioInterface_Init(uint32_t AudioFreq)
{
  RCC_ClocksTypeDef RCC_Clocks;
  DAC_InitTypeDef DAC_InitStructure;
#if defined(AUDIO_MAL_DAC_IT_EN)
  NVIC_InitTypeDef NVIC_InitStructure;
#endif
  
  /* Get misc clock frequencies: APB1 clock frequency is needed for timer configuration */
  RCC_GetClocksFreq(&RCC_Clocks);
  /* Trigger timer clock enable */
  RCC_APB1PeriphClockCmd(TRIGGER_TIM_CLOCK, ENABLE);

  /*********************** TRIGGER TIM Configuration **************************/
  /* Reset TIM configuration */
  TIM_DeInit(TRIGGER_TIM);
  
  /* Save the audio frequency in global variable to be used later */
  AudioFrequency = AudioFreq;
  
  /* Set the timer period */
  TIM_SetAutoreload(TRIGGER_TIM, (RCC_Clocks.PCLK_Frequency / AudioFreq) - 1);
  
  /* TIM TRGO update selection */
  TIM_SelectOutputTrigger(TRIGGER_TIM, TIM_TRGOSource_Update);
  
  /*************************** DAC Configuration ******************************/  
  /* Enable the CODEC_DAC peripheral clock */
  RCC_APB1PeriphClockCmd(CODEC_DAC_CLK, ENABLE);
  
  /* Reset CODEC_DAC peripheral */
  DAC_DeInit();
  
  /* CODEC_DAC peripheral configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  /* Enable DAC Channel 1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  
  /* Channel 2 configuration */
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  
  /* Enable DAC Channel 2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);
  
#if defined(AUDIO_MAL_DAC_IT_EN)
  
  /* Enable DMA underrun interrupt */
  DAC_ITConfig(DAC_Channel_1, DAC_IT_DMAUDR, ENABLE);
  
  /* DAC DMA IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = CODEC_DAC_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = EVAL_AUDIO_IRQ_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
  * @brief Restores the Audio Codec audio interface to its default state.
  * @param  None.
  * @retval None.
  */
static void Codec_AudioInterface_DeInit(void)
{
#if defined(AUDIO_MAL_DAC_IT_EN)
  NVIC_InitTypeDef NVIC_InitStructure;
#endif
  
  /* TRIGGER Timer de-initalization */
  /* Reset TIM configuration */
  TIM_DeInit(TRIGGER_TIM);
  /* Trigger timer clock enable */
  RCC_APB1PeriphClockCmd(TRIGGER_TIM_CLOCK, DISABLE);
  
  /* Disable the DAC peripheral (in case it hasn't already been disabled) */
  DAC_Cmd(DAC_Channel_1, DISABLE);
  
  /* in case of mono, only channel 1 is used and disabling channel2 is with no sense */
  DAC_Cmd(DAC_Channel_2, DISABLE);
  
  /* Deinitialize the DAC peripheral */
  DAC_DeInit();
  
  /* Disable the DAC peripheral clock */
  RCC_APB1PeriphClockCmd(CODEC_DAC_CLK, DISABLE);
  
#if defined(AUDIO_MAL_DAC_IT_EN)
  /* DAC DMA IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = CODEC_DAC_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = EVAL_AUDIO_IRQ_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
  * @brief Initializes IOs used by the Audio Codec in analog mode.
  * @param  None.
  * @retval None.
  */
static void Codec_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable DAC GPIO clocks */
  RCC_AHBPeriphClockCmd(CODEC_DAC_GPIO_CLOCK, ENABLE);
  
  /* CODEC_DAC Channel 1 and Channel 2 pins configuration in analog mode */
  GPIO_InitStructure.GPIO_Pin =  CODEC_DAC_CHANNEL1 | CODEC_DAC_CHANNEL2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief Restores the IOs used by the Audio Codec interface to their default 
  *        state
  * @param  None.
  * @retval None.
  */
static void Codec_GPIO_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Deinitialize all the GPIOs used by the driver */
  GPIO_InitStructure.GPIO_Pin = CODEC_DAC_CHANNEL1 | CODEC_DAC_CHANNEL2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CODEC_DAC_GPIO, &GPIO_InitStructure);
}

#ifdef USE_DEFAULT_UNDERRUN_CALLBACK
/**
  * @brief  Basic management of the underrun situation.
  * @param  None.
  * @retval None.
  */
uint32_t Codec_UNDERRUN_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {
  }
}
#endif /* USE_DEFAULT_UNDERRUN_CALLBACK */

#ifdef AUDIO_MAL_DAC_IT_EN
/**
  * @brief  This function handles TIM6 and DAC interrupt request.
  * @param  None
  * @retval None
  */
void CODEC_DAC_IRQHandler(void)
{
#ifdef USE_DEFAULT_UNDERRUN_CALLBACK
  if(DAC_GetFlagStatus(DAC_Channel_1, DAC_FLAG_DMAUDR) != RESET)
  {
    Codec_UNDERRUN_UserCallback();
  }
#endif /* USE_DEFAULT_UNDERRUN_CALLBACK */
}
#endif /* AUDIO_MAL_DAC_IT_EN */

/*==============================================================================
                Audio MAL Interface Control Functions
==============================================================================*/

/**
  * @brief  Initializes and prepares the Media to perform audio data transfer 
  *         from Media to the DAC peripheral.
  * @param  None.
  * @retval None.
  */
void Audio_MAL_Init(void)
{
#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
  NVIC_InitTypeDef NVIC_InitStructure;
#endif
  
  /* Enable the DMA clock */
  RCC_AHBPeriphClockCmd(AUDIO_MAL_DMA_CLOCK, ENABLE);
  
  /* Configure the DMA channel */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);
  DMA_DeInit(AUDIO_MAL_DMA_CHANNEL);
  /* Set the parameters to be configured */
  /* By default 16-bit stereo is selected: this field can be updated using EVAL_AUDIO_SetAudioConfig() */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12LD_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0; /* This field will be configured in play function */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)0xFFFE; /* This field will be configured in play function */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = AUDIO_MAL_DMA_PERIPH_DATA_SIZE;
  DMA_InitStructure.DMA_MemoryDataSize = AUDIO_MAL_DMA_MEM_DATA_SIZE;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_Init(AUDIO_MAL_DMA_CHANNEL, &DMA_InitStructure);

  /* Enable the selected DMA interrupts (selected in "stm32_eval_audio_codec.h" defines) */
#ifdef AUDIO_MAL_DMA_IT_TC_EN
  DMA_ITConfig(AUDIO_MAL_DMA_CHANNEL, DMA_IT_TC, ENABLE);
#endif /* AUDIO_MAL_DMA_IT_TC_EN */
#ifdef AUDIO_MAL_DMA_IT_HT_EN
  DMA_ITConfig(AUDIO_MAL_DMA_CHANNEL, DMA_IT_HT, ENABLE);
#endif /* AUDIO_MAL_DMA_IT_HT_EN */
#ifdef AUDIO_MAL_DMA_IT_TE_EN
  DMA_ITConfig(AUDIO_MAL_DMA_CHANNEL, DMA_IT_TE, ENABLE);
#endif /* AUDIO_MAL_DMA_IT_TE_EN */
  
  /* Enable the DAC DMA request */
  DAC_DMACmd(DAC_Channel_1, ENABLE);
    
#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
  /* DAC DMA IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_MAL_DMA_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = EVAL_AUDIO_IRQ_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif 
}

/**
  * @brief  Restore default state of the used Media.
  * @param  None.
  * @retval None.
  */
void Audio_MAL_DeInit(void)  
{
#if defined(AUDIO_MAL_DMA_IT_TC_EN) || defined(AUDIO_MAL_DMA_IT_HT_EN) || defined(AUDIO_MAL_DMA_IT_TE_EN)
  NVIC_InitTypeDef NVIC_InitStructure;  
  
  /* Deinitialize the NVIC interrupt for the DAC DMA channel */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_MAL_DMA_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPriority = EVAL_AUDIO_IRQ_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);  
#endif 
  
  /* Configure the DMA channel */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);
  
  /* Dinitialize the DMA channel */
  DMA_DeInit(AUDIO_MAL_DMA_CHANNEL);

  /* The DMA clock is not disabled, since it can be used by other channels */
}

/**
  * @brief  Starts playing audio stream from the audio Media.
  * @param  Addr: Pointer to the audio stream buffer.
  *         It must be 16 bit-aligned in case of:
  *            1. mono channel and 16 bits per sample.
  *            1. stereo channels and 8 bits per sample.
  *         It must be 32 bit-aligned in case of stereo channels and 16 bits per sample.
  * @param  Size: Number of data in the audio stream buffer in half-word.
  * @retval None.
  */
void Audio_MAL_Play(uint32_t Addr, uint32_t Size)
{
#ifndef AUDIO_USE_MACROS 
  /* Disable the DAC DMA */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);

  /* Clear the Interrupt flag */
  DMA_ClearFlag(AUDIO_MAL_DMA_FLAG_TC); 
  
  /* Configure the buffer address and size */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Addr;
  /* Size is in half-word, multiply it by 2 to get the size in byte */
  DMA_InitStructure.DMA_BufferSize = (uint32_t)(Size*2/DataSize);

  /* Configure the DMA channel with the new parameters */
  DMA_Init(AUDIO_MAL_DMA_CHANNEL, &DMA_InitStructure);
  
  /* Enable the DAC DMA channel */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, ENABLE);

  /* If the DAC peripheral is still not enabled, enable it */
  if ((CODEC_DAC->CR & (DAC_CR_EN1 | DAC_CR_EN2)) == 0)
  {
    /* Enable DAC channel1 */
    DAC_Cmd(CODEC_DAC_CHANNEL1, ENABLE);
    /* Enable DAC channel2: in case of stereo */
    DAC_Cmd(CODEC_DAC_CHANNEL2, ENABLE);
  }
  
  /* Enable the trigger timer peripheral */
  TIM_Cmd(TRIGGER_TIM, ENABLE);
#else
  
  /* Disable the DAC DMA channel */
  AUDIO_MAL_DMA_CHANNEL->CCR &= ~(uint32_t)DMA_CCR_EN;
  
  /* Clear the Interrupt flag */
  AUDIO_MAL_DMA->AUDIO_MAL_DMA_IFCR = (uint32_t)(AUDIO_MAL_DMA_FLAG_TC);
  
  /* Wait the DMA channel to be effectively disabled */
  while ((AUDIO_MAL_DMA_CHANNEL->CCR & (uint32_t)DMA_CCR_EN) != 0)
  {}
 
  /* Configure the buffer address and size */
  AUDIO_MAL_DMA_CHANNEL->CMAR = (uint32_t)Addr;

  AUDIO_MAL_DMA_CHANNEL->CNDTR = (uint32_t)(Size*2/DataSize);
  
  /* Enable the DAC DMA channel */
  AUDIO_MAL_DMA_CHANNEL->CCR |= (uint32_t)DMA_CCR_EN;
  
  /* If the DAC peripheral is still not enabled, enable it.
     Channel 2 is enabled in case of stereo  */
  CODEC_DAC->CR |= (uint32_t)(DAC_CR_EN1 | DAC_CR_EN2);
  
  /* Enable Timer */
  TRIGGER_TIM->CR1 = TIM_CR1_CEN;
#endif /* AUDIO_USE_MACROS */
}

/**
  * @brief  Pauses or Resumes the audio stream playing from the Media.
  * @param Cmd: AUDIO_PAUSE (or 0) to pause, AUDIO_RESUME (or any value different
  *        from 0) to resume. 
  * @param Addr: Address from/at which the audio stream should resume/pause.
  * @param Size: Number of data to be configured for next resume.
  * @retval None.
  */
void Audio_MAL_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size)
{
  /* Pause the audio file playing */
  if (Cmd == AUDIO_PAUSE)
  {
    /* Pause the DAC DMA channel */
    DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);
    
    /* Disable the DMA channel */
    DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);
    
    /* Clear the Interrupt flag */
    DMA_ClearFlag(AUDIO_MAL_DMA_FLAG_ALL);  
  }
  else /* AUDIO_RESUME */
  {
    /* Disable DMA channel */
    DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);
    
    /* Configure the buffer address and size */
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Addr;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)(Size*2/DataSize);
  
    /* Configure the DMA channel with the new parameters */
    DMA_Init(AUDIO_MAL_DMA_CHANNEL, &DMA_InitStructure);
    
    /* Resume the DAC DMA channel */
    DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, ENABLE);
  }
}

/**
  * @brief  Stops audio stream playing on the used Media.
  * @param  None.
  * @retval None.
  */
void Audio_MAL_Stop(void)
{   
  /* Stop the Transfer on the DAC side: Stop and disable the DMA channel */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);

  /* Clear all the DMA flags for the next transfer */
  DMA_ClearFlag(AUDIO_MAL_DMA_FLAG_ALL);
  
  /* Wait the DMA channel to be effectively disabled */
  while ((AUDIO_MAL_DMA_CHANNEL->CCR & DMA_CCR_EN) == RESET)
  {}

  /* Stop the current DMA request by resetting the DAC cell */
  Codec_AudioInterface_DeInit();
  
  /* Re-configure the DAC interface for the next play operation */
  Codec_AudioInterface_Init(AudioFrequency);
}

/**
  * @brief  Configure the audio peripheral. The configuration parameters can be
  *         provided from the header of audio file.
  * @param  NumberOfChannels: number of channels: mono or stereo
  *         This parameter can be: 1 to select mono
  *                                2 to select stereo
  * @param  BitsPerSample: the number of effective data bits
  *         This parameter can be: 8 to select 8-bit data per sample
  *                                16 to select 16-bit data per sample
  * @retval 0 if correct configuration, otherwise wrong configuration
  */
static uint32_t Audio_MAL_SetAudioConfig(uint8_t NumberOfChannels, uint8_t BitsPerSample)
{
  /* Configure the DMA channel */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, DISABLE);
  /* Check if output type is mono and the bit depth is 8-bit */
  if((NumberOfChannels == 1) && (BitsPerSample == 8))
  {
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR8R1_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DataSize = 1;/* data size is 1 byte (8-bit mono) */
  }
  /* Check if output type is stereo and the bit depth is 8-bit */
  else if((NumberOfChannels == 2) && (BitsPerSample == 8))
  {
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR8RD_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DataSize = 2;/* data size is 2 bytes (16-bit stereo) */
  }
  /* Check if output type is mono and the bit depth is 16-bit */
  else if((NumberOfChannels == 1) && (BitsPerSample == 16))
  {
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12L1_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DataSize = 2;/* data size is 2 bytes (16-bit mono) */
  }
  /* Check if output type is stereo and the bit depth is 16-bit */
  else if((NumberOfChannels == 2) && (BitsPerSample == 16))
  {
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12LD_ADDRESS;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DataSize = 4;/* data size is 4 bytes (16-bit stereo) */
  }
  else
  {
    /* wrong configuration */
    return 1;
  }
  
  /* Re-Init DMA */
  DMA_Init(AUDIO_MAL_DMA_CHANNEL, &DMA_InitStructure);
  /* Configure the DMA channel */
  DMA_Cmd(AUDIO_MAL_DMA_CHANNEL, ENABLE);
  /* correct configuration */
  return 0;
}

/**
  * @brief  Returns the remaining data count to be processed by MAL layer.
  * @param  None.
  * @retval Number of processed data.
  */
uint32_t Audio_MAL_GetRemCount(void)
{
  /* Return the remaining number of data */
  return (uint32_t)(DMA_GetCurrDataCounter(AUDIO_MAL_DMA_CHANNEL));
}

/*==============================================================================
                        Trigger Timer Functions
==============================================================================*/

/**
  * @brief  Trimm the current output frequency.
  * @param  Cycles: Number of clock cycles to be increased or decreased.
  *         The cycles are relative to the timer clock.
  * @retval 0 if correct configuration, else wrong configuration
  */
uint32_t EVAL_AUDIO_SamplingRateTrimm(int32_t Cycles)
{
  int32_t nbcycles = Cycles;
  
  if(nbcycles > 0)
  {
    while (nbcycles != 0)
    {
      TRIGGER_FrequencyIncrement();
      nbcycles--;
    }
  }
  else if(nbcycles < 0)
  {
    while (nbcycles != 0)
    {
      TRIGGER_FrequencyDecrement();
      nbcycles++;
    }
  }
  return 0;
}

/**
  * @brief Increment the timer frequency
  * @param none
  * @retval none
  */
static void TRIGGER_FrequencyIncrement(void)
{
  TRIGGER_TIM->ARR--;
}

/**
  * @brief Decrement the timer frequency
  * @param none
  * @retval none
  */
static void TRIGGER_FrequencyDecrement(void)
{
  TRIGGER_TIM->ARR++;
}

/**
  * @}
  */ 


/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
