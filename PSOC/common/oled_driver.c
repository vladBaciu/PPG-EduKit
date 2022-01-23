/**
 *   @file    oled_driver.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   SH1107 Oled driver library
 *   @details Includes I2C communication and Adafruit gfx library
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

/*==================================================================================================
 *                                        INCLUDE FILES
 * 1) system and project includes
 * 2) needed interfaces from external units
 * 3) internal and external interfaces from this unit
 *  ==============================================================================================*/

/* @brief Include PSOC generated files */
#include "project.h"

/* @brief Include custom libraries for PPG EduKit */
#include "oled_driver.h"
#include "font.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

#define I2C_TIMEOUT                 (100UL)
#define I2C_MAXIMUM_BUFF_SIZE       (32U)

#define DISPLAYWIDTH 64
#define DISPLAYHEIGHT 128
    
#define SH110X_MEMORYMODE 0x20          ///< See datasheet
#define SH110X_COLUMNADDR 0x21          ///< See datasheet
#define SH110X_PAGEADDR 0x22            ///< See datasheet
#define SH110X_SETCONTRAST 0x81         ///< See datasheet
#define SH110X_CHARGEPUMP 0x8D          ///< See datasheet
#define SH110X_SEGREMAP 0xA0            ///< See datasheet
#define SH110X_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SH110X_DISPLAYALLON 0xA5        ///< Not currently used
#define SH110X_NORMALDISPLAY 0xA6       ///< See datasheet
#define SH110X_INVERTDISPLAY 0xA7       ///< See datasheet
#define SH110X_SETMULTIPLEX 0xA8        ///< See datasheet
#define SH110X_DCDC 0xAD                ///< See datasheet
#define SH110X_DISPLAYOFF 0xAE          ///< See datasheet
#define SH110X_DISPLAYON 0xAF           ///< See datasheet
#define SH110X_SETPAGEADDR                                                     \
0xB0 ///< Specify page address to load display RAM data to page address
       ///< register
#define SH110X_COMSCANINC 0xC0         ///< Not currently used
#define SH110X_COMSCANDEC 0xC8         ///< See datasheet
#define SH110X_SETDISPLAYOFFSET 0xD3   ///< See datasheet
#define SH110X_SETDISPLAYCLOCKDIV 0xD5 ///< See datasheet
#define SH110X_SETPRECHARGE 0xD9       ///< See datasheet
#define SH110X_SETCOMPINS 0xDA         ///< See datasheet
#define SH110X_SETVCOMDETECT 0xDB      ///< See datasheet
#define SH110X_SETDISPSTARTLINE                                                \
  0xDC ///< Specify Column address to determine the initial display line or
       ///< COM0.

#define SH110X_SETLOWCOLUMN 0x00  ///< Not currently used
#define SH110X_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SH110X_SETSTARTLINE 0x40  ///< See datasheet


// see data sheet page 25 for Graphic Display Data RAM organization
// 8 pages, each page a row of DISPLAYWIDTH bytes
// start address of of row: y/8*DISPLAYWIDTH
// x pos in row: == x 
#define GDDRAM_ADDRESS(X,Y) ((_displaybuf)+((Y)/8)*(DISPLAYWIDTH)+(X))

// lower 3 bit of y determine vertical pixel position (pos 0...7) in GDDRAM byte
// (y&0x07) == position of pixel on row (page). LSB is top, MSB bottom
#define GDDRAM_PIXMASK(Y) (1 << ((Y)&0x07))

#define PIXEL_ON(X,Y) (*GDDRAM_ADDRESS(x,y) |= GDDRAM_PIXMASK(y))
#define PIXEL_OFF(X,Y) (*GDDRAM_ADDRESS(x,y) &= ~GDDRAM_PIXMASK(y))
#define PIXEL_TOGGLE(X,Y) (*GDDRAM_ADDRESS(x,y) ^= GDDRAM_PIXMASK(y)) 

#define FLASH_READ_BYTE(addr) (*(const unsigned char *)(addr))
/*==================================================================================================
 *                                      LOCAL CONSTANTS
 *  ==============================================================================================*/
