/**
 *   @file    MAX30102.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   MAX30105 library
 *   @details MAX30105 library ported to C and PSOC6 project.
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


/*==================================================================================================
 *                                        INCLUDE FILES
 * 1) system and project includes
 * 2) needed interfaces from external units
 * 3) internal and external interfaces from this unit
 *  ==============================================================================================*/

/* @brief Include PSOC generated files */
#include "project.h"

/* @brief Include custom libraries for PPG EduKit */
#include "MAX30102.h"
#include "milliseconds.h"
#include "I2C_BUS.h"

/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

/* @brief I2C timeout value */
#define I2C_TIMEOUT                 (100UL)
/* @brief I2C buffer length */
#define I2C_BUFFER_LENGTH               32
/* @brief I2C peripheral hw */
#define I2C_MAX30105_HW             I2C_BUS_HW

/* @brief I2C device address */
#define MAX30105_ADDRESS                0x57

/* @brief Predefined threshold for uncovered sensors sufrace */
#define NO_FINGER_THRESHOLD                   (50000UL)

/* @brief  Each long is 4 bytes so limit this to fit on your micro */
#define STORAGE_SIZE 4

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/

/* @brief This is our circular buffer of readings from the sensor */
typedef struct Record
{
   uint32_t red[STORAGE_SIZE];
   uint32_t IR[STORAGE_SIZE];
   uint32_t green[STORAGE_SIZE];
   uint8_t head;
   uint8_t tail;
} sense_struct; 

/*==================================================================================================
 *                                      LOCAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      LOCAL VARIABLES
 *  ==============================================================================================*/

/* @brief Status Registers */
static const uint8_t MAX30105_INTSTAT1 =		0x00;
static const uint8_t MAX30105_INTSTAT2 =		0x01;
static const uint8_t MAX30105_INTENABLE1 =		0x02;
static const uint8_t MAX30105_INTENABLE2 =		0x03;

/* @brief FIFO Registers */
static const uint8_t MAX30105_FIFOWRITEPTR = 	0x04;
static const uint8_t MAX30105_FIFOOVERFLOW = 	0x05;
static const uint8_t MAX30105_FIFOREADPTR = 	0x06;
static const uint8_t MAX30105_FIFODATA =		0x07;

/* @brief Configuration Registers */
static const uint8_t MAX30105_FIFOCONFIG = 		0x08;
static const uint8_t MAX30105_MODECONFIG = 		0x09;
static const uint8_t MAX30105_PARTICLECONFIG = 	0x0A;    // Note, sometimes listed as "SPO2" config in datasheet (pg. 11)
static const uint8_t MAX30105_LED1_PULSEAMP = 	0x0C;
static const uint8_t MAX30105_LED2_PULSEAMP = 	0x0D;
static const uint8_t MAX30105_LED3_PULSEAMP = 	0x0E;
static const uint8_t MAX30105_LED_PROX_AMP = 	0x10;
static const uint8_t MAX30105_MULTILEDCONFIG1 = 0x11;
static const uint8_t MAX30105_MULTILEDCONFIG2 = 0x12;

/* @brief Die Temperature Registers */
static const uint8_t MAX30105_DIETEMPINT = 		0x1F;
static const uint8_t MAX30105_DIETEMPFRAC = 	0x20;
static const uint8_t MAX30105_DIETEMPCONFIG = 	0x21;

/* @brief Proximity Function Registers */
static const uint8_t MAX30105_PROXINTTHRESH = 	0x30;

/* @brief Part ID Registers */
static const uint8_t MAX30105_REVISIONID = 		0xFE;
static const uint8_t MAX30105_PARTID = 			0xFF;    // Should always be 0x15. Identical to MAX30102.

/* @brief MAX30105 Commands */

/* @brief Interrupt configuration (pg 13, 14) */
static const uint8_t MAX30105_INT_A_FULL_MASK =		(uint8_t)~0b10000000;
static const uint8_t MAX30105_INT_A_FULL_ENABLE = 	0x80;
static const uint8_t MAX30105_INT_A_FULL_DISABLE = 	0x00;

static const uint8_t MAX30105_INT_DATA_RDY_MASK = (uint8_t)~0b01000000;
static const uint8_t MAX30105_INT_DATA_RDY_ENABLE =	0x40;
static const uint8_t MAX30105_INT_DATA_RDY_DISABLE = 0x00;

static const uint8_t MAX30105_INT_ALC_OVF_MASK = (uint8_t)~0b00100000;
static const uint8_t MAX30105_INT_ALC_OVF_ENABLE = 	0x20;
static const uint8_t MAX30105_INT_ALC_OVF_DISABLE = 0x00;

