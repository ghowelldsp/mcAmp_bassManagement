/***********************************************************************************************************************
 *
 * @file    ma12040p.h
 *
 * @brief   MA12040P Registers
 *  
 * @par
 * @author  G. Howell
 *
 **********************************************************************************************************************/

#ifndef MA12040P_H
#define MA12040P_H

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------- INCLUDES ---------------------------------------------------------------*/

#include <stdint.h>

/*------------------------------------------- MACROS AND DEFINES -----------------------------------------------------*/

/* 
    MA12040P I2C device addresses
*/

#define MA12040P_DEVADDR_1                          ((uint8_t)0x20) // 7-bit device address
#define MA12040P_DEVADDR_2                          ((uint8_t)0x21) // 7-bit device address
#define MA12040P_DEVADDR_3                          ((uint8_t)0x22) // 7-bit device address
#define MA12040P_DEVADDR_4                          ((uint8_t)0x23) // 7-bit device address

/*
 * VLA Control Registers
 */

// audio_proc_attack
#define MA12040P_AUDIO_PROC_ATTACK_ADDR             ((uint8_t)0x35)     // address
#define MA12040P_AUDIO_PROC_ATTACK_BMASK            ((uint8_t)0x03)     // bit mask
#define MA12040P_AUDIO_PROC_ATTACK_BPOS             ((uint8_t)0x04)     // bit position

// audio_proc_release
#define MA12040P_AUDIO_PROC_RELEASE_ADDR            (MA12040P_AUDIO_PROC_ATTACK_ADDR)   // address
#define MA12040P_AUDIO_PROC_RELEASE_BMASK           ((uint8_t)0x03)     // bit mask
#define MA12040P_AUDIO_PROC_RELEASE_BPOS            ((uint8_t)0x06)     // bit position

// audio_proc_enable
#define MA12040P_AUDIO_PROC_ENABLE_ADDR             (MA12040P_AUDIO_PROC_RELEASE_ADDR)  // address
#define MA12040P_AUDIO_PROC_ENABLE_BMASK            ((uint8_t)0x01)     // bit mask
#define MA12040P_AUDIO_PROC_ENABLE_BPOS             ((uint8_t)0x03)     // bit position

// audio_proc_mute
#define MA12040P_AUDIO_PROC_MUTE_ADDR               ((uint8_t)0x036)    // address
#define MA12040P_AUDIO_PROC_MUTE_BMASK              ((uint8_t)0x01)     // bit mask
#define MA12040P_AUDIO_PROC_MUTE_BPOS               ((uint8_t)0x07)     // bit position

// audio_proc_limiterEnable
#define MA12040P_AUDIO_PROC_LIMITER_ENABLE_ADDR     (MA12040P_AUDIO_PROC_MUTE_ADDR)     // address
#define MA12040P_AUDIO_PROC_LIMITER_ENABLE_BMASK    ((uint8_t)0x01)     // bit mask
#define MA12040P_AUDIO_PROC_LIMITER_ENABLE_BPOS     ((uint8_t)0x06)     // bit position

// vol_db_master
#define MA12040P_VOL_DB_MASTER_ADDR                 ((uint8_t)0x40)     // address
#define MA12040P_VOL_DB_MASTER_BMASK                ((uint8_t)0xFF)     // bit mask
#define MA12040P_VOL_DB_MASTER_BPOS                 ((uint8_t)0x00)     // bit position
#define MA12040P_VOL_DB_MASTER_DVAL                 ((uint8_t)0x18)     // default value

// vol_lsb_master
#define MA12040P_VOL_LSB_MASTER_ADDR                ((uint8_t)0x41)     // address
#define MA12040P_VOL_LSB_MASTER_BMASK               ((uint8_t)0x03)     // bit mask
#define MA12040P_VOL_LSB_MASTER_BPOS                ((uint8_t)0x00)     // bit position

// thr_db_ch0
#define MA12040P_THR_DB_CH0_ADDR                    ((uint8_t)0x47)     // address
#define MA12040P_THR_DB_CH0_BMASK                   ((uint8_t)0xFF)     // bit mask
#define MA12040P_THR_DB_CH0_BPOS                    ((uint8_t)0x00)     // bit position

