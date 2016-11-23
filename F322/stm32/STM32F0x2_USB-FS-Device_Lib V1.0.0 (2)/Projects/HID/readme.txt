/**
  @page USBD_HID  USB Device HID example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the USB Device HID (Joystick) example
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

This example provides a description of how to use the USB-FS-Device peripheral 
on the STM32F072 device.
	
The STM32 device is enumerated as an USB Device Joystick Mouse, that uses the
native PC Host HID driver.

The Joystick mounted on the STM32072B-EVAL board is used to emulate the Mouse directions.

This example supports remote wakeup (which is the ability of a USB device to bring
a suspended bus back to the active condition), and the tamper push-button is used as
the remote wakeup source.

By default, in Windows powered PC the Power Management feature of USB mouse devices
is turned off. This setting is different from classic PS/2 computer functionality. 
Therefore, to enable the Wake from standby option, you must manually turn on the
Power Management feature for the USB mouse.

To manually enable the Wake from standby option for the USB mouse, 
	- Start "Device Manager",
	- Select "Mice and other pointing devices",
	- Select the "HID-compliant mouse" device (make sure that PID &VID are equal to 0x5710 & 0x0483 respectively) 
	- Right click and select "Properties", 
	- Select "Power Management" tab,
	- Finally click to select "Allow this device to wake the system from standby" check box.

@par Hardware and Software environment 

   - This example runs on STM32F072xB devices
   - This example has been tested with STM32072B-EVAL and can be easily tailored
     to any other supported device and development board.

   - STM32072B-EVAL Set-up
       - Use CN4 connector to connect the board to a PC host through USB cable

@par How to use it ?

 + EWARM
    - Open the usbd_hid.eww workspace.
    - In the workspace toolbar select the project config: 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
	- Open the usbd_hid.uvproj project
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
