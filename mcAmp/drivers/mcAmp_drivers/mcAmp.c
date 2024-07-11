/***********************************************************************************************************************
 * 
 * @file    mcAmp.c
 *
 * @brief   Multichannel Amplifier API's
 *  
 * @par
 * @author  G. Howell
 * 
 **********************************************************************************************************************/

/*------------------------------------------- INCLUDES ---------------------------------------------------------------*/

#include "mcamp.h"

#include "ma12040p_driver.h"

#include <sruSC589.h>
#include "drivers/bm_twi_driver/bm_twi.h"
#include "drivers/bm_event_logging_driver/bm_event_logging.h"

/*------------------------------------------- EXTERN VARIABLES -------------------------------------------------------*/
/*------------------------------------------- PRIVATE MACROS AND DEFINES ---------------------------------------------*/

#define MCAMP_N_BUSSES                          (2U)
#define MCAMP_N_AMPS_ON_BUS_MAX                 (4U)

#define MCAMP_I2C_MUX_DEV_ADDR                  ((uint8_t)0x70)

/*------------------------------------------- PRIVATE TYPEDEFS -------------------------------------------------------*/

typedef struct mcAmp_params_t
{
    float volumeDb;                             /**< Master volume level [dBFS]. Should be a value between -144 and +24 
                                                     dBFS. */
    ma12040p_arTimes_t limiterAttackTime;       /**< Limiter attack time. */
    ma12040p_arTimes_t limiterReleaseTime;      /**< Limiter release time. */
    float limiterThresholdDb;                   /**< Limiter threshold [dBFS]. Should be a value between -144 and +24 
                                                     dBFS.*/
} mcAmp_params_t;

typedef struct mcAmp_ampsConn_t
{
    uint8_t i2cBusNo;
    uint8_t devAddr;
} mcAmp_ampsConn_t;

typedef struct mcAmp_amps_t
{
    uint8_t nDevices;
    mcAmp_ampsConn_t ampsConn[MCAMP_N_AMPS_MAX];
} mcAmp_amps_t;

/*------------------------------------------- STATIC VARIABLES -------------------------------------------------------*/
/*------------------------------------------- GLOBAL VARIABLES -------------------------------------------------------*/

BM_TWI mcAmpTwiH;

mcAmp_params_t mcAmpConfig = {
    .volumeDb = -24.0F,
    .limiterAttackTime = MA12040P_AR_TIME_FAST,
    .limiterReleaseTime = MA12040P_AR_TIME_NORMAL,
    .limiterThresholdDb = -24.0F
};

mcAmp_amps_t amps = {0};

/*------------------------------------------- STATIC FUNCTION PROTOTYPES ---------------------------------------------*/

static void i2cInit(
    void);

static void setI2cMuxBus(
    uint8_t busNum);
    
static void detectAmps(
    void);

static void setInitValues(
    void);

/*------------------------------------------- STATIC FUNCTIONS -------------------------------------------------------*/

/* i2c initialisation */
static void i2cInit(void)
{
    char msg[128] = {0};

    // initialise i2c
    if (twi_initialize(&mcAmpTwiH, MCAMP_I2C_MUX_DEV_ADDR, TWI_TYPICAL_SCLK0_FREQ, TWI2) != TWI_SIMPLE_SUCCESS)
    {
        log_event(EVENT_WARN, "McAmp: error initialising i2c");
    }

    // set baud rate
    twi_set_clock(&mcAmpTwiH, MCAMP_I2C_SPEED);

    log_event(EVENT_INFO, "McAmp: I2C initialised");

    sprintf(msg, "McAmp: I2C Config, Bus: %d, Addr: 0x%.2x, Speed: %.3f KHz",
            TWI2, MCAMP_I2C_MUX_DEV_ADDR, (double)MCAMP_I2C_SPEED / 1000.0);
    log_event(EVENT_INFO, msg);
}

