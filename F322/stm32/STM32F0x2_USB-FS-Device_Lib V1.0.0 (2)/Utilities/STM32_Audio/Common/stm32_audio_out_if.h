/**
  ******************************************************************************
  * @file    stm32_audio_out_if.h
  * @author  MCD Application Team
  * @version V2.0.2
  * @date    31-January-2014 
  * @brief   header file for the stm32_audio_out_if.c file.
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
#ifndef __USB_AUDIO_OUT_IF_H_
#define __USB_AUDIO_OUT_IF_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32_audio.h"

/** @addtogroup STM32_AUDIO_OUT_IF
  * @{
  */
  
/** @addtogroup Audio_Out_Interface
  * @{
  */ 


/** @addtogroup stm32_audio_Exported_Defines
  * @{
  */ 

/* Mute commands */
#define AUDIO_MUTE                      0x01
#define AUDIO_UNMUTE                    0x00

/* Functions return value */
#define AUDIO_OK                        0x00
#define AUDIO_FAIL                      0xFF

/* Audio Machine States */
#define AUDIO_STATE_INACTIVE            0x00
#define AUDIO_STATE_ACTIVE              0x01
#define AUDIO_STATE_PLAYING             0x02
#define AUDIO_STATE_PAUSED              0x03
#define AUDIO_STATE_STOPPED             0x04
#define AUDIO_STATE_ERROR               0x05

#ifndef MAX_AUDIO_FREQ 
 #define MAX_AUDIO_FREQ                  DEFAULT_OUT_AUDIO_FREQ
#endif /* MAX_AUDIO_FREQ */
#ifndef MIN_AUDIO_FREQ 
 #define MIN_AUDIO_FREQ                  32000
#endif /* MIN_AUDIO_FREQ */
/**
  * @}
  */ 


/** @defgroup stm32_audio_Exported_TypesDefinitions
  * @{
  */
/* Audio Commands enumeration */
typedef enum
{
  AUDIO_CMD_PLAY = 1,
  AUDIO_CMD_PAUSE,
  AUDIO_CMD_STOP,
}AUDIO_CMD_TypeDef;
/**
  * @}
  */ 



/** @defgroup stm32_audio_Exported_Macros
  * @{
  */ 

#define XferCplt     Audio_MAL_IRQHandler

/**
  * @}
  */ 

/** @defgroup stm32_audio_Exported_Variables
  * @{
  */ 

extern AUDIO_FOPS_TypeDef  AUDIO_OUT_fops;

/**
  * @}
  */ 

/** @defgroup stm32_audio_Exported_Functions
  * @{
  */
void XferCplt (void);
/**
  * @}
  */ 

#endif  /* __USB_AUDIO_OUT_IF_H_ */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
