/**
 *   @file    TLC5925.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   TLC5925 LED driver library
 *   @details Set active LED, set LED current.
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
#include "TLC5925.h"
#include "AD5273BRJZ1.h"


/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

/* @brief GREEN LED connection to LED driver */
#define GREEN_LED           (0b00000010)
/* @brief RED LED connection to LED driver */
#define RED_LED             (0b00000001)
/* @brief IR LED connection to LED driver */
#define IR_LED              (0b00001100)

/* @brief Shift modes */
#define LSBFIRST            0x00
#define MSBFIRST            0x01

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
 * Function: TLC5925_shiftOut    
 *
 * Description: Toggles CLK pin and SDI pin to shift the data
 **************************************************************************/
void TLC5925_shiftOut(uint8_t bitOrder, uint8_t val)
{
      uint8_t i;

      for (i = 0; i < 8; i++)  {
            if (bitOrder == LSBFIRST)
                  Cy_GPIO_Write(SDI_PORT, SDI_NUM, !!(val & (1 << i)));
            else      
                  Cy_GPIO_Write(SDI_PORT, SDI_NUM, !!(val & (1 << (7 - i))));
                  
          
            Cy_GPIO_Set(CLK_PORT, CLK_NUM); 
            Cy_GPIO_Clr(CLK_PORT, CLK_NUM);   
      }
}

/***************************************************************************
 * Function: TLC5925_disableLEDs    
 *
 * Description: Disables all LEDs
 **************************************************************************/
void TLC5925_disableLEDs(void)
{
       /* Pins startup conditions */
       /* LE must be low while data is being shifted in */
       Cy_GPIO_Clr(LE_PORT, LE_NUM); 
       /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */
       Cy_GPIO_Clr(CLK_PORT, CLK_NUM); 
       /* OE high keep leds turned off while data is shifted in */
       Cy_GPIO_Set(OE_PORT, OE_NUM); 

      /* Shift out the data to turn green led on: 0000 0000 0000 0010. 
         shiftOut() shifts 1 byte at a time, so the function is called twice. 
        * First byte corresponding to outputs 15 to 8. ALL UNUSED
        * First byte corresponding to outputs 7 to 0. 7 to 5 unused. */
       TLC5925_shiftOut(MSBFIRST, 0x00); 
       TLC5925_shiftOut(MSBFIRST, 0x00); 


       /* LE toggled to latch in the shifted data */
       Cy_GPIO_Set(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(OE_PORT, OE_NUM);
}

/***************************************************************************
 * Function: TLC5925_enableGreen    
 *
 * Description: Enables green LED
 **************************************************************************/
void TLC5925_enableGreen(void)
{
       /* Pins startup conditions */
       /* LE must be low while data is being shifted in */
       Cy_GPIO_Clr(LE_PORT, LE_NUM); 
       /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */
       Cy_GPIO_Clr(CLK_PORT, CLK_NUM); 
       /* OE high keep leds turned off while data is shifted in */
       Cy_GPIO_Set(OE_PORT, OE_NUM); 

      /* Shift out the data to turn green led on: 0000 0000 0000 0010. 
         shiftOut() shifts 1 byte at a time, so the function is called twice. 
        * First byte corresponding to outputs 15 to 8. ALL UNUSED
        * First byte corresponding to outputs 7 to 0. 7 to 5 unused. */
       TLC5925_shiftOut(MSBFIRST, 0x00); 
       TLC5925_shiftOut(MSBFIRST, GREEN_LED); 


       /* LE toggled to latch in the shifted data */
       Cy_GPIO_Set(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(OE_PORT, OE_NUM);
}

/***************************************************************************
 * Function: TLC5925_enableRed    
 *
 * Description: Enables red LED
 **************************************************************************/
void TLC5925_enableRed(void)
{
       /* Pins startup conditions */
       /* LE must be low while data is being shifted in */
       Cy_GPIO_Clr(LE_PORT, LE_NUM); 
       /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */
       Cy_GPIO_Clr(CLK_PORT, CLK_NUM); 
       /* OE high keep leds turned off while data is shifted in */
       Cy_GPIO_Set(OE_PORT, OE_NUM); 

      /* Shift out the data to turn green led on: 0000 0000 0000 0010. 
         shiftOut() shifts 1 byte at a time, so the function is called twice. 
        * First byte corresponding to outputs 15 to 8. ALL UNUSED
        * First byte corresponding to outputs 7 to 0. 7 to 5 unused. */
       TLC5925_shiftOut(MSBFIRST, 0x00); 
       TLC5925_shiftOut(MSBFIRST, RED_LED); 


       /* LE toggled to latch in the shifted data */
       Cy_GPIO_Set(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(OE_PORT, OE_NUM);        
}

/***************************************************************************
 * Function: TLC5925_enableIR    
 *
 * Description: Enables IR LED
 **************************************************************************/
void TLC5925_enableIR(void)
{
       /* Pins startup conditions */
       /* LE must be low while data is being shifted in */
       Cy_GPIO_Clr(LE_PORT, LE_NUM); 
       /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */
       Cy_GPIO_Clr(CLK_PORT, CLK_NUM); 
       /* OE high keep leds turned off while data is shifted in */
       Cy_GPIO_Set(OE_PORT, OE_NUM); 

      /* Shift out the data to turn green led on: 0000 0000 0000 0010. 
         shiftOut() shifts 1 byte at a time, so the function is called twice. 
        * First byte corresponding to outputs 15 to 8. ALL UNUSED
        * First byte corresponding to outputs 7 to 0. 7 to 5 unused. */
       TLC5925_shiftOut(MSBFIRST, 0x00); 
       TLC5925_shiftOut(MSBFIRST, IR_LED); 


       /* LE toggled to latch in the shifted data */
       Cy_GPIO_Set(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(LE_PORT, LE_NUM);
       Cy_GPIO_Clr(OE_PORT, OE_NUM);
}

/***************************************************************************
 * Function: TLC5925_SetCurrent_mA    
 *
 * Description: Sets the current value
 **************************************************************************/
void TLC5925_SetCurrent_mA(uint16_t val)
{ 
    uint16_t resVal = 0U;
    /* Current to resistance conversion. See 9.2.2.1 chapter in tlc5925.pdf datasheet */
    resVal = (uint16_t)(1000 * ((float)(1.21 * 18)/val));
    AD5273_SetWiper(resVal);
}
/* [] END OF FILE */