/* @brief Logo image that will be stored in user application flash memory */
const unsigned char PPG_EduKit_Logo []  = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x3f, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x3f, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x37, 0x00, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x30, 0x00, 0x33, 0x80, 0x03, 0x78, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x38, 0x00, 0x73, 0xc0, 0x07, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3c, 0x00, 0x71, 0xe0, 0x07, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3e, 0x00, 0x71, 0xf8, 0x07, 0x1c, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x9f, 0x20, 0x60, 0xfe, 0x06, 0x1f, 0xc0, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0xf0, 0x60, 0x3f, 0x06, 0x0f, 0xf0, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0xf8, 0x60, 0x0f, 0x86, 0x01, 0xf8, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0x40, 0x00, 0x00, 0x07, 0x00, 0x78, 0x60, 0x03, 0xc6, 0x00, 0x7c, 0x00, 0x00, 0x00, 
	0x03, 0x80, 0xc0, 0x00, 0x00, 0xc7, 0x00, 0x3c, 0x60, 0x01, 0xc4, 0x00, 0x1e, 0x00, 0x00, 0x00, 
	0x03, 0xc0, 0x60, 0x00, 0x00, 0xe7, 0x00, 0x1c, 0x60, 0x00, 0xe4, 0x00, 0x0f, 0x00, 0x00, 0x00, 
	0x03, 0xc0, 0x60, 0x00, 0x00, 0xe7, 0x00, 0x1f, 0xe0, 0x00, 0xfc, 0x00, 0x0f, 0x80, 0x00, 0x00, 
	0x33, 0x62, 0x20, 0x00, 0x00, 0xff, 0x00, 0x0f, 0xe0, 0x00, 0x7c, 0x00, 0x07, 0xc0, 0x00, 0x00, 
	0x1b, 0x63, 0x30, 0x00, 0x00, 0x7e, 0x00, 0x07, 0xe0, 0x00, 0x7c, 0x00, 0x03, 0xc0, 0x00, 0x00, 
	0x1f, 0xc3, 0x30, 0x00, 0x00, 0x1c, 0x00, 0x03, 0xc0, 0x00, 0x10, 0x00, 0x01, 0x40, 0x00, 0x00, 
	0x0f, 0xc1, 0x30, 0x00, 0x00, 0x1c, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0x81, 0xb0, 0x00, 0x00, 0x61, 0x00, 0x04, 0x10, 0x00, 0xbc, 0x00, 0x0b, 0xe1, 0x00, 0x00, 
	0x03, 0x01, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0x81, 0x30, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
	0x07, 0x81, 0x30, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x00, 
	0x0f, 0xc1, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x1b, 0x63, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x33, 0x63, 0x20, 0x1f, 0xc3, 0xf8, 0x03, 0xc0, 0x3f, 0xc0, 0x18, 0x00, 0x70, 0x38, 0x00, 0x00, 
	0x03, 0xe0, 0x60, 0x1f, 0xe3, 0xfc, 0x0f, 0xe0, 0x3f, 0xc0, 0x1c, 0x00, 0x70, 0x79, 0x00, 0x00, 
	0x03, 0xc0, 0x60, 0x1f, 0xf3, 0xfe, 0x0f, 0xf0, 0x3f, 0xc0, 0x18, 0x00, 0x70, 0x73, 0x9c, 0x00, 
	0x03, 0x80, 0xc0, 0x1c, 0xf3, 0x9e, 0x1e, 0x70, 0x3f, 0xc0, 0x18, 0x00, 0x70, 0xf3, 0x9c, 0x00, 
	0x03, 0x00, 0x00, 0x18, 0x73, 0x87, 0x38, 0x38, 0x3f, 0xc0, 0x18, 0x00, 0x71, 0xe1, 0x1c, 0x00, 
	0x03, 0x00, 0x00, 0x18, 0x33, 0x07, 0x38, 0x10, 0x38, 0x00, 0x18, 0x00, 0x71, 0xc0, 0x1c, 0x00, 
	0x02, 0x00, 0x00, 0x18, 0x33, 0x03, 0x30, 0x00, 0x30, 0x00, 0x38, 0x00, 0x73, 0xc0, 0x1c, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x33, 0x03, 0x20, 0x00, 0x30, 0x03, 0xf9, 0x83, 0x73, 0x83, 0xbe, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x33, 0x83, 0x60, 0x00, 0x30, 0x07, 0xf9, 0x83, 0x7f, 0x83, 0xbf, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x33, 0x87, 0x60, 0x00, 0x3f, 0x8f, 0x19, 0x83, 0x7f, 0x03, 0xbf, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0xf3, 0x86, 0x60, 0xfc, 0x3f, 0x8e, 0x19, 0x83, 0x7f, 0x03, 0xbe, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf3, 0xfe, 0x60, 0xfc, 0x3f, 0x8c, 0x19, 0x83, 0x7f, 0x03, 0x9e, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xe3, 0xfc, 0x60, 0xfc, 0x3f, 0xc8, 0x19, 0x83, 0x7f, 0x03, 0x9c, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xc3, 0xf8, 0x60, 0x0c, 0x3f, 0x98, 0x19, 0x83, 0x7f, 0x03, 0x9c, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x03, 0x80, 0x60, 0x0c, 0x38, 0x18, 0x19, 0x83, 0x73, 0x83, 0x9c, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x03, 0x80, 0x70, 0x0c, 0x30, 0x18, 0x19, 0x82, 0x73, 0xc3, 0x9c, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x03, 0x80, 0x30, 0x0c, 0x30, 0x08, 0x19, 0x86, 0x71, 0xc3, 0x9c, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x03, 0x80, 0x38, 0x1c, 0x38, 0x08, 0x19, 0x86, 0x71, 0xe3, 0x9c, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x03, 0x80, 0x3c, 0x3c, 0x3f, 0xcc, 0x19, 0xc6, 0x70, 0xe3, 0x9f, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x03, 0x80, 0x1e, 0x7c, 0x3f, 0xce, 0x39, 0xee, 0x70, 0xf3, 0x9f, 0x00, 
	0x00, 0x00, 0x00, 0x18, 0x03, 0x80, 0x0f, 0xfc, 0x3f, 0xc7, 0xf9, 0xfe, 0x70, 0x73, 0x8f, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x03, 0x80, 0x07, 0xcc, 0x7f, 0xc7, 0xfc, 0xfc, 0x70, 0x7b, 0x8f, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc3, 0xc8, 0x78, 0x00, 0x28, 0x07, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*==================================================================================================
 *                                      LOCAL VARIABLES
 *  ==============================================================================================*/