static const uint8_t MAX30105_INT_PROX_INT_MASK = (uint8_t)~0b00010000;
static const uint8_t MAX30105_INT_PROX_INT_ENABLE = 0x10;
static const uint8_t MAX30105_INT_PROX_INT_DISABLE = 0x00;

static const uint8_t MAX30105_INT_DIE_TEMP_RDY_MASK = (uint8_t)~0b00000010;
static const uint8_t MAX30105_INT_DIE_TEMP_RDY_ENABLE = 0x02;
static const uint8_t MAX30105_INT_DIE_TEMP_RDY_DISABLE = 0x00;

static const uint8_t MAX30105_SAMPLEAVG_MASK =	(uint8_t)~0b11100000;
static const uint8_t MAX30105_SAMPLEAVG_1 = 	0x00;
static const uint8_t MAX30105_SAMPLEAVG_2 = 	0x20;
static const uint8_t MAX30105_SAMPLEAVG_4 = 	0x40;
static const uint8_t MAX30105_SAMPLEAVG_8 = 	0x60;
static const uint8_t MAX30105_SAMPLEAVG_16 = 	0x80;
static const uint8_t MAX30105_SAMPLEAVG_32 = 	0xA0;

static const uint8_t MAX30105_ROLLOVER_MASK = 	0xEF;
static const uint8_t MAX30105_ROLLOVER_ENABLE = 0x10;
static const uint8_t MAX30105_ROLLOVER_DISABLE = 0x00;

static const uint8_t MAX30105_A_FULL_MASK = 	0xF0;

/* @brief Mode configuration commands (page 19) */
static const uint8_t MAX30105_SHUTDOWN_MASK = 	0x7F;
static const uint8_t MAX30105_SHUTDOWN = 		0x80;
static const uint8_t MAX30105_WAKEUP = 			0x00;

static const uint8_t MAX30105_RESET_MASK = 		0xBF;
static const uint8_t MAX30105_RESET = 			0x40;

static const uint8_t MAX30105_MODE_MASK = 		0xF8;
static const uint8_t MAX30105_MODE_REDONLY = 	0x02;
static const uint8_t MAX30105_MODE_REDIRONLY = 	0x03;
static const uint8_t MAX30105_MODE_MULTILED = 	0x07;

/* @brief Particle sensing configuration commands (pgs 19-20) */
static const uint8_t MAX30105_ADCRANGE_MASK = 	0x9F;
static const uint8_t MAX30105_ADCRANGE_2048 = 	0x00;
static const uint8_t MAX30105_ADCRANGE_4096 = 	0x20;
static const uint8_t MAX30105_ADCRANGE_8192 = 	0x40;
static const uint8_t MAX30105_ADCRANGE_16384 = 	0x60;

static const uint8_t MAX30105_SAMPLERATE_MASK = 0xE3;
static const uint8_t MAX30105_SAMPLERATE_50 = 	0x00;
static const uint8_t MAX30105_SAMPLERATE_100 = 	0x04;
static const uint8_t MAX30105_SAMPLERATE_200 = 	0x08;
static const uint8_t MAX30105_SAMPLERATE_400 = 	0x0C;
static const uint8_t MAX30105_SAMPLERATE_800 = 	0x10;
static const uint8_t MAX30105_SAMPLERATE_1000 = 0x14;
static const uint8_t MAX30105_SAMPLERATE_1600 = 0x18;
static const uint8_t MAX30105_SAMPLERATE_3200 = 0x1C;

static const uint8_t MAX30105_PULSEWIDTH_MASK = 0xFC;
static const uint8_t MAX30105_PULSEWIDTH_69 = 	0x00;
static const uint8_t MAX30105_PULSEWIDTH_118 = 	0x01;
static const uint8_t MAX30105_PULSEWIDTH_215 = 	0x02;
static const uint8_t MAX30105_PULSEWIDTH_411 = 	0x03;

/* @brief Multi-LED Mode configuration (pg 22) */
static const uint8_t MAX30105_SLOT1_MASK = 		0xF8;
static const uint8_t MAX30105_SLOT2_MASK = 		0x8F;
static const uint8_t MAX30105_SLOT3_MASK = 		0xF8;
static const uint8_t MAX30105_SLOT4_MASK = 		0x8F;

