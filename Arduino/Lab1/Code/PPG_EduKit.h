#include "Arduino.h"
#include <memory>

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <Protocentral_MAX30205.h>
#include <MAX30105.h>


#define   ENABLE_PERIPHERAL         (0xAA)
#define   DISABLE_PERIPHERAL        (0x00)

#define   ADC_TIA     ADC_CHER_CH7
#define   ADC_HPF     ADC_CHER_CH6
#define   ADC_LPF     ADC_CHER_CH5
#define   ADC_AMP     ADC_CHER_CH4

#define MAX_NUM_CHANNELS            4
#define MAX_BUFFER_SIZE             30
#define DMA_BUFFER_SIZE             (MAX_BUFFER_SIZE * MAX_NUM_CHANNELS)
#define DMA_NUMBER_OF_BUFFERS       3

#define I2C_SPEED_STANDARD        100000
#define I2C_SPEED_FAST            400000

#define OLED_I2C_ADDRESS    0x3CU
#define OLED_BUTTON_A       9U
#define OLED_BUTTON_B       8U
#define OLED_BUTTON_C       7U

#define TLC5925_LE          2U
#define TLC5925_OE          5U
#define TLC5925_CLK         3U
#define TLC5925_SDI         4U

#define AD5273_I2C_ADDRESS  0x2CU
#define AD5273_R_W_VALUE    60U
#define AD5273_R_AB_VALUE   10000UL

#define MAX30105_LED_BRIGHTNESS      60U
#define MAX30105_SAMPLE_AVERAGE      4U
#define MAX30105_LED_MODE            2U
#define MAX30105_SAMPLE_RATE         100UL
#define MAX30105_PUSLE_WIDTH         411UL
#define MAX30105_ADC_RANGE           16384UL

#define NEOPIXEL_PIN                 0U
#define SWITCH_BUTTON                1U
                                      
typedef enum{
    RED_CHANNEL,
    GREEN_CHANNEL,
    IR_CHANNEL
} PPG_EK_Led;

typedef struct{

    uint8_t oledDisplay;
    uint8_t neoPixel;
    uint8_t tempSensor;
    uint8_t ppgSensor;
    uint8_t read_TIA;
    uint8_t read_HPF;
    uint8_t read_LPF;
    uint8_t read_AMP;

} PPG_EK_Peripherals;


/* @brief Enum with frame types */
typedef enum 
{ 
    CHANNEL_DATA = 0x7C, 
    PARAMS = 0x83, 
    DEBUG_FRAME = 0xF2
} frameType_t;

/*==================================================================================================
 *                                STRUCTURES AND OTHER TYPEDEFS
 *  ==============================================================================================*/

/* @brief Store params for the debug frame. Not supported yet. */
typedef struct
{
  uint8_t dummy;
}debugType_t;


/* @brief Store params for differend kind of serial frames */
typedef struct
{
  frameType_t frameType;
  union
  {
    uint8_t hr_spo2[3];
    uint8_t wavelength;
    debugType_t debugParam;
  }params;
  bool tissueDetected;
}frameParams_t;


class PPG_EduKit {

    public:
        PPG_EduKit(void) : display(64, 128, &Wire1), pixels(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800) 
        {
            _display = (std::unique_ptr<Adafruit_SH110X>) &display;
            _pixels = (std::unique_ptr<Adafruit_NeoPixel>) &pixels;
            _ppgSensor = (std::unique_ptr<MAX30105>) &ppgSensor;
        }

        void begin(PPG_EK_Peripherals *peripheralsList, uint32_t samplingRate);
        void enableLed(PPG_EK_Led ledType, uint16_t ledCurrent, boolean setCurrent);
        uint16_t* readChannel(uint8_t channel, uint32_t *bufferLength);
        uint8_t* createSerialFrame(void *inputData, uint16_t noOfBytes, frameParams_t *serialFrameStruct);
        void sendFrame(uint8_t *pFrame);
        static void ADC_HandlerISR();
        static uint8_t int2acii(uint16_t num, char* buffer);
        Adafruit_SH110X& getHandler_OLED() { return *(this->_display); }
        Adafruit_NeoPixel& getHandler_NeoPixel() { return *(this->_pixels); }
        MAX30105& getHandler_PpgSensor() { return *(this->_ppgSensor); }
        

        volatile static uint16_t  PPG_EduKit_TIA_Buffer[DMA_BUFFER_SIZE];
        volatile static uint16_t  PPG_EduKit_HPF_Buffer[DMA_BUFFER_SIZE];
        volatile static uint16_t  PPG_EduKit_LPF_Buffer[DMA_BUFFER_SIZE];
        volatile static uint16_t  PPG_EduKit_AMP_Buffer[DMA_BUFFER_SIZE];
        volatile static uint16_t  PPG_EduKIT_BufferHead;
        volatile static bool bufferProcessed; 
        static  uint8_t numberOfActiveChannels;
        static  uint8_t adcChannels[4];
        static  uint8_t activeChannels;
        
    private:
        TwoWire *_i2cPort;
        Adafruit_SH110X display;
        Adafruit_NeoPixel pixels;        
        MAX30105 ppgSensor;
        

        uint32_t sampleingRate;
        uint8_t serialFrame[DMA_BUFFER_SIZE + 5];

        static volatile uint16_t adcBuffer[DMA_NUMBER_OF_BUFFERS][DMA_BUFFER_SIZE];
        static volatile uint8_t adcDMAIndex;        
        static volatile uint8_t adcTransferIndex;   
        static volatile bool dataReady;

        std::unique_ptr<Adafruit_SH110X> _display;
        std::unique_ptr<Adafruit_NeoPixel> _pixels;
        std::unique_ptr<MAX30105> _ppgSensor;
   
        void OLED_displaySetup(void);
        void TLC5925_enableRed(void);
        void TLC5925_enableGreen(void);
        void TLC5925_enableIR(void);
        void AD5273_setLedCurrent(uint16_t val);
        void ADC_Init(uint8_t channels, uint32_t sampleingRate);
        bool ADC_Available(void);
        uint16_t* ADC_GetFilledBuffer(void);
        void ADC_ReadBufferDone(void);


};