/* @brief display memory buffer ( === MUST INCLUDE === the preceding I2C 0x40 control byte for the display) */
static uint8_t SH110X_buffer[DISPLAYHEIGHT * DISPLAYWIDTH / 8 + 1] = { 0x40 };
/* @brief pointer to actual display memory buffer */
static uint8_t* _displaybuf = SH110X_buffer+1;
static uint16_t _displaybuf_size = sizeof(SH110X_buffer) - 1;

/* @brief Display w/h as modified by current rotation */
static int16_t _width, _height;
/* @brief Stores current cursor position */
static int16_t cursor_x, cursor_y;
/* @brief Stores current text options */
static uint16_t textcolor, textbgcolor;
static uint8_t textsize;

/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      GLOBAL VARIABLES
 *  ==============================================================================================*/

/* @brief Dirty tracking window minimum x */
int16_t window_x1; 
/* @brief Dirty tracking window minimum y */
int16_t window_y1;
/* @brief Dirty tracking window maximum x */
int16_t window_x2;    
/* @brief Dirty tracking window maximum y */
int16_t window_y2; 

/* @brief This is the 'raw' display w/h - never changes */
int16_t WIDTH, HEIGHT;
/* @brief Stores the rotation of the display */
uint8_t rotation;
/* @brief If set, 'wrap' text at right edge of display */
uint8_t wrap; 

/*==================================================================================================
 *                                   LOCAL FUNCTION PROTOTYPES
 *  ==============================================================================================*/

static uint32  display_write_buf( uint8* buf, uint16_t size, uint8_t dc_val, bool send_dc);

/*==================================================================================================
 *                                   GLOBAL FUNCTION PROTOTYPES
 *  ==============================================================================================*/

void gfx_init( int16_t width, int16_t height );

