/**
 *   @file    oled_driver.h
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   SH1107 Oled driver library
 *   @details Export SH1107 Oled driver library APIs and macros
 *
 */

/*
===== derived license (for graphics library) ======

Copyright (c) 2013 Adafruit Industries. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _OLED_DRIVER_H
#define _OLED_DRIVER_H

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

/* @brief Maximum ASCII number of digits in a number */
#define USINT2DECASCII_MAX_DIGITS   (5)

/* @brief I2C display address. 011110+SA0+RW - 0x3C or 0x3D   */
#define DISPLAY_ADDRESS 0x3C 

/* @brief Text font colors */
#define BLACK 0
#define WHITE 1
#define INVERSE 2   

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
    
void display_init(void);
void display_update(void);
void display_clear(void);
void display_contrast( uint8_t contrast );
void display_invert( uint8_t invert );
uint8_t display_usint2decascii(uint16_t num, char* buffer);
void gfx_drawPixel(int16_t x, int16_t y, uint16_t color);
void gfx_drawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color );
void gfx_setCursor( int16_t x, int16_t y );
void gfx_setTextSize( uint8_t size );
void gfx_setTextColor( uint16_t color );
void gfx_setTextBg( uint16_t background );
void gfx_write( uint8_t ch );
int16_t gfx_width(void);
int16_t gfx_height(void);
void gfx_print( const char* s );
void gfx_println( const char* s );
void gfx_drawRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color );
void gfx_fillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color );
void gfx_drawCircle( int16_t x0, int16_t y0, int16_t r,uint16_t color );
void gfx_drawTriangle( int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color );
void gfx_setRotation( uint8_t x );
void gfx_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color); 
#endif	/* _OLED_DRIVER_H */
/* [] END OF FILE */