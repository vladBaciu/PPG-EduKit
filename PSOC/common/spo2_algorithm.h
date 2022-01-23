/**
 *   @file    spo2_algorithm.h
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   MAX30105 library for HR and SpO2 computation
 *   @details MAX30105 library for HR and SpO2 computation ported to C and PSOC6 project.
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

#ifndef SPO2_ALGORITHM_H_
#define SPO2_ALGORITHM_H_

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
    
#define MA4_SIZE 4 // DONOT CHANGE
    
/*==================================================================================================
 *                                             ENUMS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 *  ==============================================================================================*/



/*==================================================================================================
 *                                GLOBAL VARIABLE DECLARATIONS
 *  ==============================================================================================*/

/* @brief uch_spo2_table is approximated as:  
    -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 */
extern const uint8_t uch_spo2_table[184];

/*==================================================================================================
 *                                    FUNCTION PROTOTYPES
 *  ==============================================================================================*/
void maxim_heart_rate_and_oxygen_saturation2(int32_t *pun_ir_buffer, int32_t n_ir_buffer_length, 
                                            int32_t *pun_red_buffer, int32_t *pn_spo2, 
                                            int8_t *pch_spo2_valid, int32_t *pn_heart_rate, 
                                            int8_t *pch_hr_valid, uint32_t freq, 
                                            int32_t peak_distance);
void maxim_heart_rate_and_oxygen_saturation(uint16_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint16_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, 
                int32_t *pn_heart_rate, int8_t *pch_hr_valid, uint32_t freq, int32_t peak_distance);

void maxim_find_peaks(int32_t *pn_locs, int32_t *n_npks,  int32_t  *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num);
void maxim_peaks_above_min_height(int32_t *pn_locs, int32_t *n_npks,  int32_t  *pn_x, int32_t n_size, int32_t n_min_height);
void maxim_remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance);
void maxim_sort_ascend(int32_t  *pn_x, int32_t n_size);
void maxim_sort_indices_descend(int32_t  *pn_x, int32_t *pn_indx, int32_t n_size);

#endif /* SPO2_ALGORITHM_H_ */
/* [] END OF FILE */