/*==================================================================================================
 *                                       LOCAL FUNCTIONS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       GLOBAL FUNCTIONS
 *  ==============================================================================================*/

/***************************************************************************
 * Function: display_init    
 *
 * Description:  Send init command over I2C. Call before first use of other 
 *               functions
 **************************************************************************/
void display_init(void){
    
    
    gfx_init(DISPLAYWIDTH, DISPLAYHEIGHT);
   
    uint8 cmdbuf[] = {
      SH110X_DISPLAYOFF,               // 0xAE
      SH110X_SETDISPLAYCLOCKDIV, 0x51, // 0xd5, 0x51,
      SH110X_MEMORYMODE,               // 0x20
      SH110X_SETCONTRAST, 0x4F,        // 0x81, 0x4F
      SH110X_DCDC, 0x8A,               // 0xAD, 0x8A
      SH110X_SEGREMAP,                 // 0xA0
      SH110X_COMSCANINC,               // 0xC0
      SH110X_SETDISPSTARTLINE, 0x0,    // 0xDC 0x00
      SH110X_SETDISPLAYOFFSET, 0x60,   // 0xd3, 0x60,
      SH110X_SETPRECHARGE, 0x22,       // 0xd9, 0x22,
      SH110X_SETVCOMDETECT, 0x35,      // 0xdb, 0x35,
      SH110X_SETMULTIPLEX, 0x3F,       // 0xa8, 0x3f,
      SH110X_SETPAGEADDR,                  // 0xb0
      SH110X_SETCOMPINS, 0x12,             // 0xda, 0x12,
      SH110X_DISPLAYALLON_RESUME, // 0xa4
      SH110X_NORMALDISPLAY,       // 0xa6
    };
    
    display_write_buf(cmdbuf, sizeof(cmdbuf), 0x00, true );
    CyDelay(200);
    cmdbuf[0] = SH110X_DISPLAYON;
    display_write_buf(cmdbuf, 1, 0x00, true);
    CyDelay(200);
    display_clear(); 
    /* Set display rotation to 1 (width -> height, height -> width) */
    gfx_setRotation(1);
    gfx_drawBitmap(0, 0, PPG_EduKit_Logo, 128, 64, 1);
    display_update();
    
   
}

/***************************************************************************
 * Function: display_write_buf    
 *
 * Description:  Send the display buffer over I2C
 *
 * NOTE:
 *          For submitting command sequences:  buf[0] must be 0x00
 *          For submitting bulk data (writing to display RAM): buf[0] must 
 *          be 0x40
 *
 **************************************************************************/
