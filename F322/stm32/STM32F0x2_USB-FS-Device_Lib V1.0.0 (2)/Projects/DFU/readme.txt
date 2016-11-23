/**
  @page USBD_DFU  USB Device DFU example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the USB Device DFU (Device Firmware Upgrade) example
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

This Demo presents the implementation of a device firmware upgrade (DFU) capability
in the STM32F072 USB device.

It follows the DFU class specification defined by the USB Implementers Forum for 
reprogramming an application through USB-FS-Device. 
The DFU principle is particularly well suited to USB-FS-Device applications that 
need to be reprogrammed in the field

To have an example of how to generate a DFU image, please refer to the project 
provided in folder "binary_template". This project allows to generate a binary
image (*.bin file) that can be used to generate a *.dfu image using the tool
provided by ST: "DFUse\DFU File Manager" available for download from www.st.com

Once the *.dfu image is generated, it can be downloaded into internal flash memory
using the ST tool "DFUse\DFUse Demonstration" or the tool "DFUse\STDFU Tester" both
available for download from www.st.com

The supported memory for this example is the Internal Flash memory
To add a new memory interface you can use the template memory interface provided 
in folder \Libraries\STM32_USB_Device_Library\Class\dfu.

@note After each device reset, hold down the TAMPER push-button on the STM32072B-EVAL board
     
@par Hardware and Software environment 

   - This example runs on STM32F072xB devices
   - This example has been tested with STM32072B-EVAL and can be easily tailored
     to any other supported device and development board.

   - STM32072B-EVAL Set-up
       - Use CN4 connector to connect the board to a PC host through USB cable

@par How to use it ?

 + EWARM
    - Open the usbd_dfu.eww workspace.
    - In the workspace toolbar select the project config: 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the usbd_dfu.uvproj project
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
