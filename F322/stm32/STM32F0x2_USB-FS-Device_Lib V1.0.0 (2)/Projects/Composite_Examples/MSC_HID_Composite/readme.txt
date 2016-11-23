/**
  @page USBD_Composite  USB Device Composite example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the USB Device Composite (MSC+HID) example
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

The examples provide a composite device.
A composite device is one that has multiple interfaces controlled independently of each other.
Using composite device, multiple functions are combined into a single device.
In this Example the independent interfaces are : Mass Storage (MSC) and (HID).

This Example was created by combining the code in the usbd_hid (HID) and usbd_msc (mass storage)

The implementation of the composite HID-MSC device is done by creating a wrapping
class (see folder msc_hid_wrapper ) that makes calls to the separate HID and MSC class layer
handlers (files usbd_custom_hid_core.c and usb_msc_core.c files).

To test the demo, you can access to SDcard content and in same time you can run Custom HID demo.

When even one of the interface class of the device is changed, it should be handled
differently by Windows. However, it doesn't recognize the modification. To avoid conflict on
Windows, we suggest to assign another VID/PID to the device or delete device instance from device manager.  

@par Hardware and Software environment 

	- This example runs on STM32F072xB devices
	- This example has been tested with STM32072B-EVAL and can be easily tailored 
     to any other supported device and development board.

    - STM32072B-EVAL Set-up
       - Use CN4 connector to connect the board to a PC host through USB cable
       - Insert the MicroSD card in the CN9 connector (right side of the board).

@par How to use it ?

 + EWARM
	- Open the usbd_composite_msc_hid.eww workspace.  
    - Rebuild all files: Project->Rebuild all
	- Load project image: Project->Debug
	- Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the usbd_composite_msc_hid.uvproj project 
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