static uint32 display_write_buf( uint8* buf, uint16_t size, uint8_t dc_val, bool send_dc){

    uint32 status = TRANSFER_ERROR;
    I2C_BUS_MasterSendStart(DISPLAY_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    if(true == send_dc)
    {
        I2C_BUS_MasterWriteByte(dc_val, I2C_TIMEOUT);
    }
    for(int i=0; i<size; i++){  
        status=I2C_BUS_MasterWriteByte(buf[i], I2C_TIMEOUT);   
    }
    I2C_BUS_MasterSendStop(I2C_TIMEOUT);
    return status;
}

/***************************************************************************
 * Function: display_setPixel    
 *
 * Description:  Write the value of the pixel in display buffer
 **************************************************************************/
static void display_setPixel( int16_t x, int16_t y, uint16_t color ){
    
    if( (x < 0) || (x >= DISPLAYWIDTH) || (y < 0) || (y >= DISPLAYHEIGHT) )
        return;

    switch( color ){
        case WHITE: 
            PIXEL_ON(x,y);
            break;
        case BLACK:
            PIXEL_OFF(x,y);
            break;
        case INVERSE: 
            PIXEL_TOGGLE(x,y);
            break;
    }
}

/***************************************************************************
 * Function: display_clear    
 *
 * Description:  Clear display buffer and display pointers
 **************************************************************************/
void display_clear(void){
    memset( _displaybuf, 0, _displaybuf_size );
    SH110X_buffer[0] = 0x40; // to be sure its there
    
    window_x1 = 0;
    window_y1 = 0;
    window_x2 = DISPLAYWIDTH - 1;
    window_y2 = DISPLAYHEIGHT - 1;
    
}

/***************************************************************************
 * Function: display_invert    
 *
 * Description:  Set contrast (0 ... 255)
 **************************************************************************/
void display_contrast( uint8_t contrast ){
        
    uint8 cmdbuf[] = {
        SH110X_SETCONTRAST,
        contrast
    };
    display_write_buf( cmdbuf, sizeof(cmdbuf), 0x00, true ); 
}

/***************************************************************************
 * Function: display_invert    
 *
 * Description:  Invert 1 for inverse display, invert 0 for normal display
 **************************************************************************/
void display_invert( uint8_t invert ){

    uint8 cmdbuf[] = {
        0x00,  
        0
    };
    cmdbuf[1] = invert ? SH110X_INVERTDISPLAY : SH110X_NORMALDISPLAY;
    display_write_buf( cmdbuf, sizeof(cmdbuf), 0x00, false); 
}

/***************************************************************************
 * Function: display_update    
 *
 * Description:  Update the display. Send the display buffer to the OLED I2C
*                controller
 **************************************************************************/
void display_update(void) {
 
    uint8_t *ptr = _displaybuf;
    uint8_t dc_byte = 0x40;
    uint8_t pages = ((DISPLAYHEIGHT + 7) / 8);
    
    uint8_t bytes_per_page = DISPLAYWIDTH;
    uint16_t maxbuff = I2C_MAXIMUM_BUFF_SIZE - 1;
    
    
    
    uint8_t first_page = window_y1 / 8;
    uint8_t page_start = MIN(bytes_per_page, (uint8_t)window_x1);
    uint8_t page_end = (uint8_t)MAX((int)0, (int)window_x2);

    for (uint8_t p = first_page; p < pages; p++) 
    {
        uint8_t bytes_remaining = bytes_per_page;
        ptr = _displaybuf + (uint16_t)p * (uint16_t)bytes_per_page;
        ptr += page_start;
        bytes_remaining -= page_start;
        
        bytes_remaining -= (DISPLAYWIDTH - 1) - page_end;


        uint8_t cmd[] = { 0x00, (uint8_t)(SH110X_SETPAGEADDR + p),
                         (uint8_t)(0x10 + ((page_start) >> 4)),
                         (uint8_t)((page_start) & 0xF)
                        };

        display_write_buf(cmd, 4, 0x00, false);
     
        while (bytes_remaining) 
        {
            uint8_t to_write = MIN(bytes_remaining, (uint8_t)maxbuff);
            display_write_buf(ptr, to_write, dc_byte, true);
            ptr += to_write;
            bytes_remaining -= to_write;
        }
   }
}

/***************************************************************************
 * Function: display_line    
 *
 * Description:  Draws a horizontal or a vertical line
 **************************************************************************/
static void display_line( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ){

    if( x1 == x2 ){
        // vertical
        uint8_t* pstart = GDDRAM_ADDRESS(x1,y1);
        uint8_t* pend = GDDRAM_ADDRESS(x2,y2);       
        uint8_t* ptr = pstart;             
        
        while( ptr <= pend ){
            
            uint8_t mask;
            if( ptr == pstart ){
                // top
                uint8_t lbit = y1 % 8;
                // bottom (line can be very short, all inside this one byte)
                uint8_t ubit = lbit + y2 - y1;
                if( ubit >= 7 )
                    ubit = 7;
                mask = ((1 << (ubit-lbit+1)) - 1) << lbit;    
            }else if( ptr == pend ){
                // top is always bit 0, that makes it easy
                // bottom
                mask = (1 << (y2 % 8)) - 1;    
            }

            if( ptr == pstart || ptr == pend ){
                switch( color ){
                    case WHITE:     *ptr |= mask; break;
                    case BLACK:     *ptr &= ~mask; break;
                    case INVERSE:   *ptr ^= mask; break;
                };  
            }else{
                switch( color ){
                    case WHITE:     *ptr = 0xff; break;
                    case BLACK:     *ptr = 0x00; break;
                    case INVERSE:   *ptr ^= 0xff; break;
                };  
            }
            
            ptr += DISPLAYWIDTH;
        }
    }else{
        // horizontal
        uint8_t* pstart = GDDRAM_ADDRESS(x1,y1);
        uint8_t* pend = pstart + x2 - x1;
        uint8_t pixmask = GDDRAM_PIXMASK(y1);    

        uint8_t* ptr = pstart;
        while( ptr <= pend ){
            switch( color ){
                case WHITE:     *ptr |= pixmask; break;
                case BLACK:     *ptr &= ~pixmask; break;
                case INVERSE:   *ptr ^= pixmask; break;
            };
            ptr++;
        }
    }
}

/***************************************************************************
 * Function: display_usint2decascii    
 *
 * Description:  Convert a number to a string
 **************************************************************************/
uint8_t display_usint2decascii(uint16_t num, char* buffer)
{
  const unsigned short powers[] = {10u, 1u }; 
  char digit; 
  uint8_t digits = 3;
  for (uint8_t pos = 0; pos < 2; pos++) 
  {
    digit = 0;
    while (num >= powers[pos])
    {
      digit++;
      num -= powers[pos];
    }

    if (digits == 2)
    {
      if (digit == 0)
      {
        if (pos < 2)  
          digit = 0;  
      }
      else
      {
        digits = pos;
      }
    }
    buffer[pos] = digit + '0';  // Convert to ASCII
    
  }
  return digits;
}

/*==================================================================================================
 *                                      ADAFRUIT GFX LIBRARY
 *  ==============================================================================================*/

/***************************************************************************
 * Function: gfx_init    
 *
 * Description:  Init graphic library variables
 **************************************************************************/
void gfx_init( int16_t width, int16_t height ){
    WIDTH = width;
    HEIGHT = height;
    _width = WIDTH;
    _height = HEIGHT;
    
    rotation = 0;
    cursor_y = cursor_x = 0;
    textsize = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap = 1;
}

/***************************************************************************
 * Function: gfx_width    
 *
 * Description:  Get display width (per current rotation)
 **************************************************************************/
int16_t gfx_width(void){
    return _width;
}

/***************************************************************************
 * Function: gfx_height    
 *
 * Description:  Get display height (per current rotation)
 **************************************************************************/
int16_t gfx_height(void){
    return _height;
}

/***************************************************************************
 * Function: gfx_rotation    
 *
 * Description:  Get current rotation
 **************************************************************************/
uint8_t gfx_rotation(void){
    return rotation;
}

/***************************************************************************
 * Function: gfx_setCursor    
 *
 * Description:  Set cursor position
 **************************************************************************/
void gfx_setCursor( int16_t x, int16_t y ){
    cursor_x = x;
    cursor_y = y;
}

/***************************************************************************
 * Function: gfx_setTextSize    
 *
 * Description:  Set text size
 **************************************************************************/
void gfx_setTextSize( uint8_t size ){
    textsize = (size > 0) ? size : 1;
}

/***************************************************************************
 * Function: gfx_setTextColor    
 *
 * Description:  Set text color
 **************************************************************************/
void gfx_setTextColor( uint16_t color ){
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = color;
}

/***************************************************************************
 * Function: gfx_setTextBg    
 *
 * Description:  Set backgroung color
 **************************************************************************/
void gfx_setTextBg( uint16_t color ){
    textbgcolor = color;
}

/***************************************************************************
 * Function: gfx_setTextWrap    
 *
 * Description:  Set if text should be wraped or not
 **************************************************************************/
void gfx_setTextWrap( uint8 w ){
    wrap = w;
}

/***************************************************************************
 * Function: gfx_setRotation    
 *
 * Description:  Changes display rotation
 **************************************************************************/
void gfx_setRotation( uint8_t x ){
    
    rotation = (x & 3);
    switch( rotation ){
        case 0:
        case 2:
            _width = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
        case 3:
            _width = HEIGHT;
            _height = WIDTH;
        break;
    }
}

/***************************************************************************
 * Function: gfx_rotation_adjust    
 *
 * Description:  Adjusts display rotation
 **************************************************************************/
static void gfx_rotation_adjust( int16_t* px, int16_t* py ){

    int16_t y0 = *py;
    
    switch( rotation ){
        case 1:
            *py = *px;
            *px = WIDTH - y0 - 1;
            break;
        case 2:
            *px = WIDTH - *px - 1;
            *py = HEIGHT - *py - 1;
            break;
        case 3:
            *py = HEIGHT - *px - 1;
            *px = y0;
            break;
    }
}

/***************************************************************************
 * Function: gfx_drawPixel    
 *
 * Description:  Draws a pixel at a specific X,Y position
 **************************************************************************/
void gfx_drawPixel( int16_t x, int16_t y, uint16_t color ){
    
    if( (x < 0) || (x >= _width) || (y < 0) || (y >= _height) )
        return;
    
    gfx_rotation_adjust( &x, &y );

    display_setPixel(x,y,color);
}

/***************************************************************************
 * Function: gfx_hvLine    
 *
 * Description:  Helper function for gfx_drawLine, handles special cases of 
 *               horizontal and vertical lines
 **************************************************************************/
static void gfx_hvLine( int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ){
    
    if( x1 != x2 && y1 != y2 ){
        // neither vertical nor horizontal
        return;
    }    
    
    // bounds check
    if( rotation == 1 || rotation == 3 ){
        if( x1 < 0 || x1 >= HEIGHT || x2 < 0 || x2 >= HEIGHT )
            return;
        if( y1 < 0 || y1 >= WIDTH || y2 < 0 || y2 >= WIDTH )
            return;
    }else{
        if( y1 < 0 || y1 >= HEIGHT || y2 < 0 || y2 >= HEIGHT )
            return;
        if( x1 < 0 || x1 >= WIDTH || x2 < 0 || x2 >= WIDTH )
            return;
    }
    
    gfx_rotation_adjust( &x1, &y1 );
    gfx_rotation_adjust( &x2, &y2 );
    
    // ensure coords are from left to right and top to bottom
    if( (x1 == x2 && y2 < y1) || (y1 == y2 && x2 < x1) ){
        // swap as needed
        int16_t t = x1; x1 = x2; x2 = t;
        t = y1; y1 = y2; y2 = t;
    }
    
    display_line( x1, y1, x2, y2, color );
}

/***************************************************************************
 * Function: gfx_drawLine    
 *
 * Description: Function used for line drawing
 **************************************************************************/
void gfx_drawLine( int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color ){
 
    if( x0 == x1 || y0 == y1 ){
        // vertical and horizontal lines can be drawn faster
        gfx_hvLine( x0, y0, x1, y1, color );
        return;
    }
    
    int16_t t;
    
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if( steep ){
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }
    if( x0 > x1 ){
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }
    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;
    if( y0 < y1 ){
        ystep = 1;
    }else{
        ystep = -1;
    }
    for( ; x0<=x1; x0++ ){
        if( steep ){
            gfx_drawPixel( y0, x0, color );
        }else{
            gfx_drawPixel( x0, y0, color );
        }
        err -= dy;
        if( err < 0 ){
            y0 += ystep;
            err += dx;
        }
    }
}

/***************************************************************************
 * Function: gfx_fillRect    
 *
 * Description: Function used to draw a rectangle 
 **************************************************************************/
void gfx_drawRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color ){
    
    gfx_drawLine( x, y, x+w-1, y, color );
    gfx_drawLine( x, y+h-1, x+w-1, y+h-1, color );
    gfx_drawLine( x, y, x, y+h-1, color );
    gfx_drawLine( x+w-1, y, x+w-1, y+h-1, color );
}

