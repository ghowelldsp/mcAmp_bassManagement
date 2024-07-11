/***********************************************************************************************************************
 *
 * @file    ma12040p_driver.c
 *
 * @brief   MA12040P Driver
 *  
 * @par
 * @author  G. Howell
 *
 **********************************************************************************************************************/

/*------------------------------------------- INCLUDES ---------------------------------------------------------------*/

#include <math.h>

#include "ma12040p_driver.h"

/*------------------------------------------- EXTERN VARIABLES -------------------------------------------------------*/
/*------------------------------------------- PRIVATE MACROS AND DEFINES ---------------------------------------------*/
/*------------------------------------------- PRIVATE TYPEDEFS -------------------------------------------------------*/
/*------------------------------------------- STATIC VARIABLES -------------------------------------------------------*/
/*------------------------------------------- GLOBAL VARIABLES -------------------------------------------------------*/
/*------------------------------------------- STATIC FUNCTION PROTOTYPES ---------------------------------------------*/

static void calcLevel(
    float levelDb,
    uint8_t *pRegValInt,
    uint8_t *pRegValFrac);

/*------------------------------------------- STATIC FUNCTIONS -------------------------------------------------------*/

// calculate register level values from the dBFS input value
static void calcLevel(
    float levelDb,
    uint8_t *pRegValInt,
    uint8_t *pRegValFrac)
{
    uint8_t levelInt;
    float levelFrac;

    // check limits
    levelDb = (levelDb > MA12040P_LEVEL_DB_MAX) ? MA12040P_LEVEL_DB_MAX : \
              (levelDb < MA12040P_LEVEL_DB_MIN) ? MA12040P_LEVEL_DB_MIN : levelDb;

    // round volume to nearest 0.25dB level (NOTE: using floor as the Sharc standard library does not include round)
    levelDb = floor(levelDb * MA12040P_N_FRAC_DIVS) / MA12040P_N_FRAC_DIVS;

    // get integer and fractional parts
    levelInt = (uint8_t)levelDb;
    levelFrac = (float)levelInt - levelDb;

    // calc integer register value
    *pRegValInt = (uint8_t)MA12040P_LEVEL_DB_MAX - levelInt;

    // calculate fractional register value
    if (levelFrac < 0.0F)
    {
        levelFrac += 1.0F;
    }
    *pRegValFrac = (uint8_t)(levelFrac * MA12040P_N_FRAC_DIVS);
}

/*------------------------------------------- GLOBAL FUNCTIONS -------------------------------------------------------*/

MA12040P_RESULT ma12040p_setMasterMute(
    BM_TWI *twi,
    ma12040p_mute_t mute)
{
    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    return ma12040p_writeReg(twi, MA12040P_AUDIO_PROC_MUTE_ADDR, mute, MA12040P_AUDIO_PROC_MUTE_BMASK,
                             MA12040P_AUDIO_PROC_MUTE_BPOS);
}

MA12040P_RESULT ma12040p_setMasterVolume(
    BM_TWI *twi,
    float volumeDb)
{
    uint8_t regValInt, regValFrac;
    MA12040P_RESULT ret = MA12040P_SUCCESS;

    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    // calculate levels
    calcLevel(volumeDb, &regValInt, &regValFrac);

    // write integer value to device
    ret |= ma12040p_writeReg(twi, MA12040P_VOL_DB_MASTER_ADDR, regValInt, MA12040P_VOL_DB_MASTER_BMASK,
                             MA12040P_VOL_DB_MASTER_BPOS);

    // write fractional value to device
    ret |= ma12040p_writeReg(twi, MA12040P_VOL_LSB_MASTER_ADDR, regValFrac, MA12040P_VOL_LSB_MASTER_BMASK, 
                             MA12040P_VOL_LSB_MASTER_BPOS);

    return ret;
}

MA12040P_RESULT ma12040p_setLimiterAttack(
    BM_TWI *twi,
    ma12040p_arTimes_t arTime)
{
    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    return ma12040p_writeReg(twi, MA12040P_AUDIO_PROC_ATTACK_ADDR, arTime, MA12040P_AUDIO_PROC_ATTACK_BMASK,
                             MA12040P_AUDIO_PROC_ATTACK_BPOS);
}

