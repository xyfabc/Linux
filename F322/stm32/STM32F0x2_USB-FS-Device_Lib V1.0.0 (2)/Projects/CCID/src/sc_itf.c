/**
  ******************************************************************************
  * @file    sc_itf.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   SmartCard Interface file
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
#include "sc_itf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Directories & Files ID */
const uint8_t MasterRoot[2] = {0x3F, 0x00};
const uint8_t GSMDir[2] = {0x7F, 0x20};
const uint8_t ICCID[2] = {0x2F, 0xE2};
const uint8_t IMSI[2] = {0x6F, 0x07};
const uint8_t CHV1[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/* State Machine for the SmartCard Interface */
SC_State SCState = SC_POWER_OFF;

/* APDU Transport Structures */
SC_ADPU_Commands SC_ADPU;
SC_ADPU_Response SC_Response;
SC_Param_t  SC_Param;
Protocol0_DataStructure_t Protocol0_DataStructure;

__IO uint32_t CardInserted = 0;
uint32_t CHV1Status = 0, i = 0;
__IO uint8_t ICCID_Content[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__IO uint8_t IMSI_Content[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Delay (uint32_t msec); 

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Manages the Warm and Cold Reset and get the Answer to Reset from ICC 
  * @param  uint8_t voltage required by host 
  * @retval None 
  */
void SC_AnswerToReset (uint8_t voltage)
{
  SCState = SC_POWER_ON;
  
  SC_ADPU.Header.CLA = 0x00;
  SC_ADPU.Header.INS = SC_GET_A2R;
  SC_ADPU.Header.P1 = 0x00;
  SC_ADPU.Header.P2 = 0x00;
  SC_ADPU.Body.LC = 0x00;
  
  /* Power ON the card */
  SC_PowerCmd(ENABLE);
  
  /* Configure the Voltage, Even if IO is still not configured */
  SC_VoltageConfig(voltage);    
  
  while ((SCState != SC_ACTIVE_ON_T0) && (SCState != SC_NO_INIT))
  { /* If Either The Card has become Active or Become De-Active */
    SC_Handler(&SCState, &SC_ADPU, &SC_Response);
  }
  
  if (SCState == SC_ACTIVE_ON_T0)
  {
    SC_UpdateParams();
    /* Apply the Procedure Type Selection (PTS) */
    SC_PTSConfig();
    SC_SaveVoltage(voltage);   /* Save Voltage for Future use */
  }
          
  /* Inserts delay(400ms) for Smartcard clock resynchronization */
  Delay(400);
}


/**
  * @brief  Provides the value of SCState variable 
  * @param  None
  * @retval uint8_t SCState 
  */
uint8_t SC_GetState (void)
{
  return SCState;
}

/**
  * @brief  Set the value of SCState variable to Off
  * @param  uint8_t scState : value of SCState to be updated 
  * @retval None
  */
void SC_SetState (uint8_t scState)
{
  SCState = (SC_State)scState;
}

/**
  * @brief  Configure the CMDVCC state for right detection of Card Insertion
  * @param  None 
  * @retval None
  */
void SC_ConfigDetection (void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(SC_OFF_GPIO_CLK | SC_CMDVCC_GPIO_CLK | 
                        SC_3_5V_GPIO_CLK | SC_RESET_GPIO_CLK, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Smartcard CMDVCC pin */
  GPIO_InitStructure.GPIO_Pin = SC_CMDVCC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SC_CMDVCC_GPIO_PORT, &GPIO_InitStructure);

  /* Configure Smartcard Reset pin */
  GPIO_InitStructure.GPIO_Pin = SC_RESET_PIN;
  GPIO_Init(SC_RESET_GPIO_PORT, &GPIO_InitStructure);

  /* Configure Smartcard 3/5V pin */
  GPIO_InitStructure.GPIO_Pin = SC_3_5V_PIN;
  GPIO_Init(SC_3_5V_GPIO_PORT, &GPIO_InitStructure);

  /* Select 5V */ 
  SC_VoltageConfig(SC_VOLTAGE_5V);

  /* Disable CMDVCC */
  SC_PowerCmd(DISABLE);

  /* Set RSTIN HIGH */  
  SC_Reset(Bit_SET);
    						                                
  /* Configure Smartcard OFF pin */
  GPIO_InitStructure.GPIO_Pin = SC_OFF_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SC_OFF_GPIO_PORT, &GPIO_InitStructure);
  
  /* Configure EXTI line connected to Smartcard OFF Pin */
  SYSCFG_EXTILineConfig(SC_OFF_EXTI_PORT_SOURCE, SC_OFF_EXTI_PIN_SOURCE);

  EXTI_ClearITPendingBit(SC_OFF_EXTI_LINE);
  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_Line = SC_OFF_EXTI_LINE;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Clear the SC_OFF_EXTI_IRQn Pending Bit */
  NVIC_ClearPendingIRQ(SC_OFF_EXTI_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = SC_OFF_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
    
}

/**
  * @brief  Saves the voltage value to be saved for further usage 
  * @param  uint8_t voltage : voltage value to be saved for further usage 
  * @retval None
  */
void SC_SaveVoltage (uint8_t voltage)
{
  SC_Param.voltage = voltage;
}

/**
  * @brief  Save the A2R Parameters for further usage 
  * @param  None
  * @retval None
  */
void SC_UpdateParams (void)
{
  /*
  FI, the reference to a clock rate conversion factor
  over the bits b8 to b5
  DI, the reference to a baud rate adjustment factor
  over the bits b4 to bl
  */
  SC_Param.SC_A2R_FiDi = SC_A2R.T[0].InterfaceByte[0].Value;
  SC_Param.SC_hostFiDi = SC_A2R.T[0].InterfaceByte[0].Value;
  
  Protocol0_DataStructure.bmFindexDindex= SC_A2R.T[0].InterfaceByte[0].Value ;
}

/**
  * @brief  Initialize the parameters structures  
  * @param  None
  * @retval None
  */
void SC_InitParams (void)
{
  /*
 FI, the reference to a clock rate conversion factor
  over the bits b8 to b5
- DI, the reference to a baud rate adjustment factor
  over the bits b4 to bl
  */
  
  SC_Param.SC_A2R_FiDi = DEFAULT_FIDI;
  SC_Param.SC_hostFiDi = DEFAULT_FIDI;
  
  Protocol0_DataStructure.bmFindexDindex=DEFAULT_FIDI;
  
  /* Placeholder, Ignored */
  /* 0 = Direct, first byte of the ICC’s ATR data. */
  Protocol0_DataStructure.bmTCCKST0 = DEFAULT_T01CONVCHECKSUM;  

  /* Extra GuardTime = 0 etu */
  Protocol0_DataStructure.bGuardTimeT0 = DEFAULT_EXTRA_GUARDTIME; 
  Protocol0_DataStructure.bWaitingIntegerT0=DEFAULT_WAITINGINTEGER;
  Protocol0_DataStructure.bClockStop =0; /* Stopping the Clock is not allowed */
}

/**
  * @brief  SC_SetParams
  *         Set the parameters for CCID/USART interface 
  * @param  Protocol0_DataStructure_t* pPtr: pointer to buffer containing the 
  *          parameters to be set in USART
  * @retval uint8_t status value
  */
uint8_t SC_SetParams (Protocol0_DataStructure_t* pPtr)
{
  uint16_t guardTime;   /* Keep it 16b for handling 8b additions */
  uint32_t fi_new;
  uint32_t di_new;
  
  guardTime = pPtr->bGuardTimeT0;
  if (guardTime > MAX_EXTRA_GUARD_TIME )
  {
    return SLOTERROR_BAD_GUARDTIME;
  }
  
  fi_new = pPtr->bmFindexDindex;
  di_new = pPtr->bmFindexDindex;
  
  /* Check for the FIDI Value set by Host */
  di_new &= (uint8_t)0x0F;
  if (SC_GetDTableValue(di_new) == 0)
  {
    return SLOTERROR_BAD_FIDI;
  }
  
  fi_new >>= 4;
  fi_new &= (uint8_t)0x0F; 
  
  if ( SC_GetDTableValue(fi_new) == 0)
  {
    return SLOTERROR_BAD_FIDI;
  }
  
  /* Put Total GuardTime in USART Settings */
  USART_SetGuardTime(SC_USART, (uint8_t)(guardTime + DEFAULT_EXTRA_GUARDTIME) );
  
  /* Save Extra GuardTime Value */
  Protocol0_DataStructure.bGuardTimeT0 = guardTime; 
  
  Protocol0_DataStructure.bmTCCKST0 = pPtr->bmTCCKST0;
  Protocol0_DataStructure.bClockStop = pPtr->bClockStop;
  
  /* Save New bmFindexDindex */
  SC_Param.SC_hostFiDi = pPtr->bmFindexDindex;
  SC_PTSConfig();
  
  Protocol0_DataStructure.bmFindexDindex = pPtr->bmFindexDindex;
     
  return SLOT_NO_ERROR;
}

/**
  * @brief  SC_ExecuteEscape
  *         SC_ExecuteEscape function from the host. This is user implementable  
  * @param  uint8_t* ptrEscape: pointer to buffer containing the Escape data 
  * @param  uint32_t escapeLen: length of escaped data
  * @param  uint8_t* responseBuff: pointer containing escape buffer response
  * @param  uint16_t* responseLen: length of escape response buffer 
  * @retval uint8_t status value
  */
uint8_t SC_ExecuteEscape (uint8_t* ptrEscape, uint32_t escapeLen, 
                          uint8_t* responseBuff,
                          uint16_t* responseLen)
{
  uint32_t idx;
  
  /* Manufacturer specific implementation ... */

  /* Do a loopback for example..*/
  /* To be Modified for end application */
  for (idx =0; idx<escapeLen; idx++)
  {
    *responseBuff++ = *ptrEscape++;
  }
  
  *responseLen = escapeLen;   /* To be Modified for end application */
  return SLOT_NO_ERROR;
}


/**
  * @brief  SC_SetClock function to define Clock Status request from the host. 
  *         This is user implementable  
  * @param  uint8_t bClockCommand: Clock status from the host 
  * @retval uint8_t status value
  */
uint8_t SC_SetClock (uint8_t bClockCommand) 
{
    /* bClockCommand • 00h restarts Clock
                     • 01h Stops Clock in the state shown in the bClockStop 
                       field of the PC_to_RDR_SetParameters command 
                       and RDR_to_PC_Parameters message.*/
  
  if (bClockCommand == 0)
  {
    /* 00h restarts Clock : Since Clock is always running, PASS this command */
     return SLOT_NO_ERROR;    
  }
  else if (bClockCommand == 1)
  {
    return SLOTERROR_BAD_CLOCKCOMMAND;
  }
    
  return SLOTERROR_CMD_NOT_SUPPORTED;
}

/**
  * @brief  SC_XferBlock function from the host. 
  *          This is user implementable  
  * @param  uint8_t* ptrBlock : Pointer containing the data from host 
  * @param  uint32_t blockLen : length of block data for the data transfer 
  * @param  uint16_t expectedLen: expected length of data transfer 
  * @retval uint8_t status value
  */
uint8_t SC_XferBlock (uint8_t* ptrBlock, uint32_t blockLen, uint16_t expectedLen)
{
  uint32_t index;
  uint32_t cmdType;
    
  SC_ADPU.Header.CLA = *ptrBlock++;
  SC_ADPU.Header.INS = *ptrBlock++;
  SC_ADPU.Header.P1 = *ptrBlock++;
  SC_ADPU.Header.P2 = *ptrBlock++;
  
  /************ Check for the Command Types from the Host **************/
  if ( (SC_ADPU.Header.INS == SC_SELECT_FILE) || \
       (SC_ADPU.Header.INS == SC_ENABLE) || \
       (SC_ADPU.Header.INS == SC_VERIFY))
  {
    cmdType = XFER_BLK_SEND_DATA;    /* Set the Flag as 1 for commands which issue data 
                               to the card */
  }
  else if ((SC_ADPU.Header.INS == SC_GET_RESPONCE) || \
           (SC_ADPU.Header.INS == SC_READ_BINARY))
  {
    cmdType = XFER_BLK_RECEIVE_DATA;
  }
  else
  {
    cmdType = XFER_BLK_NO_DATA;
  }
  
  if (blockLen == 0x04)
  { /* Case: no data to exchange with ICC, only header sent by Host= 4Bytes
       Command TPDU = CLA INS P1 P2, the CCID is responsible to add P3 =00h */
    SC_ADPU.Body.LC = 0;
    SC_ADPU.Body.LE = 0;
  }
  
  /************ Process the commands based on their Types from the Host ********/
  if (cmdType == XFER_BLK_RECEIVE_DATA)
  {
    /*
    Form 2, data expected from ICC:
    Command TPDU = CLA INS P1 P2 Le, Le=P3 from 00h to FFh (00h means 100h)
    Response TPDU = Data(Le) SW1 SW2, Data(Le) is for the Le data received from 
    the ICC or empty if ICC rejects the command.   */
    SC_ADPU.Body.LE = *ptrBlock++;
    SC_ADPU.Body.LC = 0x00;  /* To be compatible with Lower Layer, Reset to 0 */
    
    SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    
    for (index=0; index < (SC_ADPU.Body.LE) ; index++)
    {/* Copy the ATR to the Response Buffer */
      Ccid_bulkin_data.abData[index] = SC_Response.Data[index];
    }
    
    Ccid_bulkin_data.abData[index] = SC_Response.SW1;
    index++;
    Ccid_bulkin_data.abData[index] = SC_Response.SW2;
    
    /* Response Len = Expected Len + SW1, SW2*/    
    Ccid_bulkin_data.dwLength= (SC_ADPU.Body.LE) + 2;  
    
  }
  else 
  {
    /* cmdType == XFER_BLK_SEND_DATA, XFER_BLK_NO_DATA*/
    /*  
      Form 3, data are to be sent to the ICC:
      Command TPDU = CLA INS P1 P2 Lc Data(Lc), Lc=P3 from 01h to FFh and Data(Lc) 
       for the Lc data to send to the ICC.
      Response TPDU = SW1 SW2  */
    
    SC_ADPU.Body.LC = *ptrBlock++;
    SC_ADPU.Body.LE = 0;     /* To be compatible with Lower Layer, Reset to 0 */
    
    for(index = 0; index < SC_ADPU.Body.LC; index++)
    {
      SC_ADPU.Body.Data[index] = *ptrBlock++;
    }
    
    while(index < LC_MAX) 
    {    
      SC_ADPU.Body.Data[index++] = 0;
    } 
    /******* Call the API from Lower Layer to process the command ********/
    SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    Ccid_bulkin_data.abData[0] = SC_Response.SW1;
    Ccid_bulkin_data.abData[1] = SC_Response.SW2;
    Ccid_bulkin_data.dwLength=2;    
  }
  
  return SLOT_NO_ERROR;
}

/**
  * @brief  Class Specific Request from the host to provide supported Clock Freq
  *          This is Optional function & user implementable  
  * @param  uint8_t* pbuf : Pointer to be updated by function which points to the 
  *                         supported clock frequencies 
  * @param  uint16_t* len : length of data to be transferred to host 
  * @retval uint8_t status value
  */
uint8_t SC_Request_GetClockFrequencies(uint8_t* pbuf, uint16_t* len)
{ /* User have to fill the pbuf with the GetClockFrequency data buffer */
  
  *len = 0;  
  return 0;
}

/**
  * @brief  Class Specific Request from the host to provide supported data rates 
  *          This is Optional function & user implementable  
  * @param  uint8_t* pbuf : Pointer to be updated by function which points to the 
  *                         supported data rates 
  * @param  uint16_t* len : length of data to be transferred to host 
  * @retval uint8_t status value
  */
uint8_t SC_Request_GetDataRates(uint8_t* pbuf, uint16_t* len)
{
  /* User have to fill the pbuf with the GetDataRates data buffer */
  
  *len = 0;
  return 0;
}

/**
  * @brief  Class Specific Request from the host to provide supported data rates 
  *          This is Optional function & user implementable  
  * @param  uint8_t bmChanges : value specifying which parameter is valid in 
  *                    command among next bClassGetResponse, bClassEnvelope
  * @param  uint8_t* bClassGetResponse : Value to force the class byte of the 
  *                     header in a Get Response command.
  * @param  uint8_t* bClassEnvelope : Value to force the class byte of the header
  *                     in a Envelope command.
  * @retval uint8_t status value
  */
uint8_t SC_T0Apdu(uint8_t bmChanges, uint8_t bClassGetResponse, 
                  uint8_t bClassEnvelope)
{
  /* User have to fill the pbuf with the GetDataRates data buffer */
  
  if (bmChanges == 0)
  { /* Bit cleared indicates that the associated field is not significant and 
       that default behaviour defined in CCID class descriptor is selected */
    return SLOT_NO_ERROR;
  }
  
  return SLOTERROR_CMD_NOT_SUPPORTED;
}


/**
  * @brief  Mechanical Function being requested by Host 
  *          This is Optional function & user implementable  
  * @param  uint8_t bFunction : value corresponds to the mechanical function 
  *                       being requested by host
  * @retval uint8_t status value
  */
uint8_t SC_Mechanical(uint8_t bFunction) 
{
  
  return SLOTERROR_CMD_NOT_SUPPORTED;
}

/**
  * @brief  SC_SetDataRateAndClockFrequency
  *         Set the Clock and data Rate of the Interface 
  *          This is Optional function & user implementable  
  * @param  uint32_t dwClockFrequency : value of clock in kHz requested by host
  * @param  uint32_t dwDataRate : value of data rate requested by host
  * @retval uint8_t status value
  */
uint8_t SC_SetDataRateAndClockFrequency(uint32_t dwClockFrequency, 
                                        uint32_t dwDataRate) 
{
  /* User have to fill the pbuf with the GetDataRates data buffer */
  
  if ((dwDataRate == DEFAULT_DATA_RATE) && 
      (dwClockFrequency == DEFAULT_CLOCK_FREQ))
  {
    return SLOT_NO_ERROR;
  }

  return SLOTERROR_CMD_NOT_SUPPORTED;
}

/**
  * @brief  SC_Secure
  *         Process the Secure command 
  *          This is Optional function & user implementable  
  * @param  uint32_t dwLength : length of data from the host 
  * @param  uint8_t bBWI : Block Waiting Timeout sent by host
  * @param  uint16_t wLevelParameter : Parameters sent by host
  * @param  uint8_t pbuf : buffer containing the data
  * @param  uint32_t* returnLen : Length of data expected to return
  * @retval uint8_t status value
  */
uint8_t SC_Secure(uint32_t dwLength, uint8_t bBWI, uint16_t wLevelParameter, 
                    uint8_t* pbuf, uint32_t* returnLen )
{
  *returnLen =0;
  return SLOTERROR_CMD_NOT_SUPPORTED;
}

/**
  * @brief  Delay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
static void Delay (uint32_t msec)
{
  USB_BSP_mDelay(msec);   
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
