#include "PPG_EduKit.h"
#include <Wire.h>


#define FRAME_START               (0xDA)
#define FRAME_TERMINATOR_1        (0xEA)
#define FRAME_TERMINATOR_2        (0xDC)

const unsigned char PPG_EduKit_Logo [] PROGMEM = {
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


volatile uint16_t  PPG_EduKit::PPG_EduKit_TIA_Buffer[DMA_BUFFER_SIZE] = {0UL};
volatile uint16_t  PPG_EduKit::PPG_EduKit_HPF_Buffer[DMA_BUFFER_SIZE] = {0UL};
volatile uint16_t  PPG_EduKit::PPG_EduKit_LPF_Buffer[DMA_BUFFER_SIZE] = {0UL};
volatile uint16_t  PPG_EduKit::PPG_EduKit_AMP_Buffer[DMA_BUFFER_SIZE] = {0UL};
volatile uint16_t  PPG_EduKit::PPG_EduKIT_BufferHead = 0UL;
volatile bool      PPG_EduKit::bufferProcessed = true; 
volatile uint16_t  PPG_EduKit::adcBuffer[DMA_NUMBER_OF_BUFFERS][DMA_BUFFER_SIZE];
volatile uint8_t   PPG_EduKit::adcDMAIndex = 0U;       
volatile uint8_t   PPG_EduKit::adcTransferIndex = 0U;  
volatile bool      PPG_EduKit::dataReady = false;

uint8_t PPG_EduKit::numberOfActiveChannels = 0U;
uint8_t PPG_EduKit::adcChannels[4] = {0};
uint8_t PPG_EduKit::activeChannels = 0U;


void PPG_EduKit::begin(PPG_EK_Peripherals *peripheralsList, uint32_t samplingRate)
{
    Serial.begin(115200);

    Wire1.begin();

    pinMode(TLC5925_LE, OUTPUT);
    pinMode(TLC5925_OE, OUTPUT);
    pinMode(TLC5925_CLK, OUTPUT);
    pinMode(TLC5925_SDI, OUTPUT);

    digitalWrite(TLC5925_OE, HIGH);

#ifndef __SAM3X8E__
    pinMode(SWITCH_BUTTON, INPUT_PULLUP);
#endif

    if(peripheralsList->neoPixel == ENABLE_PERIPHERAL)
    {
        pixels.begin();
        delay(500);
        pixels.clear();
        pixels.show();
        delay(1000);
    }

    if(peripheralsList->oledDisplay == ENABLE_PERIPHERAL)
    {
        OLED_displaySetup();
    }
    
    if(peripheralsList->ppgSensor == ENABLE_PERIPHERAL)
    {
        if (!ppgSensor.begin(Wire1))
        {
            Serial.println("MAX30105 error!!!");
        }

        ppgSensor.setup(MAX30105_LED_BRIGHTNESS, MAX30105_SAMPLE_AVERAGE, MAX30105_LED_MODE, 
                          MAX30105_SAMPLE_RATE, MAX30105_PUSLE_WIDTH, MAX30105_ADC_RANGE); 
    }
    
    if(peripheralsList->read_TIA == ENABLE_PERIPHERAL)
    {
        activeChannels |= ADC_TIA;
    }
    if(peripheralsList->read_HPF == ENABLE_PERIPHERAL)
    {
        activeChannels |= ADC_HPF;
    }
    if(peripheralsList->read_LPF == ENABLE_PERIPHERAL)
    {
        activeChannels |= ADC_LPF;
    }
    if(peripheralsList->read_AMP == ENABLE_PERIPHERAL)
    {
        activeChannels |= ADC_AMP;
    }


    if(activeChannels != 0x00U)
    {
       ADC_Init(activeChannels, samplingRate);
    } 
    
}


void PPG_EduKit::OLED_displaySetup(void)
{
    display.begin(OLED_I2C_ADDRESS, true); 
    delay(1000);

    display.clearDisplay();
    display.setRotation(1);
    display.drawBitmap(0, 0, PPG_EduKit_Logo, 128, 64, 1); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.display();
    

    pinMode(OLED_BUTTON_A, INPUT_PULLUP);
    pinMode(OLED_BUTTON_B, INPUT_PULLUP);
    pinMode(OLED_BUTTON_C, INPUT_PULLUP);

}

void PPG_EduKit::enableLed(PPG_EK_Led ledType, uint16_t ledCurrent, boolean setCurrent)
{
    if(setCurrent == true)
    {
        AD5273_setLedCurrent(ledCurrent);    
    }

    switch(ledType)
    {
        case RED_CHANNEL:
            TLC5925_enableRed();
            break;
        case GREEN_CHANNEL:
            TLC5925_enableGreen(); 
            break;
        case IR_CHANNEL:
            TLC5925_enableIR(); 
            break;

        default:
            AD5273_setLedCurrent(0x00UL); 
            break;
    }

}

void PPG_EduKit::TLC5925_enableRed(void)
{
    /* LE must be low while data is being shifted in */
    digitalWrite(TLC5925_LE, LOW);
    /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */ 
    digitalWrite(TLC5925_CLK, LOW);
    /* OE high keep leds turned off while data is shifted in */
    digitalWrite(TLC5925_OE, HIGH); 
    
    /* Shift out the data to turn green led on: 0000 0000 0000 0010. shiftOut() shifts 1 byte at a time, so the function is called twice. */
    /* First byte corresponding to outputs 15 to 8. ALL UNUSED */
    shiftOut(TLC5925_SDI, TLC5925_CLK, MSBFIRST, 0x00); 
    /* Second byte corresponding to outputs 7 to 0. 7 to 5 unused. */
    shiftOut(TLC5925_SDI, TLC5925_CLK, MSBFIRST, 0b00000001); 
    
    /* LE toggled to latch in the shifted data */
    digitalWrite(TLC5925_LE, HIGH);
    digitalWrite(TLC5925_LE, LOW);
    /* Enable output drive */
    digitalWrite(TLC5925_OE, LOW);
}


void PPG_EduKit::TLC5925_enableGreen(void)
{
    /* LE must be low while data is being shifted in */
    digitalWrite(TLC5925_LE, LOW);
    /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */ 
    digitalWrite(TLC5925_CLK, LOW);
    /* OE high keep leds turned off while data is shifted in */
    digitalWrite(TLC5925_OE, HIGH); 
    
    /* Shift out the data to turn green led on: 0000 0000 0000 0010. shiftOut() shifts 1 byte at a time, so the function is called twice. */
    /* First byte corresponding to outputs 15 to 8. ALL UNUSED */
    shiftOut(TLC5925_SDI, TLC5925_CLK, MSBFIRST, 0x00); 
    /* Second byte corresponding to outputs 7 to 0. 7 to 5 unused. */
    shiftOut(TLC5925_SDI, TLC5925_CLK, MSBFIRST, 0b00000010); 
    
    /* LE toggled to latch in the shifted data */
    digitalWrite(TLC5925_LE, HIGH);
    digitalWrite(TLC5925_LE, LOW);
    /* Enable output drive */
    digitalWrite(TLC5925_OE, LOW);
}

void PPG_EduKit::TLC5925_enableIR(void)
{
    /* LE must be low while data is being shifted in */
    digitalWrite(TLC5925_LE, LOW);
    /* Data shifted with CLK rising edge. This makes sure the CLK starts in LOW position */ 
    digitalWrite(TLC5925_CLK, LOW);
    /* OE high keep leds turned off while data is shifted in */
    digitalWrite(TLC5925_OE, HIGH); 
    
    /* Shift out the data to turn green led on: 0000 0000 0000 0010. shiftOut() shifts 1 byte at a time, so the function is called twice. */
    /* First byte corresponding to outputs 15 to 8. ALL UNUSED */
    shiftOut(TLC5925_SDI, TLC5925_CLK, MSBFIRST, 0x00); 
    /* Second byte corresponding to outputs 7 to 0. 7 to 5 unused. */
    shiftOut(TLC5925_SDI, TLC5925_CLK, MSBFIRST, 0b00001100); 
    
    /* LE toggled to latch in the shifted data */
    digitalWrite(TLC5925_LE, HIGH);
    digitalWrite(TLC5925_LE, LOW);
    /* Enable output drive */
    digitalWrite(TLC5925_OE, LOW);

}

void PPG_EduKit::AD5273_setLedCurrent(uint16_t val)
{
    uint8_t returned_bytes = 0U;
    uint16_t resVal = 0UL;
    uint8_t rdacVal = 0U;

    /* Current to resistance conversion. See 9.2.2.1 chapter in tlc5925.pdf datasheet */
    resVal = (uint16_t)(1000 * ((float)(1.21 * 18)/val));

    if(0x00UL != resVal)
    {
      /* R_WB(D) = (D * R_AB)/63 + R_W 
        rdacVal is D in this equation*/
      rdacVal = (uint8_t)ceil(((resVal - AD5273_R_W_VALUE)/ (float)AD5273_R_AB_VALUE) * 63.0);
    }

    /* Start communicating to the AD5273 at the specific address */
    Wire1.beginTransmission((uint8_t) AD5273_I2C_ADDRESS);
    /* Send the first byte 0h, to indicate the non-OTP operation */
    Wire1.write(0x00);
    /* Send the D value from 0 to  63 for the digipot resisitance */
    Wire1.write(rdacVal);
    /* Complete Transmission */
    Wire1.endTransmission();

    /* Read Written Value from the device (Read from AD5273) */
    /* Request 2 bytes from the AD5273 */
    returned_bytes = Wire1.requestFrom(AD5273_I2C_ADDRESS, 1);
    /* If no data was returned, something was wrong during the connection */
    if(returned_bytes == 0x00)
    {
        while(1); //halt the program
    }

    /* Read the digipot and store into a variable. It's two's complement (16bits signed) data */
    int readValue =  Wire1.read();
    if(rdacVal != readValue)
    {
        while(1); //halt the program
    }
}

uint16_t* PPG_EduKit::readChannel(uint8_t channel, uint32_t *bufferLength)
{
    bool validChannel = false;
    uint8_t channelIndex = 0U;
    uint32_t samples = 0UL;
    uint16_t *pBuffer = NULL;

    for(uint8_t i = 0; i < numberOfActiveChannels; i++)
    {
        if(adcChannels[i] == channel) 
        {
            validChannel = true;
            channelIndex = i;
            break;
        }
    }

    if(ADC_Available() && (validChannel = true))
    {
        uint16_t* cBuf = ADC_GetFilledBuffer();
        switch(channel)
        {
            case ADC_TIA:
                memset((void *)PPG_EduKit_TIA_Buffer, 0, DMA_BUFFER_SIZE);
                break;
            case ADC_LPF:
                memset((void *)PPG_EduKit_LPF_Buffer, 0, DMA_BUFFER_SIZE);
                break;
            case ADC_HPF:
                memset((void *)PPG_EduKit_HPF_Buffer, 0, DMA_BUFFER_SIZE);
                break;
            case ADC_AMP:
                memset((void *)PPG_EduKit_AMP_Buffer, 0, DMA_BUFFER_SIZE);
                break;
        }

        for (uint32_t i = channelIndex; i < DMA_BUFFER_SIZE; i = i + numberOfActiveChannels)
        {
            switch(channel)
            {
                case ADC_TIA:
                    PPG_EduKit_TIA_Buffer[samples] = cBuf[i];
                    pBuffer = (uint16_t *) PPG_EduKit_TIA_Buffer;
                    break;
                case ADC_LPF:
                    PPG_EduKit_LPF_Buffer[samples] = cBuf[i];
                    pBuffer = (uint16_t *) PPG_EduKit_LPF_Buffer;
                    break;
                case ADC_HPF:
                    PPG_EduKit_HPF_Buffer[samples] = cBuf[i];
                    pBuffer = (uint16_t *) PPG_EduKit_HPF_Buffer;
                    break;
                case ADC_AMP:
                    PPG_EduKit_AMP_Buffer[samples] = cBuf[i];
                    pBuffer = (uint16_t *) PPG_EduKit_AMP_Buffer;
                    break;
            }
            samples++;
        }

        ADC_ReadBufferDone();

        *bufferLength = samples;
    }


    return pBuffer;
}


void PPG_EduKit::ADC_Init(uint8_t channels, uint32_t samplingRate)
{
    uint8_t count = 0;
    if(channels & ADC_AMP)
    {
        adcChannels[count] = ADC_AMP;
        count++;
    }

    if(channels & ADC_LPF)
    {
        adcChannels[count] = ADC_LPF;
        count++;
    }

    if(channels & ADC_HPF)
    {
        adcChannels[count] = ADC_HPF;
        count++;
    }

    if(channels & ADC_TIA)
    {
        adcChannels[count] = ADC_TIA;
        count++;
    }

    numberOfActiveChannels = count;
 
    pmc_enable_periph_clk(ID_TC0);


    // Configure timer
    TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET | TC_CMR_ASWTRG_CLEAR | TC_CMR_TCCLKS_TIMER_CLOCK1);

    // It is good to have the timer 0 on PIN2, good for Debugging
    //int result = PIO_Configure( PIOB, PIO_PERIPH_B, PIO_PB25B_TIOA0, PIO_DEFAULT);

    // Configure ADC pin A7
    //  the below code is taken from adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);

    ADC->ADC_CR = ADC_CR_SWRST;         // Reset the controller.
    ADC->ADC_MR = 0;                    // Reset Mode Register.
    ADC->ADC_PTCR = (ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS); // Reset PDC transfer.

    ADC->ADC_MR |= ADC_MR_PRESCAL(3);   // ADC clock = MSCK/((PRESCAL+1)*2), 13 -> 750000 Sps
    ADC->ADC_MR |= ADC_MR_STARTUP_SUT0; // What is this by the way?
    ADC->ADC_MR |= ADC_MR_TRACKTIM(15);
    ADC->ADC_MR |= ADC_MR_TRANSFER(1);
    ADC->ADC_MR |= ADC_MR_TRGEN_EN;
    ADC->ADC_MR |= ADC_MR_TRGSEL_ADC_TRIG1; // selecting TIOA0 as trigger.
    ADC->ADC_CHER = channels;

    /* Interupts */
    ADC->ADC_IDR   = ~ADC_IDR_ENDRX;
    ADC->ADC_IER   =  ADC_IER_ENDRX;
    /* Waiting for ENDRX as end of the transfer is set
      when the current DMA transfer is done (RCR = 0), i.e. it doesn't include the
      next DMA transfer.

      If we trigger on RXBUFF This flag is set if there is no more DMA transfer in
      progress (RCR = RNCR = 0). Hence we may miss samples.
    */

    
    unsigned int cycles = 42000000 / samplingRate;

    /*  timing of ADC */
    TC_SetRC(TC0, 0, cycles);      // TIOA0 goes HIGH on RC.
    TC_SetRA(TC0, 0, cycles / 2);  // TIOA0 goes LOW  on RA.

    // We have to reinitalise just in case the Sampler is stopped and restarted...
    dataReady = false;
    adcDMAIndex = 0;
    adcTransferIndex = 0;
    for (int i = 0; i < DMA_NUMBER_OF_BUFFERS; i++)
    {
      memset((void *)adcBuffer[i], 0, DMA_BUFFER_SIZE);
    }

    ADC->ADC_RPR  = (unsigned long) adcBuffer[adcDMAIndex];  // DMA buffer
    ADC->ADC_RCR  = (unsigned int)  DMA_BUFFER_SIZE;  // ADC works in half-word mode.
    ADC->ADC_RNPR = (unsigned long) adcBuffer[(adcDMAIndex + 1)];  // next DMA buffer
    ADC->ADC_RNCR = (unsigned int)  DMA_BUFFER_SIZE;

    // Enable interrupts
    NVIC_EnableIRQ(ADC_IRQn);
    ADC->ADC_PTCR  =  ADC_PTCR_RXTEN;  // Enable receiving data.
    ADC->ADC_CR   |=  ADC_CR_START;    //start waiting for trigger.

    // Start timer
    TC0->TC_CHANNEL[0].TC_SR;
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN;
    TC_Start(TC0, 0);
}