static const uint8_t SLOT_NONE = 				0x00;
static const uint8_t SLOT_RED_LED = 			0x01;
static const uint8_t SLOT_IR_LED = 				0x02;
static const uint8_t SLOT_GREEN_LED = 			0x03;
static const uint8_t SLOT_NONE_PILOT = 			0x04;
static const uint8_t SLOT_RED_PILOT =			0x05;
static const uint8_t SLOT_IR_PILOT = 			0x06;
static const uint8_t SLOT_GREEN_PILOT = 		0x07;

static const uint8_t MAX_30105_EXPECTEDPARTID = 0x15;

/*==================================================================================================
 *                                      GLOBAL CONSTANTS
 *  ==============================================================================================*/


/*==================================================================================================
 *                                      GLOBAL VARIABLES
 *  ==============================================================================================*/

/* @brief Stores the number of samples aquired in the input buffer */
uint32_t samplesTaken = 0UL;
/* @brief Channel buffers */
uint16_t FIFO_Buffer[3 * FIFO_NUMBER_OF_SAMPLES];

/* @brief Stores sensors revision ID */
uint8_t gRevisionID;

/* @brief Stores the number of active LEDs */
uint8_t gActiveLEDs;

/* @brief Circular buffer for PPG readings */
sense_struct sense;
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
 * Function: MAX30105_Setup    
 *
 * Description: Init I2C peripheral and setup MAX30105 sensor 
 **************************************************************************/
void MAX30105_Setup(uint8_t  powerLevel, uint8_t sampleAverage, uint8_t ledMode, uint32_t sampleRate, uint32_t pulseWidth, uint32_t adcRange)
{
    
    cy_en_scb_i2c_status_t initStatus;
    uint32_t dataRate;
    uint8_t rxBuffer = 0U;
    
    /* Configure component. */ 
    initStatus = I2C_BUS_Init(&I2C_BUS_config);
    if(initStatus!=CY_SCB_I2C_SUCCESS)
    {
        HandleError();
    }
    
    dataRate = I2C_BUS_SetDataRate(I2C_BUS_DATA_RATE_HZ, I2C_BUS_CLK_FREQ_HZ);
    
    /* check whether data rate set is not greather then required reate. */
    if(dataRate > I2C_BUS_DATA_RATE_HZ)
    {
        HandleError();
    }
    
    Cy_SCB_I2C_Enable(I2C_MAX30105_HW);
    
    /* Check connection */
    MAX30105_ReadRegister8(&rxBuffer, MAX30105_PARTID);
    
    if(MAX_30105_EXPECTEDPARTID != rxBuffer)
    {
        HandleError();
    }
    
    MAX30105_ReadRegister8(&rxBuffer, MAX30105_REVISIONID);
    
    gRevisionID = rxBuffer;
    
    MAX30105_SoftReset();
    
    
    //FIFO Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //The chip will average multiple samples of same type together if you wish
    if (sampleAverage == 1) MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_1); //No averaging per FIFO record
    else if (sampleAverage == 2) MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_2);
    else if (sampleAverage == 4) MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_4);
    else if (sampleAverage == 8) MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_8);
    else if (sampleAverage == 16) MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_16);
    else if (sampleAverage == 32) MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_32);
    else MAX30105_SetFIFOAverage(MAX30105_SAMPLEAVG_4);

    //setFIFOAlmostFull(2); //Set to 30 samples to trigger an 'Almost Full' interrupt
    MAX30105_EnableFIFORollover(); //Allow FIFO to wrap/roll over
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Mode Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    if (ledMode == 3)  MAX30105_SetLEDMode(MAX30105_MODE_MULTILED); //Watch all three LED channels
    else if (ledMode == 2)  MAX30105_SetLEDMode(MAX30105_MODE_REDIRONLY); //Red and IR
    else  MAX30105_SetLEDMode(MAX30105_MODE_REDONLY); //Red only
    gActiveLEDs = ledMode; //Used to control how many bytes to read from FIFO buffer
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Particle Sensing Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    if(adcRange < 4096)  MAX30105_SetADCRange(MAX30105_ADCRANGE_2048); //7.81pA per LSB
    else if(adcRange < 8192)  MAX30105_SetADCRange(MAX30105_ADCRANGE_4096); //15.63pA per LSB
    else if(adcRange < 16384)  MAX30105_SetADCRange(MAX30105_ADCRANGE_8192); //31.25pA per LSB
    else if(adcRange == 16384)  MAX30105_SetADCRange(MAX30105_ADCRANGE_16384); //62.5pA per LSB
    else  MAX30105_SetADCRange(MAX30105_ADCRANGE_2048);

    if (sampleRate < 100)  MAX30105_SetSampleRate(MAX30105_SAMPLERATE_50); //Take 50 samples per second
    else if (sampleRate < 200)  MAX30105_SetSampleRate(MAX30105_SAMPLERATE_100);
    else if (sampleRate < 400)  MAX30105_SetSampleRate(MAX30105_SAMPLERATE_200);
    else if (sampleRate < 800)  MAX30105_SetSampleRate(MAX30105_SAMPLERATE_400);
    else if (sampleRate < 1000)  MAX30105_SetSampleRate(MAX30105_SAMPLERATE_800);
    else if (sampleRate < 1600)  MAX30105_SetSampleRate(MAX30105_SAMPLERATE_1000);
    else if (sampleRate < 3200) MAX30105_SetSampleRate(MAX30105_SAMPLERATE_1600);
    else if (sampleRate == 3200) MAX30105_SetSampleRate(MAX30105_SAMPLERATE_3200);
    else MAX30105_SetSampleRate(MAX30105_SAMPLERATE_50);

    //The longer the pulse width the longer range of detection you'll have
    //At 69us and 0.4mA it's about 2 inches
    //At 411us and 0.4mA it's about 6 inches
    if (pulseWidth < 118) MAX30105_SetPulseWidth(MAX30105_PULSEWIDTH_69); //Page 26, Gets us 15 bit resolution
    else if (pulseWidth < 215) MAX30105_SetPulseWidth(MAX30105_PULSEWIDTH_118); //16 bit resolution
    else if (pulseWidth < 411) MAX30105_SetPulseWidth(MAX30105_PULSEWIDTH_215); //17 bit resolution
    else if (pulseWidth == 411) MAX30105_SetPulseWidth(MAX30105_PULSEWIDTH_411); //18 bit resolution
    else MAX30105_SetPulseWidth(MAX30105_PULSEWIDTH_69);
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //LED Pulse Amplitude Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //Default is 0x1F which gets us 6.4mA
    //powerLevel = 0x02, 0.4mA - Presence detection of ~4 inch
    //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
    //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
    //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch

    MAX30105_SetPulseAmplitudeRed(powerLevel);
    MAX30105_SetPulseAmplitudeIR(powerLevel);
    MAX30105_SetPulseAmplitudeGreen(powerLevel);
    MAX30105_SetPulseAmplitudeProximity(powerLevel);
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

    //Multi-LED Mode Configuration, Enable the reading of the three LEDs
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    MAX30105_EnableSlot(1, SLOT_RED_LED);
    if (ledMode > 1) MAX30105_EnableSlot(2, SLOT_IR_LED);
    if (ledMode > 2) MAX30105_EnableSlot(3, SLOT_GREEN_LED);
    //enableSlot(1, SLOT_RED_PILOT);
    //enableSlot(2, SLOT_IR_PILOT);
    //enableSlot(3, SLOT_GREEN_PILOT);
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

     MAX30105_ClearFIFO(); //Reset the FIFO before we begin checking the sensor
}


