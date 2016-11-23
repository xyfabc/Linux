/**
  @page USBD_AUDIO_Streaming  USB Device Audio Streaming Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Description of the STM32072 USB Device Audio Streaming Example
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

This example presents the implementation of an USB Device audio streaming (Out: Speaker/Headset)
on STM32F072 devices.

This example uses the DAC interface to stream audio data from USB to the audio
amplifier implemented on the evaluation board. The audio downstream is driven
with 48 kHz sampling rate, a 16-bit depth, and 2 channels (stereo) to headset.

It supports single audio frequency for the output: the host PC driver manages the
sampling rate conversion from original audio file sampling rate to the sampling
rate supported by the example.  
It is advised to set high audio frequencies to guarantee high audio quality.
It is also possible to modify the default volume through define DEFAULT_VOLUME in 
file usbd_conf.h.

This example provides a synchronization mechanism allowing to overcome the
difference between USB clock domain and STM32 clock domain:
   
  Clock update synchronization consists on slightly modifying the period of the
  timer triggering the DAC peripheral. This solution is based on speeding-up and
  slowing-down the trigger clock in order to match the USB clock domain.
  This clock allows to keep all data samples (no data loss) but modifies slightly
  the audio frequency (this modification is not perceived by human ear).
  For this method, the distance between write pointer and read pointer is periodically 
  measured and depending on the measured distance a correction action is performed:  
    - If the measured distance is higher than 3/4 buffer size, the timer (trigger of DAC) is speed-up
    - If the measured distance is lower than 1/4 buffer size, the timer (trigger of DAC) is slow-down
      
For the application configuration, a single file may be modified: audio_app_conf.h 
which contains most needed defines gathered from all configuration files.
Defines that are not gathered in this file are in general optional configuration
or not modifiable parameters.

How to check the efficiency of synchronization mechanisms:
----------------------------------------------------------
The example provides simple ways to check efficiency of the synchronization mechanisms:
 - Two LEDs (LED1 and LED2) are used to monitor two main events: SOF event and 
   Frame Transfer Complete event. If the SOF and Out flows are well synchronized,
   these two events shall be in phase. This can be monitored by connecting oscilloscope
   probes to the LEDs pins. When synchronization mechanism is selected, variations
   in the Frame Transfer Complete event will be observed in such a way it is continuously
   resynchronized with the SOF event.
   If no synchronization method is selected, then the Frame Transfer Complete event
   will be desynchronized relatively to SOF event.
  
Another way to check the efficiency is to play a sine wave and connect oscilloscope
probe to Audio_OUT_L(PA4) and/or Audio_OUT_R(PA5) to compare the obtained waveform
with the original.


@par Known limitations

   - The following situation has not been validated: When dynamic frequency switching
     is enabled (SUPPORTED_FREQ_NBR define in usbd_conf.h is higher than 1) and when
     the host uses this feature to optimize the bus usage by switch audio frequency
     multiple times while the audio file is playing (Windows XP SP3 and Windows 7 
     drivers don't use this mechanism). 
   - While application is playing an audio stream, if the USB cable is unplugged
     without stopping the audio stream and ejecting the device correctly, then
     a "noise" may result from this operation. This issue is due to missing
     management of cable disconnected event(will be fixed in next versions)


@par Hardware and Software environment 

   - This example runs on STM32F072xB devices
   - This example has been tested with STM32072B-EVAL and can be easily tailored
     to any other supported device and development board.

   - STM32072B-EVAL Set-up
     - Use CN14 connector to connect an audio headset to the STM32072B-EVAL board
     - Use CN4 connector to connect the board to a PC host through USB cable
     - Set JP18 in position 2-3


@par How to use it ?

 + EWARM
    - Open the usbd_audio.eww workspace.
    - In the workspace toolbar select the project config: 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)
    - Connect the board to a PC with a USB cable through CN4
    - Connect the audio headset or a speaker to audio jack connector CN14
    - Reset the board then start an audio player in the PC host

 + MDK-ARM
	- Open the usbd_audio.uvproj project
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