MA12040P_RESULT ma12040p_setLimiterRelease(
    BM_TWI *twi,
    ma12040p_arTimes_t arTime)
{
    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    return ma12040p_writeReg(twi, MA12040P_AUDIO_PROC_RELEASE_ADDR, arTime, MA12040P_AUDIO_PROC_RELEASE_BMASK,
                             MA12040P_AUDIO_PROC_RELEASE_BPOS);
}

MA12040P_RESULT ma12040p_setLimiterThreshold(
    BM_TWI *twi,
    ma12040p_channels_t channel,
    float thresholdDb)
{
    uint8_t regValInt, regValFrac;
    MA12040P_RESULT ret = MA12040P_SUCCESS;

    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    // calculate levels
    calcLevel(thresholdDb, &regValInt, &regValFrac);

    // write integer value to device
    ret |= ma12040p_writeReg(twi, MA12040P_THR_DB_CH0_ADDR + channel, regValInt, MA12040P_THR_DB_CH0_BMASK,
                             MA12040P_THR_DB_CH0_BPOS);

    // write fractional value to device
    ret |= ma12040p_writeReg(twi, MA12040P_THR_LSB_CH0_ADDR, regValFrac, MA12040P_THR_LSB_CH0_BMASK, 
                             MA12040P_THR_LSB_CH0_BPOS + (channel * 0x2U));

    return ret;
}

MA12040P_RESULT ma12040p_setLimiterEnable(
    BM_TWI *twi,
    ma12040p_enable_t enDi)
{
    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    return ma12040p_writeReg(twi, MA12040P_AUDIO_PROC_LIMITER_ENABLE_ADDR, enDi, 
                             MA12040P_AUDIO_PROC_LIMITER_ENABLE_BMASK, MA12040P_AUDIO_PROC_LIMITER_ENABLE_BPOS);
}

MA12040P_RESULT ma12040p_setVlaEnable(
    BM_TWI *twi,
    ma12040p_enable_t enDi)
{
    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    return ma12040p_writeReg(twi, MA12040P_AUDIO_PROC_ENABLE_ADDR, enDi, MA12040P_AUDIO_PROC_ENABLE_BMASK,
                             MA12040P_AUDIO_PROC_ENABLE_BPOS);
}

MA12040P_RESULT ma12040p_writeReg(
    BM_TWI *twi,
    uint8_t regAddr,
    uint8_t regValue,
    uint8_t regBitMask,
    uint8_t regBitPos)
{
    uint8_t rdData;
    uint8_t wrData[2] = {regAddr, 0U};
    uint8_t *pWrVal = &wrData[1];

    // check NULL pointers
    if (NULL == twi)
    {
        return MA12040P_ERROR;
    }

    // read the register addres
    ma12040p_readReg(twi, regAddr, pWrVal);

    // clear the bits
    *pWrVal &= ~(regBitMask << regBitPos);

    // write the new data
    *pWrVal |= regValue << regBitPos;

    // write param data
    if (twi_write_block_r(twi, wrData, 2U, false) != TWI_SIMPLE_SUCCESS)
    {
        return MA12040P_ERROR;
    }

    // read back the value to check it has been written correctly
    if (ma12040p_readReg(twi, regAddr, &rdData) != MA12040P_SUCCESS)
    {
        return MA12040P_ERROR;
    }
    else
    {
        if (rdData != *pWrVal)
        {
            return MA12040P_ERROR;
        }
    }

    return MA12040P_SUCCESS;
}

MA12040P_RESULT ma12040p_readReg(
    BM_TWI *twi,
    uint8_t regAddr,
    uint8_t *regValue)
{
    // check NULL pointers
    if ((NULL == twi) || (NULL == regValue))
    {
        return MA12040P_ERROR;
    }

    // write register address to the device
    if (twi_write_block_r(twi, &regAddr, 1U, false) != TWI_SIMPLE_SUCCESS)
    {
        return MA12040P_ERROR;
    }

    // read the register address data sent back from the device
    if (twi_read(twi, regValue) != TWI_SIMPLE_SUCCESS)
    {
        return MA12040P_ERROR;
    }

    return MA12040P_SUCCESS;
}
