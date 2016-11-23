/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Main program body.
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
#include "app.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
#define APPLICATION_ADDRESS     (uint32_t)0x08003000
#if   (defined ( __CC_ARM ))
  __IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));
#elif (defined (__ICCARM__))
#pragma location = 0x20000000
  __no_init __IO uint32_t VectorTable[48];
#elif defined ( __GNUC__ )
  __IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
#endif
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t i=0;
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f072.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */ 
  
  /* Copy the vector table from the Flash (mapped at the base of the application
  load address 0x08003000) to the base address of the SRAM at 0x20000000. */
  for(i = 0; i < 48; i++)
  {
    VectorTable[i] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));
  }
  
  /* Enable the SYSCFG peripheral clock*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
  
  /* Remap SRAM at 0x00000000 */
  SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
  
  /* Initialize Leds mounted on STM32072B-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* Turn on LED1 and LED3 */
  STM_EVAL_LEDOn(LED1);
  STM_EVAL_LEDOn(LED3);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }

  while (1)
  {
    /* Toggle LED2 and LED4 */
    STM_EVAL_LEDToggle(LED2);
    STM_EVAL_LEDToggle(LED4);

    /* Insert 50 ms delay */
    Delay(50);

    /* Toggle LED1 and LED3 */
    STM_EVAL_LEDToggle(LED1);
    STM_EVAL_LEDToggle(LED3);

    /* Insert 100 ms delay */
    Delay(100);
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement()
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
