/**
  ******************************************************************************
  * @file    songutilities.h
  * @author  MCD Application Team
  * @version V2.0.2
  * @date    31-January-2014 
  * @brief   This file contains all the functions prototypes for memory management
  *          layer routines
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
#ifndef __SONG_UTILITIES_H
#define __SONG_UTILITIES_H

/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"

/** @addtogroup STM32_audio_codecs
  * @{
  */

/** @addtogroup SongUtilities
  * @{
  */

/** @defgroup SongUtilities_Exported_Types
  * @{
  */
typedef struct {
  char * Title;
  char * Artist;
  char * Album;
  char * Year;
  char * Genre;
  uint32_t SamplingRate;
}TAGS_TypeDef;

/**
  * @}
  */

/** @defgroup SongUtilities_Exported_Constants
  * @{
  */
#ifndef MAX_TAG_STRING_LENGTH
 #define MAX_TAG_STRING_LENGTH          40
#endif /* MAX_TAG_STRING_LENGTH */

/* #define USE_DYNAMIC_MEMORY */ /* uncomment this define to use dynamic memory allocation
                                 in SongUtilities_parseExtendedContentDescriptors(); this 
                                 function extracts WMA song description from extended content descriptor object */

/**
  * @}
  */

/** @defgroup SongUtilities_Exported_Macros
  * @{
  */

/* Specify "mem_cpy" function to be used by mp3/wma tag parser */
#ifndef MEM_CPY
 #define MEM_CPY        mem_cpy
#endif /* MEM_CPY */

/* Specify "mem_set" function to be used by mp3/wma tag parser */
#ifndef MEM_SET
 #define MEM_SET          mem_set
#endif /* MEM_SET */

/* Specify "Seek File" function to be used by mp3/wma tag parser */
#ifndef FILE_SEEK
 #define FILE_SEEK        f_lseek
#endif /* FILE_SEEK */

/* Specify "Read File" function to be used by mp3/wma tag parser */
#ifndef FILE_READ
 #define FILE_READ         f_read
#endif /* FILE_READ */


/**
  * @}
  */

/** @defgroup SongUtilities_Exported_Functions
  * @{
  */
#ifdef __MP3_DECODER__
 int8_t SongUtilities_MP3TagParser(uint8_t * buffer,  TAGS_TypeDef * pIDTAGS ,FIL * file);
#endif /* __MP3_DECODER__ */
#ifdef __WMA_DECODER__
 void SongUtilities_WMATagParser( TAGS_TypeDef *pIDTAG,FIL * file, uint8_t * pBuf );
#endif /* __WMA_DECODER__ */
void StrCtrlLength(void* Dst, void* Src, uint32_t MaxLen);

#endif /* __SONG_UTILITIES_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
