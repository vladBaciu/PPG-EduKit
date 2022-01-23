/**
 *   @file    SerialFrame.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   Serial wrapper for data transfer
 *   @details Transfer data in a frame based manner
 *            
 */

/*==================================================================================================
 *
 * Copyright Vrije Universiteit Brussel, 2021
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 *  ==============================================================================================*/


/*==================================================================================================
 *                                        INCLUDE FILES
 * 1) system and project includes
 * 2) needed interfaces from external units
 * 3) internal and external interfaces from this unit
 *  ==============================================================================================*/

#include "serial_frame.h"
#include "Uart_Printf.h"
/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

/* @brief Frame flags. Same as in the Python application */
#define FRAME_START               (0xDA)
#define FRAME_TERMINATOR_1        (0xEA)
#define FRAME_TERMINATOR_2        (0xDC)

/*==================================================================================================
 *                                      LOCAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      LOCAL VARIABLES
 *  ==============================================================================================*/



/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      GLOBAL VARIABLES
 *  ==============================================================================================*/

/*==================================================================================================
 *                                   LOCAL FUNCTION PROTOTYPES
 *  ==============================================================================================*/

/*==================================================================================================
 *                                   GLOBAL FUNCTION PROTOTYPES
 *  ==============================================================================================*/

/*==================================================================================================
 *                                       LOCAL FUNCTIONS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       GLOBAL FUNCTIONS
 *  ==============================================================================================*/

/***************************************************************************
 * Function: createSerialFrame  
 *
 * Description: Create the type of frame according to the frameType parameter
 **************************************************************************/
uint8_t* createSerialFrame(void *inputData, uint16_t noOfBytes, frameParams_t *serialFrameStruct)
{
  static uint8_t serialFrame[SERIAL_FRAME_LENGTH_MAX + 5];
  if( SERIAL_FRAME_LENGTH_MAX + 6 < noOfBytes)
    serialFrameStruct->frameType = 0xFF;
    
  serialFrame[0] = FRAME_START;
  
  switch(serialFrameStruct->frameType)
  {
    case CHANNEL_DATA:
        serialFrame[1] = CHANNEL_DATA;
        serialFrame[2] = serialFrameStruct->sensor;
        serialFrame[3] = serialFrameStruct->tissueDetected;
        serialFrame[4] = serialFrameStruct->params.wavelength;
        memcpy(&serialFrame[5], inputData, noOfBytes);
        serialFrame[noOfBytes + 5] = FRAME_TERMINATOR_1;
        serialFrame[noOfBytes + 6] = FRAME_TERMINATOR_2;
        break;
        
    case PARAMS:
        serialFrame[1] = PARAMS;
        serialFrame[2] = serialFrameStruct->sensor;
        serialFrame[3] = serialFrameStruct->tissueDetected;
        serialFrame[4] = serialFrameStruct->params.hr_spo2[0];
        serialFrame[5] = serialFrameStruct->params.hr_spo2[1];
        serialFrame[6] = serialFrameStruct->params.hr_spo2[2];
        serialFrame[7] = FRAME_TERMINATOR_1;
        serialFrame[8] = FRAME_TERMINATOR_2;
        break;
        
    case DEBUG_FRAME:
        break;
        
    default:
        serialFrame[1] = 0xDE;
        serialFrame[2] = 0xAD;
        serialFrame[3] = 0xDE;
        serialFrame[4] = 0xAD;
        serialFrame[5] = FRAME_TERMINATOR_1;
        serialFrame[6] = FRAME_TERMINATOR_2;
        break;
  }

  return serialFrame;
}

/***************************************************************************
 * Function: sendFrame  
 *
 * Description: Sends the buffer frame untill the end terminators are detected
 **************************************************************************/
void sendFrame(uint8_t *pFrame)
{ 
  bool terminator_1 = FALSE;
  bool endOfFrame = FALSE;
  
  while(!endOfFrame)
  {

    
    printf("%02x", *pFrame);
    
    if(FRAME_TERMINATOR_1 == *pFrame)
    {
      terminator_1 = TRUE;
    }

    if((TRUE == terminator_1) && (FRAME_TERMINATOR_2 == *pFrame))
    {
      endOfFrame = TRUE;
    }
   
    pFrame++;

  }
  printf("\n");
}


/*******************************************************************************
* When printf function is called it is redirected to the following functions
* depending on compiler used.
*******************************************************************************/
#if defined (__GNUC__)
/*******************************************************************************
* Function Name: _write
********************************************************************************
* Summary: 
* NewLib C library is used to retarget printf to _write. printf is redirected to 
* this function when GCC compiler is used to print data to terminal using UART. 
*
* \param file
* This variable is not used.
* \param *ptr
* Pointer to the data which will be transfered through UART.
* \param len
* Length of the data to be transfered through UART.
*
* \return
* returns the number of characters transferred using UART.
* \ref int
*******************************************************************************/
int _write(int file, char *ptr, int len)
{
        int nChars = 0;

        /* Suppress the compiler warning about an unused variable. */
        if (0 != file)
        {
        }
                
        nChars = Cy_SCB_UART_PutArray(Uart_Printf_HW, ptr, len);
           
        return (nChars);
}
#elif defined(__ARMCC_VERSION)
    
/*******************************************************************************
* Function Name: fputc
********************************************************************************
* Summary: 
* printf is redirected to this function when MDK compiler is used to print data
* to terminal using UART.
*
* \param ch
* Character to be printed through UART.
*
* \param *file
* pointer to a FILE object that identifies the stream where the character is to be
* written.
*
* \return
* This function returns the character that is written in case of successful
* write operation else in case of error EOF is returned.
* \ref int
*******************************************************************************/
    struct __FILE
    {
        int handle;
    };
    
    enum
    {
        STDIN_HANDLE,
        STDOUT_HANDLE,
        STDERR_HANDLE
    };
    
    FILE __stdin = {STDIN_HANDLE};
    FILE __stdout = {STDOUT_HANDLE};
    FILE __stderr = {STDERR_HANDLE};
    
    int fputc(int ch, FILE *file)
    {
        int ret = EOF;
        switch(file->handle)
        {
            case STDOUT_HANDLE:
                while (0UL == Cy_SCB_UART_Put(Uart_Printf_HW, ch))
                {
                }
                ret = ch;
            break;
                
            case STDERR_HANDLE:
                ret = ch;
            break;
                
            default:
                file = file;
            break;
        }
        return(ret);
    }
    

#endif /* (__GNUC__) */