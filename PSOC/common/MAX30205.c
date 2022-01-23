/**
 *   @file    MAX30205.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   
 *   @details 
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

#include "MAX30205.h"
#include "I2C_BUS.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/

/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

#define MAX30205_ADDRESS    0x48

#define MAX30205_TEMPERATURE    0x00
#define MAX30205_CONFIGURATION  0x01 
#define MAX30205_THYST          0x02  
#define MAX30205_TOS            0x03 

#define I2C_TIMEOUT         100UL
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
 * Function: MAX30205_Init    
 *
 * Description:  Initialize I2C communication and sensor registers
 **************************************************************************/
void MAX30205_Init(void)
{
    uint32_t initStatus;
    uint32_t dataRate;
  
    /* Configure component. */ 
    initStatus = I2C_BUS_Init(&I2C_BUS_config);
    if(initStatus!=CY_SCB_I2C_SUCCESS)
    {
        HandleError();
    }
    
    dataRate = I2C_BUS_SetDataRate(I2C_BUS_DATA_RATE_HZ, I2C_BUS_CLK_FREQ_HZ);
    
    /* Check whether data rate set is not greather then required reate. */
    if(dataRate > I2C_BUS_DATA_RATE_HZ)
    {
        HandleError();
    }
    
    Cy_SCB_I2C_Enable(I2C_BUS_HW);
    
    initStatus = MAX30205_WriteByte(0x00, MAX30205_CONFIGURATION);
    initStatus |= MAX30205_WriteByte(0x00, MAX30205_THYST);
    initStatus |= MAX30205_WriteByte(0x00, MAX30205_TOS);
    
    if(initStatus == TRANSFER_ERROR)
    {
        HandleError();
    }
}

/***************************************************************************
 * Function: MAX30205_WriteByte  
 *
 * Description:  Write one byte over I2C
 **************************************************************************/
uint32_t MAX30205_WriteByte(uint8_t value, uint8_t reg)
{
    uint32_t  errorStatus = TRANSFER_ERROR;

    errorStatus = I2C_BUS_MasterSendStart(MAX30205_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    
    errorStatus |= I2C_BUS_MasterWriteByte(reg, I2C_TIMEOUT);
    
    errorStatus |= I2C_BUS_MasterWriteByte(value, I2C_TIMEOUT);
    
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
    
    return errorStatus;
}

/***************************************************************************
 * Function: MAX30205_ReadBytes 
 *
 * Description:  Read bytes from temperature sensor
 **************************************************************************/
uint32_t MAX30205_ReadBytes(uint8_t *buffer, uint8_t bytes_number, uint8_t reg)
{
    uint32_t  errorStatus = TRANSFER_ERROR;
    uint8_t i = 0U;
    uint8_t byte = 0U;
    
    errorStatus = I2C_BUS_MasterSendStart(MAX30205_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    errorStatus |= I2C_BUS_MasterWriteByte(reg, I2C_TIMEOUT);
    errorStatus |= I2C_BUS_MasterSendReStart(MAX30205_ADDRESS, CY_SCB_I2C_READ_XFER, I2C_TIMEOUT);
    
    if(bytes_number == 1U)
    {
         errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_NAK, &byte, I2C_TIMEOUT);
         *buffer = byte;
    }
    else
    {
        while((bytes_number != 0) && (errorStatus != TRANSFER_ERROR))
        {
            errorStatus |= I2C_BUS_MasterReadByte(CY_SCB_I2C_ACK, &byte, I2C_TIMEOUT);
            bytes_number = bytes_number - 1;
            *(buffer + bytes_number) = byte;
        }
    }
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
    
    return errorStatus;
}

/***************************************************************************
 * Function: MAX30205_GetTemperature 
 *
 * Description:  Read temperature register and convert to temperature
 **************************************************************************/
float MAX30205_GetTemperature(void)
{
    uint32_t errorStatus = TRANSFER_ERROR;
    
    uint8_t raw_bytes[2] = {0};
    int16_t raw_data = 0UL;
    
    errorStatus = MAX30205_ReadBytes(&raw_bytes[0], 2, MAX30205_TEMPERATURE);
    if(errorStatus == TRANSFER_ERROR)
    {
        HandleError();
    }
   
    raw_data = raw_bytes[0] << 8 | raw_bytes[1];  
   
    return raw_data * 0.00390625;
}