/**
 *   @file    MAX30102.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   MAX30105 library
 *   @details MAX30105 library ported to C and PSOC6 project.
 *            
 */

/*==================================================================================================
 *
 *   Copyright (c) 2016 SparkFun Electronics
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 *   PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 *   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
 *   IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *  ==============================================================================================*/

#ifndef MAX30102_H
#define MAX30102_H


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

/* @brief No of samples per second */
#define MAX30105_FREQS                  (25UL)
/* @brief Min number of samples between two peaks */
#define MAX30105_SAMPLES_BETWEEN_PEAKS  (4UL)
    
/* @brief Channel indexes */
#define RED_CHANNEL   (0U)
#define GREEN_CHANNEL (1U)
#define IR_CHANNEL    (2U)

/* @brief Buffer macros configuration */
#define READ_FROM_FIFO_BUFFER                 TRUE
#define FIFO_NUMBER_OF_SAMPLES               (50U)
#define FIFO_NUMBER_OF_OVERLAPPING_SAMPLES   (25U)
    
/*==================================================================================================
 *                                             ENUMS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                GLOBAL VARIABLE DECLARATIONS
 *  ==============================================================================================*/

/* @brief Points to the head of the buffer */
extern uint32_t samplesTaken;
/* @brief Channels buffer */
extern uint16_t FIFO_Buffer[3 * FIFO_NUMBER_OF_SAMPLES];

/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 *  ==============================================================================================*/    
uint32_t MAX30105_ReadRegister8(uint8_t *rxBuffer, uint8_t reg);
uint32_t MAX30105_WriteRegister8(uint8_t txBuffer, uint8_t reg);
uint32_t MAX30105_WriteWithBitMask(uint8_t reg, uint8_t mask, uint8_t val);
uint32_t MAX30105_GetRed(void);
uint32_t MAX30105_GetIR(void);
uint32_t MAX30105_GetGreen(void);
uint32_t MAX30105_GetFIFORed(void);
uint32_t MAX30105_GetFIFOIR(void);
uint32_t MAX30105_GetFIFOGreen(void);
uint32_t MAX30105_readChannel(uint8_t channel);
uint16_t MAX30105_Check(void);
uint8_t MAX30105_Available(void);
uint8_t MAX30105_GetWritePointer(void);
uint8_t MAX30105_GetReadPointer(void);
void MAX30105_NextSample(void);
void MAX30105_SetFIFOAverage(uint8_t numberOfSamples);
void MAX30105_EnableFIFORollover(void);
void MAX30105_SetLEDMode(uint8_t mode);
void MAX30105_SetADCRange(uint8_t adcRange);
void MAX30105_SetSampleRate(uint8_t sampleRate);
void MAX30105_SetPulseWidth(uint8_t pulseWidth);
void MAX30105_SetPulseAmplitudeRed(uint8_t amplitude);
void MAX30105_SetPulseAmplitudeIR(uint8_t amplitude);
void MAX30105_SetPulseAmplitudeGreen(uint8_t amplitude);
void MAX30105_SetPulseAmplitudeProximity(uint8_t amplitude);
void MAX30105_EnableSlot(uint8_t slotNumber, uint8_t device);
void MAX30105_ClearFIFO(void);
void MAX30105_Setup(uint8_t  powerLevel, uint8_t sampleAverage, uint8_t ledMode, uint32_t sampleRate, uint32_t pulseWidth, uint32_t adcRange);
void MAX30105_SoftReset(void);
bool MAX30105_ReadChannels(uint16_t *channelsBuffer, bool readFIFO);
bool MAX30105_SafeCheck(uint8_t maxTimeToCheck);

#endif /* MAX30102_H */
/* [] END OF FILE */
