/**
 *   @file    custom_ppg.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   Custom PPG measurement using PPG EduKit
 *   @details For the moment defines the IRQ for ADC read.
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
#include "custom_ppg.h"
#include "TLC5925.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

/* @brief TIMER_1 period value. Timer_1 is used for SpO2 measurement */
#define TIMER_1_PERIOD_NSEC               1000UL
/* @brief TIMER_AdcRead period value. TIMER_AdcRead is used for ADC reading */
#define TIMER_AdcRead_PERIOD_NSEC         8000UL

/* @brief AMP channel index */
#define ADC_CHANNEL_0_INV_AMP               0

/*==================================================================================================
 *                                      LOCAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      LOCAL VARIABLES
 *  ==============================================================================================*/

/* @brief Toggle LEDs using the TLC5925 shift register */
volatile static uint8_t gLedToggle = 0;
/* @brief Array of function pointers for LED enable */
static void (*pEnableFunctions[3])() = {TLC5925_enableRed, TLC5925_enableIR, TLC5925_disableLEDs};

/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      GLOBAL VARIABLES
 *  ==============================================================================================*/

/* @brief Custom PPG buffers */
volatile int32_t CUSTOM_PPG_bufferGreen[CUSTOM_PPG_BUFFER_LENGTH];
volatile int32_t CUSTOM_PPG_bufferRed[CUSTOM_PPG_BUFFER_LENGTH];
volatile int32_t CUSTOM_PPG_bufferIR[CUSTOM_PPG_BUFFER_LENGTH];

/* @brief Points to the head of the buffer (last sample) */
volatile uint16_t CUSTOM_PPG_bufferHead;

/* @brief The application should set the buffer to TRUE when the buffer is used for HR/SpO2. */
volatile bool bBufferProcessed = TRUE;

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
 * Function: CUSTOM_PPG_InterruptHandler_AdcRead    
 *
 * Description: ISR routine for Timer_AdcRead ovf.
 **************************************************************************/
void CUSTOM_PPG_InterruptHandler_AdcRead(void)
{
    /* Clear the terminal count interrupt */
    
    Cy_TCPWM_ClearInterrupt(Timer_AdcRead_HW, Timer_AdcRead_CNT_NUM, CY_TCPWM_INT_ON_TC);
    
    
    if((CUSTOM_PPG_BUFFER_LENGTH != CUSTOM_PPG_bufferHead) && (TRUE == bBufferProcessed))
    {   
        
        (*pEnableFunctions[1])();
        ADC_StartConvert();
        if(ADC_IsEndConversion(CY_SAR_RETURN_STATUS) != 0)
        {
            CUSTOM_PPG_bufferIR[CUSTOM_PPG_bufferHead] =  ADC_GetResult16(ADC_CHANNEL_0_INV_AMP);
       
            
        }
        
        (*pEnableFunctions[0])();
    
    
        ADC_StartConvert();
        if(ADC_IsEndConversion(CY_SAR_RETURN_STATUS) != 0)
        {
            CUSTOM_PPG_bufferRed[CUSTOM_PPG_bufferHead] =  ADC_GetResult16(ADC_CHANNEL_0_INV_AMP);
            CUSTOM_PPG_bufferHead++;
        }

    }
    else
    {
        bBufferProcessed = FALSE;
    }
        
}

/***************************************************************************
 * Function: CUSTOM_PPG_AssignISR_AdcRead    
 *
 * Description: Assign for Timer_AdcRead ovf an ISR
 **************************************************************************/
void CUSTOM_PPG_AssignISR_AdcRead(void)
{
    Cy_SysInt_Init(&adcRead_isr_cfg, CUSTOM_PPG_InterruptHandler_AdcRead);
    NVIC_ClearPendingIRQ(adcRead_isr_cfg.intrSrc);/* Clears the interrupt */
    NVIC_EnableIRQ(adcRead_isr_cfg.intrSrc); /* Enable the core interrupt */
}

/***************************************************************************
 * Function: CUSTOM_PPG_InitAndStartTimer_AdcRead    
 *
 * Description: Init and start Timer_AdcRead
 **************************************************************************/
void CUSTOM_PPG_InitAndStartTimer_AdcRead(void)
{
     /* Start the TCPWM component in timer/counter mode. The return value of the
     * function indicates whether the arguments are valid or not. It is not used
     * here for simplicity. */
    (void)Cy_TCPWM_Counter_Init(Timer_AdcRead_HW, Timer_AdcRead_CNT_NUM, &Timer_AdcRead_config);
    Cy_TCPWM_Enable_Multiple(Timer_AdcRead_HW, Timer_AdcRead_CNT_MASK); /* Enable the counter instance */
    
    /* Set the timer period in milliseconds. To count N cycles, period should be
     * set to N-1. */
    Cy_TCPWM_Counter_SetPeriod(Timer_AdcRead_HW, Timer_AdcRead_CNT_NUM, TIMER_AdcRead_PERIOD_NSEC - 1);
    
    /* Trigger a software reload on the counter instance. This is required when 
     * no other hardware input signal is connected to the component to act as
     * a trigger source. */
    Cy_TCPWM_TriggerReloadOrIndex(Timer_AdcRead_HW, Timer_AdcRead_CNT_MASK);     
}

/***************************************************************************
 * Function: ADC_ISR_Callback    
 *
 * Description: ADC ISR callback. If ADC ISR is enabled, the function will
 *              be called every time the ADC conversion is finished.
 *              Timer_AdcRead should be disabled if the ADC ISR is used.
 **************************************************************************/
void ADC_ISR_Callback(void)
{
    /* NOT USED */
}

/* [] END OF FILE */