uint16_t* PPG_EduKit::ADC_GetFilledBuffer()
{
  return (uint16_t*) adcBuffer[adcTransferIndex];
}

bool PPG_EduKit::ADC_Available()
{
  return dataReady;
}

void PPG_EduKit::ADC_ReadBufferDone()
{
  dataReady = false;
}

void PPG_EduKit::ADC_HandlerISR()
{
    unsigned long status = ADC->ADC_ISR;
    if (status & ADC_ISR_ENDRX)  
    {
        adcTransferIndex = adcDMAIndex;
        adcDMAIndex = (adcDMAIndex + 1) % DMA_NUMBER_OF_BUFFERS;
        ADC->ADC_RNPR  = (unsigned long) adcBuffer[(adcDMAIndex + 1) % DMA_NUMBER_OF_BUFFERS];
        ADC->ADC_RNCR  = DMA_BUFFER_SIZE;
        dataReady = true;
    }
}


uint8_t* PPG_EduKit::createSerialFrame(void *inputData, uint16_t noOfBytes, frameParams_t *serialFrameStruct)
{
  if(DMA_BUFFER_SIZE + 5 < noOfBytes)
    serialFrameStruct->frameType = (frameType_t) 0xFF;
    
  serialFrame[0] = FRAME_START;
  
  switch(serialFrameStruct->frameType)
  {
    case CHANNEL_DATA:
        serialFrame[1] = CHANNEL_DATA;
        serialFrame[2] = serialFrameStruct->tissueDetected;
        serialFrame[3] = serialFrameStruct->params.wavelength;
        memcpy(&serialFrame[4], inputData, noOfBytes);
        serialFrame[noOfBytes + 4] = FRAME_TERMINATOR_1;
        serialFrame[noOfBytes + 5] = FRAME_TERMINATOR_2;
        break;
        
    case PARAMS:

        break;
        
    case DEBUG_FRAME:
        break;
        
    default:
        serialFrame[1] = 0xDE;
        serialFrame[2] = 0xAD;
        serialFrame[3] = 0xDE;
        serialFrame[4] = 0xAD;
        serialFrame[5] = FRAME_TERMINATOR_1;
        serialFrame[6] = FRAME_TERMINATOR_2;
        break;
  }

  return serialFrame;
}

void PPG_EduKit::sendFrame(uint8_t *pFrame)
{ 
  bool terminator_1 = false;
  bool endOfFrame = false;
  
  while(!endOfFrame)
  {
    if (*pFrame < 16) {Serial.print("0");}
    Serial.print(*pFrame, HEX);  
    
    if(FRAME_TERMINATOR_1 == *pFrame)
    {
      terminator_1 = true;
    }

    if((true == terminator_1) && (FRAME_TERMINATOR_2 == *pFrame))
    {
      endOfFrame = true;
    }
   
    pFrame++;

  }
  Serial.print('\n');
}


uint8_t PPG_EduKit::int2acii(uint16_t num, char* buffer)
{
  const unsigned short powers[] = {10u, 1u }; 
  char digit; 
  uint8_t digits = 2;
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









void ADC_Handler() 
{
    PPG_EduKit::ADC_HandlerISR();
}