/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Read 8bit register
 **************************************************************************/
uint32_t MAX30105_ReadRegister8(uint8_t *rxBuffer, uint8_t reg)
{
    uint32_t  errorStatus = TRANSFER_ERROR;

    errorStatus = I2C_BUS_MasterSendStart(MAX30105_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    errorStatus = I2C_BUS_MasterWriteByte(reg, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    errorStatus = I2C_BUS_MasterSendReStart(MAX30105_ADDRESS, CY_SCB_I2C_READ_XFER, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
        goto exit;
    }
    
    errorStatus = I2C_BUS_MasterReadByte(CY_SCB_I2C_NAK, rxBuffer, I2C_TIMEOUT);
    /* Check status of transaction */
    if ((errorStatus == CY_SCB_I2C_SUCCESS)           ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
    {
        /* Send Stop condition on the bus */
        if (I2C_BUS_MasterSendStop(I2C_TIMEOUT) == CY_SCB_I2C_SUCCESS)
        {
           errorStatus = TRANSFER_CMPLT;
        }
    }
    
    exit:
        if(TRANSFER_CMPLT != errorStatus)
        {
           errorStatus = TRANSFER_ERROR;
        }
    
    return errorStatus;
}


/***************************************************************************
 * Function: MAX30105_WriteRegister8    
 *
 * Description: Write 8bit register
 **************************************************************************/
uint32_t MAX30105_WriteRegister8(uint8_t txBuffer, uint8_t reg)
{
    uint32_t  errorStatus = TRANSFER_ERROR;

    errorStatus = I2C_BUS_MasterSendStart(MAX30105_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    errorStatus = I2C_BUS_MasterWriteByte(reg, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    errorStatus = I2C_BUS_MasterWriteByte(txBuffer, I2C_TIMEOUT);
    /* Check status of transaction */
    if ((errorStatus == CY_SCB_I2C_SUCCESS)           ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
    {
        /* Send Stop condition on the bus */
        if (I2C_BUS_MasterSendStop(I2C_TIMEOUT) == CY_SCB_I2C_SUCCESS)
        {
           errorStatus = TRANSFER_CMPLT;
        }
    }
    
    exit:
        if(TRANSFER_CMPLT != errorStatus)
        {
           errorStatus = TRANSFER_ERROR;
        }
    
    return errorStatus;
}

/***************************************************************************
 * Function: MAX30105_WriteWithBitMask    
 *
 * Description: Write 8bit register with mask
 **************************************************************************/
uint32_t MAX30105_WriteWithBitMask(uint8_t reg, uint8_t mask, uint8_t val)
{
   uint8_t reg_value = 0U;
   uint32_t  errorStatus = TRANSFER_ERROR;
    
   errorStatus = MAX30105_ReadRegister8(&reg_value, reg);

   if(TRANSFER_CMPLT != errorStatus)
   {
        goto exit;
   }
   reg_value = (reg_value & mask) | val;
   
   errorStatus = MAX30105_WriteRegister8(reg_value, reg);
   
   exit:
      if(TRANSFER_CMPLT != errorStatus)
      {
        errorStatus = TRANSFER_ERROR;
      }

   return errorStatus;
}

/***************************************************************************
 * Function: MAX30105_SoftReset    
 *
 * Description: Send a soft reset command.
 **************************************************************************/
void MAX30105_SoftReset(void)
{
   uint8_t rxBuffer = 0U;
   uint32_t startTime;

   MAX30105_WriteWithBitMask(MAX30105_MODECONFIG, MAX30105_RESET_MASK, MAX30105_RESET);
   
   startTime = MILLIS_GetValue();
   while(MILLIS_GetValue() - startTime < 100)
   {
    MAX30105_ReadRegister8(&rxBuffer, MAX30105_MODECONFIG);
    
    if ((rxBuffer & MAX30105_RESET) == 0) 
    {
        break;
    }
    CyDelayUs(1);
   }

}

/***************************************************************************
 * Function: MAX30105_SetFIFOAverage    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetFIFOAverage(uint8_t numberOfSamples)
{
    MAX30105_WriteWithBitMask(MAX30105_FIFOCONFIG, MAX30105_SAMPLEAVG_MASK, numberOfSamples);
}

/***************************************************************************
 * Function: MAX30105_EnableFIFORollover    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_EnableFIFORollover(void) {
  MAX30105_WriteWithBitMask(MAX30105_FIFOCONFIG, MAX30105_ROLLOVER_MASK, MAX30105_ROLLOVER_ENABLE);
}

/***************************************************************************
 * Function: MAX30105_SetLEDMode    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetLEDMode(uint8_t mode) {
  // Set which LEDs are used for sampling -- Red only, RED+IR only, or custom.
  // See datasheet, page 19
  MAX30105_WriteWithBitMask(MAX30105_MODECONFIG, MAX30105_MODE_MASK, mode);
}

/***************************************************************************
 * Function: MAX30105_SetADCRange    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetADCRange(uint8_t adcRange) {
  // adcRange: one of MAX30105_ADCRANGE_2048, _4096, _8192, _16384
  MAX30105_WriteWithBitMask(MAX30105_PARTICLECONFIG, MAX30105_ADCRANGE_MASK, adcRange);
}

/***************************************************************************
 * Function: MAX30105_SetSampleRate    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetSampleRate(uint8_t sampleRate) {
  // sampleRate: one of MAX30105_SAMPLERATE_50, _100, _200, _400, _800, _1000, _1600, _3200
  MAX30105_WriteWithBitMask(MAX30105_PARTICLECONFIG, MAX30105_SAMPLERATE_MASK, sampleRate);
}

/***************************************************************************
 * Function: MAX30105_SetPulseWidth    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetPulseWidth(uint8_t pulseWidth) {
  // pulseWidth: one of MAX30105_PULSEWIDTH_69, _188, _215, _411
  MAX30105_WriteWithBitMask(MAX30105_PARTICLECONFIG, MAX30105_PULSEWIDTH_MASK, pulseWidth);
}

/***************************************************************************
 * Function: MAX30105_SetPulseAmplitudeRed    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetPulseAmplitudeRed(uint8_t amplitude) {
  MAX30105_WriteRegister8(amplitude, MAX30105_LED1_PULSEAMP);
}

/***************************************************************************
 * Function: MAX30105_SetPulseAmplitudeIR    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetPulseAmplitudeIR(uint8_t amplitude) {
  MAX30105_WriteRegister8(amplitude, MAX30105_LED2_PULSEAMP);
}

/***************************************************************************
 * Function: MAX30105_SetPulseAmplitudeGreen    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetPulseAmplitudeGreen(uint8_t amplitude) {
  MAX30105_WriteRegister8(amplitude, MAX30105_LED3_PULSEAMP);
}

/***************************************************************************
 * Function: MAX30105_SetPulseAmplitudeProximity    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_SetPulseAmplitudeProximity(uint8_t amplitude) {
  MAX30105_WriteRegister8(amplitude, MAX30105_LED_PROX_AMP);
}

/***************************************************************************
 * Function: MAX30105_EnableSlot    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_EnableSlot(uint8_t slotNumber, uint8_t device) {

  switch (slotNumber) {
    case (1):
      MAX30105_WriteWithBitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT1_MASK, device);
      break;
    case (2):
      MAX30105_WriteWithBitMask(MAX30105_MULTILEDCONFIG1, MAX30105_SLOT2_MASK, device << 4);
      break;
    case (3):
      MAX30105_WriteWithBitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT3_MASK, device);
      break;
    case (4):
      MAX30105_WriteWithBitMask(MAX30105_MULTILEDCONFIG2, MAX30105_SLOT4_MASK, device << 4);
      break;
    default:
      //Shouldn't be here!
      break;
  }
}

/***************************************************************************
 * Function: MAX30105_ClearFIFO    
 *
 * Description: check datasheet
 **************************************************************************/
void MAX30105_ClearFIFO(void) {
  MAX30105_WriteRegister8(0, MAX30105_FIFOWRITEPTR);
  MAX30105_WriteRegister8(0, MAX30105_FIFOOVERFLOW);
  MAX30105_WriteRegister8(0, MAX30105_FIFOREADPTR);
}


/***************************************************************************
 * Function: MAX30105_GetWritePointer    
 *
 * Description: check datasheet
 **************************************************************************/
uint8_t MAX30105_GetWritePointer(void) {
  
  uint8_t rxBuffer = 0U;
  MAX30105_ReadRegister8(&rxBuffer, MAX30105_FIFOWRITEPTR);

  return rxBuffer;
}

/***************************************************************************
 * Function: MAX30105_GetReadPointer    
 *
 * Description: check datasheet
 **************************************************************************/
uint8_t MAX30105_GetReadPointer(void) {
    
  uint8_t rxBuffer = 0U;  
  MAX30105_ReadRegister8(&rxBuffer, MAX30105_FIFOREADPTR);

  return rxBuffer;
}

/***************************************************************************
 * Function: MAX30105_Check    
 *
 * Description: Check for new data samples
 **************************************************************************/
uint16_t MAX30105_Check(void)
{
  //Read register FIDO_DATA in (3-byte * number of active LED) chunks
  //Until FIFO_RD_PTR = FIFO_WR_PTR

  uint8_t readPointer = 0U; 
  uint8_t writePointer= 0U;
  int16_t numberOfSamples = 0UL; 
  int16_t bytesLeftToRead = 0UL;
  int16_t toGet = 0UL;
  uint8_t temp[sizeof(uint32_t)] = {0U};
  uint32_t tempLong = 0UL;
  uint32_t  errorStatus = TRANSFER_ERROR;
  cy_en_scb_i2c_command_t cmd = CY_SCB_I2C_ACK;

  readPointer = MAX30105_GetReadPointer();
  writePointer = MAX30105_GetWritePointer();
  
  //Do we have new data?
  if (readPointer != writePointer)
  {
    //Calculate the number of readings we need to get from sensor
    numberOfSamples = writePointer - readPointer;
    if (numberOfSamples < 0) numberOfSamples += 32; //Wrap condition

    //We now have the number of readings, now calc bytes to read
    //For this example we are just doing Red and IR (3 bytes each)
    bytesLeftToRead = numberOfSamples * gActiveLEDs * 3;

    //Get ready to read a burst of data from the FIFO register
    I2C_BUS_MasterSendStart(MAX30105_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    
    
    errorStatus = I2C_BUS_MasterWriteByte(MAX30105_FIFODATA, I2C_TIMEOUT);
    
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    errorStatus = I2C_BUS_MasterSendReStart(MAX30105_ADDRESS, CY_SCB_I2C_READ_XFER, I2C_TIMEOUT);
    
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
        goto exit;
    }
    
    //We may need to read as many as 288 bytes so we read in blocks no larger than I2C_BUFFER_LENGTH
    //I2C_BUFFER_LENGTH changes based on the platform. 
    
    while (bytesLeftToRead > 0)
    {
        
      toGet = bytesLeftToRead;
      if (toGet > I2C_BUFFER_LENGTH)
      {
        //If toGet is 32 this is bad because we read 6 bytes (Red+IR * 3 = 6) at a time
        //32 % 6 = 2 left over. We don't want to request 32 bytes, we want to request 30.
        //32 % 9 (Red+IR+GREEN) = 5 left over. We want to request 27.

        toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (gActiveLEDs * 3)); //Trim toGet to be a multiple of the samples we need to read
      }

      bytesLeftToRead -= toGet;

 
      while (toGet > 0)
      {
        toGet -= gActiveLEDs * 3;
        if((toGet == 0) && (0UL == bytesLeftToRead))
        {
           /* The last byte must be NACKed */
           cmd = CY_SCB_I2C_NAK;
        }
        else
        { 
           cmd = CY_SCB_I2C_ACK; 
        }
        sense.head++; //Advance the head of the storage struct
        sense.head %= STORAGE_SIZE; //Wrap condition

        //Burst read three bytes - RED
        temp[3] = 0;
        errorStatus = I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[2], I2C_TIMEOUT);
        errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[1], I2C_TIMEOUT);
        if(gActiveLEDs == 1)
        {
            errorStatus |= I2C_BUS_MasterReadByte(cmd, &temp[0], I2C_TIMEOUT);
        }
        else
        {
            errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[0], I2C_TIMEOUT);
        }
        
        if(TRANSFER_CMPLT != errorStatus)
        {
            goto exit;
        }
        
              
        //Convert array to long
        memcpy(&tempLong, temp, sizeof(tempLong));
		
		tempLong &= 0x3FFFF; //Zero out all but 18 bits

        sense.red[sense.head] = tempLong; //Store this reading into the sense array

        if (gActiveLEDs > 1)
        {
          //Burst read three more bytes - IR
          temp[3] = 0;
          errorStatus = I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[2], I2C_TIMEOUT);
          errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[1], I2C_TIMEOUT);
          if (gActiveLEDs == 2)
          { 
            errorStatus |= I2C_BUS_MasterReadByte(cmd, &temp[0], I2C_TIMEOUT);
          }
          else
          {
            errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[0], I2C_TIMEOUT);
          }
            
           if(TRANSFER_CMPLT != errorStatus)
           {
                goto exit;
           }
          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));

		  tempLong &= 0x3FFFF; //Zero out all but 18 bits
          
		  sense.IR[sense.head] = tempLong;
        }

        if (gActiveLEDs > 2)
        {
          //Burst read three more bytes - Green
          temp[3] = 0;
          errorStatus = I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[2], I2C_TIMEOUT);
          errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &temp[1], I2C_TIMEOUT);
          errorStatus |= I2C_BUS_MasterReadByte(cmd, &temp[0], I2C_TIMEOUT);

          if(TRANSFER_CMPLT != errorStatus)
          {
                goto exit;
          }
          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));

		  tempLong &= 0x3FFFF; //Zero out all but 18 bits

          sense.green[sense.head] = tempLong;
        }
        
      }

    } //End while (bytesLeftToRead > 0)

    /* Send Stop condition on the bus */
    errorStatus = I2C_BUS_MasterSendStop(I2C_TIMEOUT);
    if (CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
   
  } //End readPtr != writePtr

  exit:
      if(TRANSFER_CMPLT != errorStatus)
      {
        errorStatus = TRANSFER_ERROR;
      }
    
  return numberOfSamples; //Let the world know how much new data we found
}

