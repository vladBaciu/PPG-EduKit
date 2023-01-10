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

#ifndef SERIALFRAME_H
#define SERIALFRAME_H

/*==================================================================================================
 *                                        INCLUDE FILES
 *  ==============================================================================================*/

#include "utils.h"
    
/*==================================================================================================
 *                                          CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      DEFINES AND MACROS
 *  ==============================================================================================*/
    
#define SERIAL_FRAME_LENGTH_MAX   (300U)
    
/*==================================================================================================
 *                                             ENUMS
 *  ==============================================================================================*/

/* @brief Enum with frame types */
typedef enum 
{ 
    CHANNEL_DATA = 0x7C, 
    PARAMS = 0x83, 
    DEBUG_FRAME = 0xF2
} frameType_t;

typedef enum
{
    CUSTOM_SENSOR = 0x64,
    MAX_SENSOR = 0x21
} sensorType_t;

/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 *  ==============================================================================================*/

/* @brief Store params for the debug frame. Not supported yet. */
typedef struct
{
  uint8_t dummy;
}debugType_t;



/* @brief Store params for differend kind of serial frames */
typedef struct
{
  frameType_t frameType;
  sensorType_t sensor;
  union
  {
    uint8_t hr_spo2[3];
    uint8_t wavelength;
    debugType_t debugParam;
  }params;
  bool tissueDetected;
}frameParams_t;

/*==================================================================================================
 *                                GLOBAL VARIABLE DECLARATIONS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 *  ==============================================================================================*/

uint8_t* createSerialFrame(void *inputData, uint16_t noOfBytes, frameParams_t *serialFrameStruct);
void sendFrame(uint8_t *pFrame);

#endif /* SERIALFRAME_H */