/* set the i2c mux bus number */
static void setI2cMuxBus(
    uint8_t busNum)
{
    uint8_t busNumRegVal, regVal;
    
    busNumRegVal = 1U << busNum;

    // write bus number
    if (twi_write_block_r(&mcAmpTwiH, &busNumRegVal, 1, false) != TWI_SIMPLE_SUCCESS)
    {
        log_event(EVENT_WARN, "McAmp: error writing bus number to i2c mux");
    }

    // readback bus number of check against sent value
    if (twi_read(&mcAmpTwiH, &regVal) != TWI_SIMPLE_SUCCESS)
    {
        log_event(EVENT_WARN, "McAmp: error reading the i2c mux bus");
    }
    else
    {
        if (regVal != busNumRegVal)
        {
            log_event(EVENT_WARN, "McAmp: error setting the i2c mux bus");
        }
    }
}

/* detect all connected amps */
static void detectAmps(void)
{
    uint8_t i, j, regVal;
    char msg[128] = {0U};

    for (i = 0U; i < MCAMP_N_BUSSES; i++)
    {
        // set i2c mux bus number
        setI2cMuxBus(i);

        // go through each possible ma12040p device address and read the master volume register, if the read returns
        // the correct default value (0x18) then the device is connected
        for (j = 0U; j < MCAMP_N_AMPS_ON_BUS_MAX; j++)
        {
            // set i2c address to the current amp
            twi_set_temporary_address(&mcAmpTwiH, MA12040P_DEVADDR_1 + j);

            // read the value of the master volume register
            if (ma12040p_readReg(&mcAmpTwiH, MA12040P_VOL_DB_MASTER_ADDR, &regVal) != MA12040P_SUCCESS)
            {
                log_event(EVENT_WARN, "McAmp: error reading register");
            }

            if (regVal == MA12040P_VOL_DB_MASTER_DVAL)
            {
                // add amp to config
                amps.ampsConn[amps.nDevices].i2cBusNo = i;
                amps.ampsConn[amps.nDevices].devAddr = MA12040P_DEVADDR_1 + j;
                amps.nDevices++;

                sprintf(msg, "McAmp: MA12040P amp found. I2C Mux Bus: %d; Device Address: 0x%.2x", i, MA12040P_DEVADDR_1 + j);
                log_event(EVENT_INFO, msg);
            }
            else
            {
                sprintf(msg, "McAmp: MA12040P amp not found. I2C Mux Bus: %d; Device Address: 0x%.2x", i, MA12040P_DEVADDR_1 + j);
                log_event(EVENT_WARN, msg);
            }
        }

        // restore device address back to the i2c mux
        twi_restore_address(&mcAmpTwiH);
    }

    // reset the i2c mux bus to default (0)
    setI2cMuxBus(0U);
}