// thr_db_ch1
#define MA12040P_THR_DB_CH1_ADDR                    ((uint8_t)0x48)     // address
#define MA12040P_THR_DB_CH1_BMASK                   ((uint8_t)0xFF)     // bit mask
#define MA12040P_THR_DB_CH1_BPOS                    ((uint8_t)0x00)     // bit position

// thr_db_ch2
#define MA12040P_THR_DB_CH2_ADDR                    ((uint8_t)0x49)     // address
#define MA12040P_THR_DB_CH2_BMASK                   ((uint8_t)0xFF)     // bit mask
#define MA12040P_THR_DB_CH2_BPOS                    ((uint8_t)0x00)     // bit position

// thr_db_ch3
#define MA12040P_THR_DB_CH3_ADDR                    ((uint8_t)0x4A)     // address
#define MA12040P_THR_DB_CH3_BMASK                   ((uint8_t)0xFF)     // bit mask
#define MA12040P_THR_DB_CH3_BPOS                    ((uint8_t)0x00)     // bit position

// thr_lsb_ch0
#define MA12040P_THR_LSB_CH0_ADDR                   ((uint8_t)0x48)     // address
#define MA12040P_THR_LSB_CH0_BMASK                  ((uint8_t)0x03)     // bit mask
#define MA12040P_THR_LSB_CH0_BPOS                   ((uint8_t)0x00)     // bit position

// thr_lsb_ch1
#define MA12040P_THR_LSB_CH1_ADDR                   (MA12040P_THR_LSB_CH0_ADDR)        // address
#define MA12040P_THR_LSB_CH1_BMASK                  ((uint8_t)0x03)     // bit mask
#define MA12040P_THR_LSB_CH1_BPOS                   ((uint8_t)0x02)     // bit position

// thr_lsb_ch2
#define MA12040P_THR_LSB_CH2_ADDR                   (MA12040P_THR_LSB_CH0_ADDR)        // address
#define MA12040P_THR_LSB_CH2_BMASK                  ((uint8_t)0x03)     // bit mask
#define MA12040P_THR_LSB_CH2_BPOS                   ((uint8_t)0x04)     // bit position

// thr_lsb_ch3
#define MA12040P_THR_LSB_CH3_ADDR                   (MA12040P_THR_LSB_CH0_ADDR)        // address
#define MA12040P_THR_LSB_CH3_BMASK                  ((uint8_t)0x03)     // bit mask
#define MA12040P_THR_LSB_CH3_BPOS                   ((uint8_t)0x06)     // bit position

/*
 * VLA Settings
 */

#define MA12040P_N_CHANNELS_PER_AMP               	(4U)

#define MA12040P_LEVEL_DB_MAX                       (24.0F)
#define MA12040P_LEVEL_DB_MIN                       (-144.0F)

// number of fractonal divisions per dB (as dB values accurate to the nearest 0.25dB)
#define MA12040P_N_FRAC_DIVS                        (4U)

typedef enum
{
    MA12040P_AR_TIME_SLOW   = 0x00U,
    MA12040P_AR_TIME_NORMAL = 0x01U,
    MA12040P_AR_TIME_FAST   = 0x02U,
} ma12040p_arTimes_t;

typedef enum
{
    MA12040P_DISABLE        = 0x00U,
    MA12040P_ENABLE         = 0x01U,
} ma12040p_enable_t;

typedef enum
{
    MA12040P_UNMUTE         = 0x00U,
    MA12040P_MUTE           = 0x01U,
} ma12040p_mute_t;

typedef enum
{
    MA12040P_CHANNEL_0      = 0x00U,
    MA12040P_CHANNEL_1      = 0x01U,
    MA12040P_CHANNEL_2      = 0x02U,
    MA12040P_CHANNEL_3      = 0x03U,
} ma12040p_channels_t;

/*------------------------------------------- TYPEDEFS ---------------------------------------------------------------*/
/*------------------------------------------- EXPORTED VARIABLES -----------------------------------------------------*/
/*------------------------------------------- GLOBAL FUNCTION PROTOTYPES ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MA12040P_H */