/***************************************************************************
 * Function: gfx_fillRect    
 *
 * Description: Fill a rectangle with the specified color
 **************************************************************************/
void gfx_fillRect( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color ){
    int16_t i = 0;
    if( h > w ){
        for( i = x ; i < x+w ; i++ ){
            gfx_drawLine( i, y, i, y+h-1, color );
        }
    }else{
        for( i = y ; i < y+h ; i++ ){
            gfx_drawLine( x, i, x+w-1, i, color );
        }
    }
}

/***************************************************************************
 * Function: gfx_drawCircle    
 *
 * Description: Displays a circle
 **************************************************************************/
void gfx_drawCircle( int16_t x0, int16_t y0, int16_t r,uint16_t color ){

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    gfx_drawPixel( x0 , y0+r, color );
    gfx_drawPixel( x0 , y0-r, color );
    gfx_drawPixel( x0+r, y0 , color );
    gfx_drawPixel( x0-r, y0 , color );
    while( x < y ){
        if( f >= 0 ){
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        gfx_drawPixel( x0 + x, y0 + y, color );
        gfx_drawPixel( x0 - x, y0 + y, color );
        gfx_drawPixel( x0 + x, y0 - y, color );
        gfx_drawPixel( x0 - x, y0 - y, color );
        gfx_drawPixel( x0 + y, y0 + x, color );
        gfx_drawPixel( x0 - y, y0 + x, color );
        gfx_drawPixel( x0 + y, y0 - x, color );
        gfx_drawPixel( x0 - y, y0 - x, color );
    }
}

/***************************************************************************
 * Function: gfx_drawTriangle    
 *
 * Description: Draws a triangle
 **************************************************************************/
void gfx_drawTriangle( int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color ){
    
    gfx_drawLine( x0, y0, x1, y1, color );
    gfx_drawLine( x1, y1, x2, y2, color );
    gfx_drawLine( x2, y2, x0, y0, color );
}


/***************************************************************************
 * Function: gfx_drawChar    
 *
 * Description: Draws one character at a specific X,Y position
 **************************************************************************/
void gfx_drawChar( int16_t x, int16_t y, unsigned char c,uint16_t color, uint16_t bg, uint8_t size) {
    if( (x >= _width) || // Clip right
        (y >= _height) || // Clip bottom
        ((x + 6 * size - 1) < 0) || // Clip left
        ((y + 8 * size - 1) < 0)) // Clip top
        return;

    int8_t i = 0;
    for( i = 0 ; i < 6 ; i++ ){
        uint8_t line;
        if( i == 5 )
            line = 0x0;
        else
           line = font[(c*5)+i];
        int8_t j = 0;
        for( j = 0; j < 8 ; j++ ){
            if( line & 0x1 ){
                if( size == 1 ) // default size
                    gfx_drawPixel( x+i, y+j, color );
                else { // big size
                    gfx_fillRect( x+(i*size), y+(j*size), size, size, color );
                }
            } else if( bg != color ){
                if( size == 1 ) // default size
                    gfx_drawPixel( x+i, y+j, bg );
                else { // big size
                    gfx_fillRect( x+i*size, y+j*size, size, size, bg );
                }
            }
            line >>= 1;
        }
    }
}

/***************************************************************************
 * Function: gfx_write    
 *
 * Description: Draws one character on the OLED display
 **************************************************************************/
void gfx_write( uint8_t ch ){
    if( ch == '\n' ){
        cursor_y += textsize*8;
        cursor_x = 0;
    }else if( ch == '\r' ){
        // skip em
    }else{
        gfx_drawChar(cursor_x, cursor_y, ch, textcolor, textbgcolor, textsize);
        cursor_x += textsize*6;
        if( wrap && (cursor_x > (_width - textsize*6)) ){
            cursor_y += textsize*8;
            cursor_x = 0;
        }
    }
}

/***************************************************************************
 * Function: gfx_print    
 *
 * Description: Displays a string on the OLED display
 **************************************************************************/
void gfx_print( const char* s ){
    
    unsigned int len = strlen( s );
    unsigned int i = 0; 
    for( i = 0 ; i < len ; i++ ){
        gfx_write( s[i] );
    }
}

/***************************************************************************
 * Function: gfx_println    
 *
 * Description: Displays a string on the OLED display
 **************************************************************************/
void gfx_println( const char* s ){ 
    gfx_print( s ); 
    gfx_write( '\n' );
}

/***************************************************************************
 * Function: gfx_drawBitmap    
 *
 * Description: Draws a bitmap array at a specific x,y position
 **************************************************************************/
void gfx_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                    int16_t w, int16_t h, uint16_t color) 
{

  /* Bitmap scanline pad = whole byte */
  int16_t byteWidth = (w + 7) / 8; 
  uint8_t byte = 0;

  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = FLASH_READ_BYTE(&bitmap[j * byteWidth + i / 8]);
      if (byte & 0x80)
        gfx_drawPixel(x + i, y, color);
    }
  }

}
/* [] END OF FILE */