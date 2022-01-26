/**
 *   @file    milliseconds.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   MILLS library
 *   @details Library used for time measurement
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

/* @brief Include PSOC generated files */
#include "project.h"

/* @brief Include custom libraries for PPG EduKit */
#include "milliseconds.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

/* @brief Timer period in nanoseconds */
#define TIMER_PERIOD_NSEC               1000UL

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

/* @brief Global variable that keeps track of the time */
volatile uint32 gMilliseconds;

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
 * Function: MILLIS_InterruptHandler    
 *
 * Description: Timer interrupt handler. Increment gMilliseconds variable
 **************************************************************************/
void MILLIS_InterruptHandler(void)
{
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(Timer_HW, Timer_CNT_NUM, CY_TCPWM_INT_ON_TC);
    
    gMilliseconds++;
    
}

/***************************************************************************
 * Function: MILLIS_AssignISR    
 *
 * Description: Assign ISR to Timer ovf
 **************************************************************************/
void MILLIS_AssignISR(void)
{
    Cy_SysInt_Init(&isr_1ms_cfg, MILLIS_InterruptHandler);
    NVIC_ClearPendingIRQ(isr_1ms_cfg.intrSrc);/* Clears the interrupt */
    NVIC_EnableIRQ(isr_1ms_cfg.intrSrc); /* Enable the core interrupt */
}

/***************************************************************************
 * Function: MILLIS_InitAndStartTimer    
 *
 * Description: Init and start Timer
 **************************************************************************/
void MILLIS_InitAndStartTimer(void)
{
     /* Start the TCPWM component in timer/counter mode. The return value of the
     * function indicates whether the arguments are valid or not. It is not used
     * here for simplicity. */
    (void)Cy_TCPWM_Counter_Init(Timer_HW, Timer_CNT_NUM, &Timer_config);
    Cy_TCPWM_Enable_Multiple(Timer_HW, Timer_CNT_MASK); /* Enable the counter instance */
    
    /* Set the timer period in milliseconds. To count N cycles, period should be
     * set to N-1. */
    Cy_TCPWM_Counter_SetPeriod(Timer_HW, Timer_CNT_NUM, TIMER_PERIOD_NSEC - 1);
    
    /* Trigger a software reload on the counter instance. This is required when 
     * no other hardware input signal is connected to the component to act as
     * a trigger source. */
    Cy_TCPWM_TriggerReloadOrIndex(Timer_HW, Timer_CNT_MASK);     
}

/***************************************************************************
 * Function: MILLIS_DisableTimer    
 *
 * Description: Disable Timer
 **************************************************************************/
void MILLIS_DisableTimer(void)
{
    Cy_TCPWM_Disable_Multiple(Timer_HW, Timer_CNT_MASK);
}

/***************************************************************************
 * Function: MILLIS_GetValue    
 *
 * Description: Read time value
 **************************************************************************/
uint32_t MILLIS_GetValue(void)
{
    return gMilliseconds;
}
/* [] END OF FILE */
