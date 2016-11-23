/**
  @page USBD_HID  USB Device Custom HID example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the USB Custom_HID Demo
  ******************************************************************************
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
   @endverbatim

   
@par Example Description 

This Demo provides a description of how to use the USB-FS-Device on the STM32F072 device.
The STM32 device is enumerated as HID compliant device, that uses the native PC 
Host USB-FS-Device HID driver.

To test the example, you need to:

  1)- Start the "USB HID Demonstrator" PC applet and connect STM32 USB to PC
  2)- The device should be detected and shown in the USB HID target box
  3)- Press Graphic View button
  4)- You can Select "use SET_FEATURE" or "use SET_REPORT" in order to use SET_REPORT request to send HID Report
       for LED control
  5)- You can use the potentiometer of the evaluation board to transfer the result of the converted voltage (By the ADC)
  the to the PC host (these values are sent to the PC using the endpoint1 IN)   
  5)- Make sure that following report ID are configured: LED1 ID (0x1) , LED2 ID(0x2),
       LED3 ID(0x3), LED4 ID(0x4), BUTTON1_ID(0x6) and Potentiometer_ID(0x7).
  6)- Select Leds to switch on/off on the EVAL board => a SET_REPORT request will be sent


@par Hardware and Software environment 

   - This example runs on STM32F072xB devices
   - This example has been tested with STM32072B-EVAL and can be easily tailored
     to any other supported device and development board.

   - STM32072B-EVAL Set-up
       - Use CN4 connector to connect the board to a PC host through USB cable


@par How to use it ?

 + EWARM
    - Open the usbd_custom_hid.eww workspace.
    - In the workspace toolbar select the project config: 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
	- Open the usbd_custom_hid.uvproj project
	- Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)    

 + TrueSTUDO
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
    - Click on File->Import, select General->'Existing Projects into Workspace' and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory, select the project.
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Run->Debug (F11)
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
