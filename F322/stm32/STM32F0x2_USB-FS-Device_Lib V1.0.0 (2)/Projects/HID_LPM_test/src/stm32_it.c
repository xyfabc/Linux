/**
  ******************************************************************************
  * @file    stm32_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
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
#include "stm32_it.h"
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP     20
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef LPM_ENABLED
extern __IO uint32_t  L1_remote_wakeup;
#endif
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static uint8_t *USBD_HID_GetPos (void);

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function sends the input report repeatedly.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  uint8_t *buf;
  
  /* Get Joystick position */
  buf = USBD_HID_GetPos();
  
  /* Update the cursor position */
  if((buf[1] != 0) ||(buf[2] != 0))
  {
    /* Send Report */
    USBD_HID_SendReport (&USB_Device_dev, 
                         buf,
                         4);
  }    
} 

/**
  * @brief  This function handles USB FS Handler.
  * @param  None
  * @retval None
  */
void USB_IRQHandler(void)
{
  USB_Istr();
}

/**
  * @brief  This function handles EXTI14_15_IRQ Handler.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
  
  if (EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
  {
#ifdef LPM_ENABLED
    /* check if L1 remote wakeup case*/   
    if (L1_remote_wakeup)
    {
     
     /* L1 remote wakeup */
     Resume(L1_RESUME_INTERNAL);
          
      /* clear L1_remote_wakeup flag*/
      L1_remote_wakeup = 0;
    }    
    
    /* Check if the L2 remote wakeup feature is enabled */
    else
#endif    
    if (USB_Device_dev.dev.DevRemoteWakeup)
    {
      /* Exit low power mode and re-configure clocks */
      Resume(RESUME_INTERNAL);
      USB_Device_dev.dev.device_status = USB_Device_dev.dev.device_old_status;
      /*Disable Remote wakeup Feature*/
      USB_Device_dev.dev.DevRemoteWakeup = 0;
    }
    
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
  }
}

/**
  * @brief  JoyState
  * @param  None
  * @retval The direction value
*/
uint8_t JoyState(void)
{
  /* "right" key is pressed */
  if (STM_EVAL_PBGetState(Button_RIGHT))
  {
    return JOY_RIGHT;
  }
  /* "left" key is pressed */
  if (STM_EVAL_PBGetState(Button_LEFT))
  {
    return JOY_LEFT;
  }
  /* "up" key is pressed */
  if (STM_EVAL_PBGetState(Button_UP))    
  {
    return JOY_UP;
  }
  /* "down" key is pressed */
  if (STM_EVAL_PBGetState(Button_DOWN))
  {
    return JOY_DOWN;
  }
  /* No key is pressed */
  else
  {
    return 0;
  } 
}

/**
  * @brief  USBD_HID_GetPos
  * @param  None
  * @retval Pointer to report
*/
static uint8_t *USBD_HID_GetPos (void)
{
  int8_t  x = 0 , y = 0 ;
  static uint8_t HID_Buffer [4];
  
  switch (JoyState())
  {
  case JOY_LEFT:
    x -= CURSOR_STEP;
    break;  
    
  case JOY_RIGHT:
    x += CURSOR_STEP;
    break;
    
  case JOY_UP:
    y -= CURSOR_STEP;
    break;
    
  case JOY_DOWN:
    y += CURSOR_STEP;
    break;
    
  default:
    break;
  }
  HID_Buffer[0] = 0;
  HID_Buffer[1] = x;
  HID_Buffer[2] = y;
  HID_Buffer[3] = 0;
  return HID_Buffer; 
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f072.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
