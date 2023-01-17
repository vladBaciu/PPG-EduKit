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
#include "heartRate.h"

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


/*==================================================================================================
 *                                      LOCAL CONSTANTS
 *  ==============================================================================================*/
static const uint16_t FIRCoeffs[12] = {172, 321, 579, 927, 1360, 1858, 2390, 2916, 3391, 3768, 4012, 4096};

/*==================================================================================================
 *                                      LOCAL VARIABLES
 *  ==============================================================================================*/

/* @brief Toggle LEDs using the TLC5925 shift register */
volatile static bool bToggleFunctions = FALSE;
/* @brief Array of function pointers for LED enable */
static void (*pEnableFunctions[2])() = {TLC5925_enableRed, TLC5925_enableIR};

static int16_t IR_AC_Max = 20;
static int16_t IR_AC_Min = -20;

static int16_t IR_AC_Signal_Current = 0;
static int16_t IR_AC_Signal_Previous;
static int16_t IR_AC_Signal_min = 0;
static int16_t IR_AC_Signal_max = 0;
static int16_t IR_Average_Estimated;

static int16_t positiveEdge = 0;
static int16_t negativeEdge = 0;
static int32_t ir_avg_reg = 0;

static int16_t cbuf[32];
static uint8_t offset = 0;
/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      GLOBAL VARIABLES
 *  ==============================================================================================*/

/* @brief Custom PPG buffers */
volatile uint16_t CUSTOM_PPG_bufferGreen[CUSTOM_PPG_BUFFER_LENGTH];
volatile uint16_t CUSTOM_PPG_bufferRed[CUSTOM_PPG_BUFFER_LENGTH];
volatile uint16_t CUSTOM_PPG_bufferIR[CUSTOM_PPG_BUFFER_LENGTH];

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

static int16_t CUSTOM_lowPassFIRFilter(int16_t din);

/*==================================================================================================
 *                                       GLOBAL FUNCTIONS
 *  ==============================================================================================*/

/***************************************************************************
 * Function: CUSTOM_PPG_InterruptHandler_Spo2    
 *
 * Description: ISR routine for Timer_1 ovf.
 **************************************************************************/
void CUSTOM_PPG_InterruptHandler_Spo2(void)
{
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(Timer_1_HW, Timer_1_CNT_NUM, CY_TCPWM_INT_ON_TC);
    (*pEnableFunctions[bToggleFunctions])();
    if(CUSTOM_PPG_BUFFER_LENGTH != CUSTOM_PPG_bufferHead)
    {
        if(FALSE == bToggleFunctions)
        {
           CUSTOM_PPG_bufferRed[CUSTOM_PPG_bufferHead] =  ADC_GetResult16(ADC_CHANNEL_0_INV_AMP); 
        }
        else
        {
           CUSTOM_PPG_bufferIR[CUSTOM_PPG_bufferHead] =  ADC_GetResult16(ADC_CHANNEL_0_INV_AMP);
        CUSTOM_PPG_bufferHead++;
        }
        
    }
    else
    {
       CUSTOM_PPG_bufferHead = 0UL;
    }
    bToggleFunctions = !bToggleFunctions;
}

/***************************************************************************
 * Function: CUSTOM_PPG_AssignISR_Spo2    
 *
 * Description: Assign for Timer_1 ovf an ISR
 **************************************************************************/
void CUSTOM_PPG_AssignISR_Spo2(void)
{
    Cy_SysInt_Init(&spo2_isr_cfg, CUSTOM_PPG_InterruptHandler_Spo2);
    NVIC_ClearPendingIRQ(spo2_isr_cfg.intrSrc);/* Clears the interrupt */
    NVIC_EnableIRQ(spo2_isr_cfg.intrSrc); /* Enable the core interrupt */
}

/***************************************************************************
 * Function: CUSTOM_PPG_InitAndStartTimer_Spo2    
 *
 * Description: Init and start Timer_1
 **************************************************************************/
