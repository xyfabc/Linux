/**
  @page USBD_Composite  USB Device Composite example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the USB Device Composite (HID+CDC) example
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
In this Example the independent interfaces are : Custom HID (HID) and VCP (CDC).

This Example was created by combining the code in the usbd_vcp (virtual COM port) and usbd_hid (HID)

The implementation of the composite CDC-HID device is done by creating a wrapping
class (see folder hid_cdc_wrapper ) that makes calls to the separate CDC and HID class layer
handlers (files usb_cdc_core.c and usb_custom_hid_core.c files).

To test the demo, you can run Custom HID demo and at same time you can
do a USB to USART communication by opening a serial terminal window (like hyperterminal)
and selecting the STM32 virtual comport (you need to follow same steps as  the CDC virtual comport example).
   
To run this example, you will need to modify the following lines in the .inf file of the cdc driver:
stmcdc.inf (you will find it in "C:\Program Files\STMicroelectronics\Software\Virtual COM Port Driver).
 
The stmcdc.inf exists to assist in the loading of the proper USB serial drivers in the Windows OSs for 
the composite device to operate correctly as a virtual COM interface.

The following Lines associates the CDC device with the OS’s usbser.sys driver file 
and causes the OS to load this driver during the enumeration process.

The "MI" stands for Multiple Interface.


***********************************************
;VID/PID Settings
;------------------------------------------------------------------------------
[DeviceList.NT]
%DESCRIPTION%=DriverInstall,USB\VID_0483&PID_3256&MI_01

[DeviceList.NTamd64]
%DESCRIPTION%=DriverInstall,USB\VID_0483&PID_3256&MI_01

******************************************************
When even one of the interface class of the device is changed, it should be handled
differently by Windows. However, it doesn't recognize the modification. To avoid conflict on
Windows, we suggest to assign another VID/PID to the device or delete device instance from device manager.

@par Hardware and Software environment 

	- This example runs on STM32F072xB devices
	- This example has been tested with STM32072B-EVAL and can be easily tailored 
     to any other supported device and development board.

    - STM32072B-EVAL Set-up
       - Use CN4 connector to connect the board to a PC host through USB cable
       - Use CN10 connector to connect the board to PC (or to another evaluation board) over 
      RS232 (USART) serial cable.
       - Jumper JP7 should be connected in position 1-2.
       - For loopback mode test: remove RS232 cable on CN10 and connect directly USART
      TX and RX pins: PD5 and PD6 (with a cable or a jumper)

@par How to use it ?

 + EWARM
	- Open the usbd_composite_cdc_hid.eww workspace.  
    - Rebuild all files: Project->Rebuild all
	- Load project image: Project->Debug
	- Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the usbd_composite_cdc_hid.uvproj project 
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)    
	  Run->Debug (F11)
    
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
