/**
 *   @file    AD5273BRJZ1.c
 *   @author  Vlad-Eusebiu Baciu
 *   @brief   Library for the AD5273BRJZ1 digital pot.
 *   @details Initialize the digital pot, set resistance, read status. 
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
#include "AD5273.h"
#include "I2C_BUS.h"

/*==================================================================================================
 *                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *  ==============================================================================================*/


/*==================================================================================================
 *                                       LOCAL MACROS
 *  ==============================================================================================*/

/* @brief Address pin connected to ground */
#define AD5273_AD_PIN          (0x00)
/* @brief I2C base address */
#define AD5273_BASE_ADDRESS    (0x2CU) 
/* @brief I2C device address. 010110 MSB*/
#define AD5273_ADDRESS         (AD5273_BASE_ADDRESS | AD5273_AD_PIN)  

/* @brief I2C HW peripheral used */
#define I2C_AD5273_HW                       I2C_BUS_HW
/* @brief I2C timeout value */
#define I2C_TIMEOUT                         (100UL)
/* @brief Instruction byte according to datasheet */
#define I2C_AD5273_INSTRUCTION_BYTE         (0x00)

/* @brief AD5273 status masks. E1|E0|D5|...|D0| */
#define AD5273_E0_STATUS_MASK   (1 << 6)
#define AD5273_E1_STATUS_MASK   (1 << 7)
#define AD5273_RDAC_MASK        ~(0xC0)

/* @brief Minimum wiper value in ohms (according to datasheet) */
#define R_W_VALUE               (60U)
/* @brief Maximum AB value. AD5273 is a 10000 ohm digital potentiometer */
#define R_AB_VALUE              (10000UL)

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
 * Function: AD5273_Init    
 *
 * Description: Init I2C peripheral and set the desired data rate.
 *              If the I2C peripheral is already initialized the initialization
*               will be skipped
 **************************************************************************/
void AD5273_Init(void)
{
    cy_en_scb_i2c_status_t initStatus;
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
    
    Cy_SCB_I2C_Enable(I2C_AD5273_HW);
}

/***************************************************************************
 * Function: AD5273_ReadData    
 *
 * Description: Read rdac value and error bits
 **************************************************************************/
uint8_t AD5273_ReadData(void)
{
    uint32_t  errorStatus = TRANSFER_ERROR;
    uint8_t rdacVal = 0U;
    
    /* Send I2C start condition */
    errorStatus = I2C_BUS_MasterSendStart(AD5273_ADDRESS, CY_SCB_I2C_READ_XFER, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    /* Read data byte from the digital potentiometer */
    errorStatus = I2C_BUS_MasterReadByte(CY_SCB_I2C_NAK, &rdacVal, I2C_TIMEOUT);
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
    
    return rdacVal;
}

/***************************************************************************
 * Function: AD5273_WriteData    
 *
 * Description: Set resistance value of the digital potentiometer
 **************************************************************************/
uint8_t AD5273_WriteData(uint8_t val)
{
    uint32_t  errorStatus = TRANSFER_ERROR;
    
    /* RDAC has 64 contact points, the value can not exceed this value */
    if(val > 64) goto exit;
    
    /* Send I2C start condition */
    errorStatus = I2C_BUS_MasterSendStart(AD5273_ADDRESS, CY_SCB_I2C_WRITE_XFER, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    /* Send frame 2 instruction byte */
    errorStatus = I2C_BUS_MasterWriteByte(I2C_AD5273_INSTRUCTION_BYTE, I2C_TIMEOUT);
    if(CY_SCB_I2C_SUCCESS != errorStatus)
    {
       goto exit;
    }
    
    /* Write RDAC value */
    errorStatus = I2C_BUS_MasterWriteByte(val & AD5273_RDAC_MASK, I2C_TIMEOUT);
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
 * Function: AD5273_SetWiper    
 *
 * Description: Converts the desired resistance value to RDAC value.
 **************************************************************************/
void AD5273_SetWiper(uint32_t resVal)
{
    uint8_t result = 0U;
    uint8_t rdacVal = 0U;
    
    if(0x00 != resVal)
    {
      /* R_WB(D) = (D * R_AB)/63 + R_W 
        rdacVal is D in this equation*/
      rdacVal = (uint8_t)ceil(((resVal - R_W_VALUE)/ (float)R_AB_VALUE) * 63.0);
    }
    
    /* Write RDAC value */
    AD5273_WriteData(rdacVal);
    
    /* Check device status */
    result = AD5273_ReadData();
    if(((AD5273_E1_STATUS_MASK | AD5273_E0_STATUS_MASK) & result) != 0x00)
    {
        HandleError();
    }
}


/* [] END OF FILE */