void CUSTOM_PPG_InitAndStartTimer_Spo2(void)
{
     /* Start the TCPWM component in timer/counter mode. The return value of the
     * function indicates whether the arguments are valid or not. It is not used
     * here for simplicity. */
    (void)Cy_TCPWM_Counter_Init(Timer_1_HW, Timer_1_CNT_NUM, &Timer_1_config);
    Cy_TCPWM_Enable_Multiple(Timer_1_HW, Timer_1_CNT_MASK); /* Enable the counter instance */
    
    /* Set the timer period in milliseconds. To count N cycles, period should be
     * set to N-1. */
    Cy_TCPWM_Counter_SetPeriod(Timer_1_HW, Timer_1_CNT_NUM, TIMER_1_PERIOD_NSEC - 1);
    
    /* Trigger a software reload on the counter instance. This is required when 
     * no other hardware input signal is connected to the component to act as
     * a trigger source. */
    Cy_TCPWM_TriggerReloadOrIndex(Timer_1_HW, Timer_1_CNT_MASK);     
}

/***************************************************************************
 * Function: CUSTOM_PPG_InterruptHandler_AdcRead    
 *
 * Description: ISR routine for Timer_AdcRead ovf.
 **************************************************************************/
void CUSTOM_PPG_InterruptHandler_AdcRead(void)
{
    /* Clear the terminal count interrupt */
    
    Cy_TCPWM_ClearInterrupt(Timer_AdcRead_HW, Timer_AdcRead_CNT_NUM, CY_TCPWM_INT_ON_TC);
    ADC_StartConvert();
    
    if((CUSTOM_PPG_BUFFER_LENGTH != CUSTOM_PPG_bufferHead) && (TRUE == bBufferProcessed))
    {
        if(ADC_IsEndConversion(CY_SAR_RETURN_STATUS) != 0)
        {
            CUSTOM_PPG_bufferIR[CUSTOM_PPG_bufferHead] =  ADC_GetResult16(ADC_CHANNEL_0_INV_AMP);
       
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
 * Function: checkForBeat    
 *
 * Description: Heart Rate Monitor functions takes a sample value and the 
 *              sample number. Returns true if a beat is detected,
 *              A running average of four samples is recommended for display 
 *              on the screen.
 **************************************************************************/
bool CUSTOM_checkForBeat(uint32_t sample)
{
  bool beatDetected = false;

  //  Save current state
  IR_AC_Signal_Previous = IR_AC_Signal_Current;
  
  //This is good to view for debugging
  //Serial.print("Signal_Current: ");
  //Serial.println(IR_AC_Signal_Current);

  //  Process next data sample
  IR_Average_Estimated = averageDCEstimator(&ir_avg_reg, sample);
  IR_AC_Signal_Current = sample - IR_Average_Estimated;
 
  //  Detect positive zero crossing (rising edge)
  if ((IR_AC_Signal_Previous < 0) & (IR_AC_Signal_Current >= 0))
  {
  
    IR_AC_Max = IR_AC_Signal_max; //Adjust our AC max and min
    IR_AC_Min = IR_AC_Signal_min;

    positiveEdge = 1;
    negativeEdge = 0;
    IR_AC_Signal_max = 0;

    if (((IR_AC_Max - IR_AC_Min) > 10) & ((IR_AC_Max - IR_AC_Min) < 120))
    {
      //Heart beat!!!
      beatDetected = true;
    }
  }

  //  Detect negative zero crossing (falling edge)
  if ((IR_AC_Signal_Previous > 0) & (IR_AC_Signal_Current <= 0))
  {
    positiveEdge = 0;
    negativeEdge = 1;
    IR_AC_Signal_min = 0;
  }

  //  Find Maximum value in positive cycle
  if (positiveEdge & (IR_AC_Signal_Current > IR_AC_Signal_Previous))
  {
    IR_AC_Signal_max = IR_AC_Signal_Current;
  }

  //  Find Minimum value in negative cycle
  if (negativeEdge & (IR_AC_Signal_Current < IR_AC_Signal_Previous))
  {
    IR_AC_Signal_min = IR_AC_Signal_Current;
  }
  
  return(beatDetected);
}

/***************************************************************************
 * Function: lowPassFIRFilter    
 *
 * Description: Low Pass FIR Filter
 **************************************************************************/
int16_t CUSTOM_lowPassFIRFilter(int16_t din)
{  
  cbuf[offset] = din;

  int32_t z = mul16(FIRCoeffs[11], cbuf[(offset - 11) & 0x1F]);
  
  for (uint8_t i = 0 ; i < 11 ; i++)
  {
    z += mul16(FIRCoeffs[i], cbuf[(offset - i) & 0x1F] + cbuf[(offset - 22 + i) & 0x1F]);
  }

  offset++;
  offset %= 32; //Wrap condition

  return(z >> 15);
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
