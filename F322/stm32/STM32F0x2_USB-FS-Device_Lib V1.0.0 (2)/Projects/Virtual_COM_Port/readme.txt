/**
  @page USBD_VCP  USB Device VCP example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the USB VirtualComport Demo
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

This Demo presents the implementation of a Virtual Com Port (VCP) capability in 
the STM32F072 USB device. 

It illustrates an implementation of the CDC class following the PSTN subprotocol.
The VCP example allows the STM32 device to behave as a USB-to-RS232 bridge.
 - On one side, the STM32 communicates with host (PC) through USB interface in Device mode.
 - On the other side, the STM32 communicates with other devices (same host, other host,
   other devices…) through the USART interface (RS232).

The support of the VCP interface is managed through the ST Virtual Com Port driver
available for download from www.st.com.

When the VCP application starts, the USB device is enumerated as serial communication
port and can be configured in the same way (baudrate, data format, parity, stop bit
length…).

To test this example, you can use one of the following configurations:
 - Configuration 1: Connect USB cable to host and USART (RS232) to a different host
   (PC or other device) or to the same host. In this case, you can open two hyperterminal-like
   terminals to send/receive data to/from host to/from device.
 - Configuration 2: Connect USB cable to Host and connect USART TX pin to USART
   RX pin on the evaluation board (Loopback mode). In this case, you can open one
   terminal (relative to USB com port or USART com port) and all data sent from this
   terminal will be received by the same terminal in loopback mode. This mode is useful
   for test and performance measurements.

To use a different CDC communication interface you can use the template CDC 
interface provided in folder \Libraries\STM32_USB_Device_Library\Class\cdc.


@note When transferring a big file (USB OUT transfer) user has to adapt the size
      of IN buffer, for more details refer to usbd_conf.h file (APP_RX_DATA_SIZE constant).    

	
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
    - Open the usbd_virtualcomport.eww workspace.
    - In the workspace toolbar select the project config: 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the usbd_virtualcomport.uvproj project
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