static void setInitValues(void)
{
    char msg[128];
    uint8_t i, j;
    MA12040P_RESULT ret = MA12040P_SUCCESS;

    for (i = 0U; i < amps.nDevices; i++)
    {
        setI2cMuxBus(amps.ampsConn[i].i2cBusNo);
        twi_set_temporary_address(&mcAmpTwiH, amps.ampsConn[i].devAddr);

        sprintf(msg, "McAmp: Bus %d, DevAddr 0x%x - initialising params", amps.ampsConn[i].i2cBusNo,
                amps.ampsConn[i].devAddr);
        log_event(EVENT_INFO, msg);

        // master volume
        if (MA12040P_SUCCESS == ma12040p_setMasterVolume(&mcAmpTwiH, mcAmpConfig.volumeDb))
        {
            sprintf(msg, "\t master volume level: %.2f dBFS", mcAmpConfig.volumeDb);
            log_event(EVENT_INFO, msg);
        }
        else
        {
            sprintf(msg, "\t error setting up master volume level at %.2f dBFS", mcAmpConfig.volumeDb);
            log_event(EVENT_WARN, msg);
        }

        // limiter attack time
        if (MA12040P_SUCCESS == ma12040p_setLimiterAttack(&mcAmpTwiH, mcAmpConfig.limiterAttackTime))
        {
            if (MA12040P_AR_TIME_SLOW == mcAmpConfig.limiterAttackTime)
            {
                log_event(EVENT_INFO, "\t limiter attack time: SLOW");
            }
            else if (MA12040P_AR_TIME_NORMAL == mcAmpConfig.limiterAttackTime)
            {
                log_event(EVENT_INFO, "\t limiter attack time: NORMAL");
            }
            else if (MA12040P_AR_TIME_FAST == mcAmpConfig.limiterAttackTime)
            {
                log_event(EVENT_INFO, "\t limiter attack time: FAST");
            }
        }
        else
        {
            log_event(EVENT_WARN, "\t error setting limiter attack time");
        }

        // limiter release time
        if (MA12040P_SUCCESS == ma12040p_setLimiterRelease(&mcAmpTwiH, mcAmpConfig.limiterReleaseTime))
        {
            if (MA12040P_AR_TIME_SLOW == mcAmpConfig.limiterReleaseTime)
            {
                log_event(EVENT_INFO, "\t limiter release time: SLOW");
            }
            else if (MA12040P_AR_TIME_NORMAL == mcAmpConfig.limiterReleaseTime)
            {
                log_event(EVENT_INFO, "\t limiter release time: NORMAL");
            }
            else if (MA12040P_AR_TIME_FAST == mcAmpConfig.limiterReleaseTime)
            {
                log_event(EVENT_INFO, "\t limiter release time: FAST");
            }
        }
        else
        {
            log_event(EVENT_WARN, "\t error setting limiter release time");
        }

        // limiter thresholds for each channel
        ret = MA12040P_SUCCESS;
        for (j = 0U; j < MA12040P_N_CHANNELS_PER_AMP; j++)
        {
            ret |= ma12040p_setLimiterThreshold(&mcAmpTwiH, (ma12040p_channels_t)j, mcAmpConfig.limiterThresholdDb);

            if (MA12040P_SUCCESS == ret)
            {
                sprintf(msg, "\t limiter threshold level: %.2f dBFS", mcAmpConfig.limiterThresholdDb);
                log_event(EVENT_INFO, msg);
            }
            else
            {
                sprintf(msg, "\t error setting limiter threshold level at %.2f dBFS", mcAmpConfig.limiterThresholdDb);
                log_event(EVENT_WARN, msg);
            }
        }

        // enable limiters
        if (MA12040P_SUCCESS == ma12040p_setLimiterEnable(&mcAmpTwiH, MA12040P_ENABLE))
        {
            log_event(EVENT_INFO, "\t enabled limiter");
        }
        else
        {
            log_event(EVENT_WARN, "\t error enabling limiter");
        }

        twi_restore_address(&mcAmpTwiH);
    }

    // enable the VLA's across all channels
    ret = MA12040P_SUCCESS;
    for (i = 0U; i < amps.nDevices; i++)
    {
        setI2cMuxBus(amps.ampsConn[i].i2cBusNo);
        twi_set_temporary_address(&mcAmpTwiH, amps.ampsConn[i].devAddr);

        if (MA12040P_SUCCESS != ma12040p_setVlaEnable(&mcAmpTwiH, MA12040P_ENABLE))
        {
            ret = MA12040P_ERROR;
            sprintf(msg, "McAmp: error enabling VLA's on bus %d, devAddr 0x%x", amps.ampsConn[i].i2cBusNo,
                    amps.ampsConn[i].devAddr);
            log_event(EVENT_WARN, msg);
        }

        twi_restore_address(&mcAmpTwiH);
    }
    if (MA12040P_SUCCESS == ret)
    {
        log_event(EVENT_INFO, "McAmp: VLA's enabled on all devices");
    }

    // reset the i2c mux bus to default
    setI2cMuxBus(0U);
}

/*------------------------------------------- GLOBAL FUNCTIONS -------------------------------------------------------*/

/* initialise */
void mcAmp_init(
    void)
{
    // i2c initialisation
    i2cInit();

    // check any amps connected
    detectAmps();

    if (amps.nDevices)
    {
        // set initial value
        setInitValues();
    }
    else
    {
        log_event(EVENT_WARN, "McAmp: No amps connected. McAmp not setup!");
    }
}

/* enable */
void mcAmp_enable(
    void)
{
    SRU2(LOW, DAI1_PB14_I);

    log_event(EVENT_INFO, "McAmp: enabled amps");
}
