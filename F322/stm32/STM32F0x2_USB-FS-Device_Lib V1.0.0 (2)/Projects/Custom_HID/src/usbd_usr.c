/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   This file contains user callback structure for USB events Management
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
#include "usbd_usr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    0x40012440

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,   
};

uint32_t ADC_ConvertedValueX = 0;
uint32_t ADC_ConvertedValueX_1 = 0;

/* Private function prototypes -----------------------------------------------*/
static void ADC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Device lib initialization
  * @param  None
  * @retval None
  */
void USBD_USR_Init(void)
{   
  /* Initialize LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* Configure the ADC*/
  ADC_Configuration();
  
}

/**
  * @brief  Reset Event
  * @param  speed : device speed
  * @retval None
  */
void USBD_USR_DeviceReset(uint8_t speed )
{
}

/**
  * @brief  Configuration Event
  * @param  None
  * @retval None
  */
void USBD_USR_DeviceConfigured (void)
{
}

/**
  * @brief  Device suspend Event
  * @param  None
  * @retval None
  */
void USBD_USR_DeviceSuspended(void)
{
}


/**
  * @brief  Device resume Event
  * @param  None
  * @retval None
  */
void USBD_USR_DeviceResumed(void)
{
}

/**
  * @brief  ADC_Configuration
*         Configure the ADC
  * @param  None
  * @retval None
  */
static void ADC_Configuration(void)
{
  ADC_InitTypeDef    ADC_InitStructure;
  DMA_InitTypeDef    DMA_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Configure ADC Channel11 as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* DMA1 channel1 configuration */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValueX;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* Enable the DMA1 Channel1 Transfer complete interrupt */
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStructure); 
  
  /* Convert the ADC1 Channel 1 with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_10 , ADC_SampleTime_55_5Cycles);   
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
  /* Test DMA1 TC flag */
  while((DMA_GetFlagStatus(DMA1_FLAG_TC1)) == RESET ); 
  
  ADC_ConvertedValueX = ADC_GetConversionValue(ADC1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
