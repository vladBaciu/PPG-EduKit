/**
 *   @file    heartRate.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   MAX30105 library for HR computation
 *   @details MAX30105 library for HR computation ported to C and PSOC6 project.
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

#ifndef HEARTRATE_H_
#define HEARTRATE_H_

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

/*==================================================================================================
 *                                             ENUMS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                GLOBAL VARIABLE DECLARATIONS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 *  ==============================================================================================*/
    
bool checkForBeat(int32_t sample);
int16_t averageDCEstimator(int32_t *p, uint16_t x);
int16_t lowPassFIRFilter(int16_t din);
int32_t mul16(int16_t x, int16_t y);

#endif /* HEARTRATE_H_ */
/* [] END OF FILE */