/***************************************************************************
 * Function: MAX30105_SafeCheck    
 *
 * Description: Pool sensor for new samples
 **************************************************************************/
bool MAX30105_SafeCheck(uint8_t maxTimeToCheck)
{
  uint32_t markTime;
    
  markTime  = MILLIS_GetValue();
  
  while(1)
  {
	if(MILLIS_GetValue() - markTime > maxTimeToCheck) return(false);

	if(MAX30105_Check() == true) //We found new data!
	  return(true);

	CyDelayUs(1);
  }
}

/***************************************************************************
 * Function: MAX30105_GetRed    
 *
 * Description: Report the most recent RED value
 **************************************************************************/
uint32_t MAX30105_GetRed(void)
{
  //Check the sensor for new data for 250ms
  if(MAX30105_SafeCheck(250))
    return (sense.red[sense.head]);
  else
    return(0); //Sensor failed to find new data
}


/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Report the most recent IR value
 **************************************************************************/
uint32_t MAX30105_GetIR(void)
{
  //Check the sensor for new data for 250ms
  if(MAX30105_SafeCheck(250))
    return (sense.IR[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Report the most recent Green value
 **************************************************************************/
uint32_t MAX30105_GetGreen(void)
{
  //Check the sensor for new data for 250ms
  if(MAX30105_SafeCheck(250))
    return (sense.green[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Report the next Red value in the FIFO
 **************************************************************************/
uint32_t MAX30105_GetFIFORed(void)
{
  return (sense.red[sense.tail]);
}

/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Report the next IR value in the FIFO
 **************************************************************************/
uint32_t MAX30105_GetFIFOIR(void)
{
  return (sense.IR[sense.tail]);
}

/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Report the next Green value in the FIFO
 **************************************************************************/
uint32_t MAX30105_GetFIFOGreen(void)
{
  return (sense.green[sense.tail]);
}

/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Tell caller how many samples are available
 **************************************************************************/
uint8_t MAX30105_Available(void)
{
  int8_t numberOfSamples = sense.head - sense.tail;
  if (numberOfSamples < 0) numberOfSamples += STORAGE_SIZE;

  return (numberOfSamples);
}

/***************************************************************************
 * Function: MAX30105_ReadRegister8    
 *
 * Description: Advance the tail
 **************************************************************************/
void MAX30105_NextSample(void)
{
  if(MAX30105_Available()) //Only advance the tail if new data is available
  {
    sense.tail++;
    sense.tail %= STORAGE_SIZE; //Wrap condition
  }
}

/**************************************************** CUSTOM FUNCTIONS **********************************************************/

/***************************************************************************
 * Function: MAX30105_readChannel    
 *
 * Description: read one of the 3 channels
 **************************************************************************/
uint32_t MAX30105_readChannel(uint8_t channel)
{
  uint32_t channelValue;
  if(RED_CHANNEL == channel)
  {
    channelValue = MAX30105_GetRed();
  }
  else if(GREEN_CHANNEL == channel)
  {
    channelValue = MAX30105_GetGreen();
  }
  else if(IR_CHANNEL == channel)
  {
    channelValue = MAX30105_GetIR();
  }
  else
  {
    channelValue = 0;
  }
  
  return channelValue;
}

/***************************************************************************
 * Function: MAX30105_ReadChannels    
 *
 * Description: Read all the data channels
 **************************************************************************/
bool MAX30105_ReadChannels(uint16_t *channelsBuffer, bool readFIFO)
{
  bool bIsTissuePresent = TRUE;
  if(READ_FROM_FIFO_BUFFER != readFIFO)
  {
    *(channelsBuffer + RED_CHANNEL) = (uint16_t) MAX30105_GetRed();
    *(channelsBuffer + GREEN_CHANNEL) = (uint16_t) MAX30105_GetGreen();
    *(channelsBuffer + IR_CHANNEL) = (uint16_t) MAX30105_GetIR();
    if (*(channelsBuffer + IR_CHANNEL) < NO_FINGER_THRESHOLD)
    {
      bIsTissuePresent = FALSE;
    }
  }
  else
  {
    MAX30105_Check();
 
    while(MAX30105_Available())
      {
        *(channelsBuffer + ((RED_CHANNEL * FIFO_NUMBER_OF_SAMPLES) + samplesTaken)) = MAX30105_GetFIFORed();
        *(channelsBuffer + ((GREEN_CHANNEL * FIFO_NUMBER_OF_SAMPLES) + samplesTaken)) = MAX30105_GetFIFOGreen();
        *(channelsBuffer + ((IR_CHANNEL * FIFO_NUMBER_OF_SAMPLES) + samplesTaken)) = MAX30105_GetFIFOIR();
        if (*(channelsBuffer + ((IR_CHANNEL * FIFO_NUMBER_OF_SAMPLES) + samplesTaken)) < NO_FINGER_THRESHOLD)
        {
            bIsTissuePresent = FALSE;
        }
        MAX30105_NextSample();

        samplesTaken++;
        
        if (FIFO_NUMBER_OF_SAMPLES == samplesTaken)
        {
          break;
        }
      }
  }
  return bIsTissuePresent;
}

/* [] END OF FILE */
