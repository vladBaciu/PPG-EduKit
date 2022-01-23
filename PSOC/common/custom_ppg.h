/**
 *   @file    custom_ppg.h
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   Export PPG EduKit functions
 *   @details Export all the custom APIs and macros for PPG measurement using the custom PPG sensor.
 *
 */

/*==================================================================================================
 *
 * Copyright Vrije Universiteit Brussel, 2021
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 *  ==============================================================================================*/
#ifndef CUSTOM_PPG_H_
#define CUSTOM_PPG_H_
    
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

/* @brief PPG sampling rate defined by Timer_AdcRead period */
#define CUSTOM_PPG_FREQS                  (125UL)
/* @brief PPG buffer length used to compute HR or SpO2 */
#define CUSTOM_PPG_BUFFER_LENGTH              (512U)
#define CUSTOM_NUMBER_OF_OVERLAPPING_SAMPLES  (480U)  
/* @brief Min number of samples between two peaks */
#define CUSTOM_SAMPLES_BETWEEN_PEAKS  (60UL)

/*==================================================================================================
 *                                             ENUMS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                GLOBAL VARIABLE DECLARATIONS
 *  ==============================================================================================*/

extern volatile int32_t CUSTOM_PPG_bufferGreen[CUSTOM_PPG_BUFFER_LENGTH];
extern volatile int32_t CUSTOM_PPG_bufferRed[CUSTOM_PPG_BUFFER_LENGTH];
extern volatile int32_t CUSTOM_PPG_bufferIR[CUSTOM_PPG_BUFFER_LENGTH];
extern volatile uint16_t CUSTOM_PPG_bufferHead;
extern volatile bool bBufferProcessed;

/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 *  ==============================================================================================*/   

void CUSTOM_PPG_AssignISR_Spo2(void);
void CUSTOM_PPG_InterruptHandler_Spo2(void);
void CUSTOM_PPG_InitAndStartTimer_Spo2(void);
void CUSTOM_PPG_AssignISR_FillBuff(void);
void CUSTOM_PPG_InitAndStartTimer_FillBuff(void);
void CUSTOM_PPG_InterruptHandler_FillBuff(void);
void printadcval(void);
void CUSTOM_PPG_InitAndStartTimer_AdcRead(void);
void CUSTOM_PPG_AssignISR_AdcRead(void);   

#endif /* CUSTOM_PPG_H_ */    
/* [] END OF FILE */
