/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: WaveShare Team / Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: PCA9685.h
*
* Description:: Implementation of modified version of
*               WaveShare's motor demo libaries, adapted to
*               the needs of our project.
*
**************************************************************/

#include "PCA9685.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

//GPIO config
#define SUBADR1             0x02
#define SUBADR2             0x03
#define SUBADR3             0x04
#define MODE1               0x00
#define PRESCALE            0xFE
#define LED0_ON_L           0x06
#define LED0_ON_H           0x07
#define LED0_OFF_L          0x08
#define LED0_OFF_H          0x09
#define ALLLED_ON_L         0xFA
#define ALLLED_ON_H         0xFB
#define ALLLED_OFF_L        0xFC
#define ALLLED_OFF_H        0xFD

#define PCA_CHANNEL_0       0
#define PCA_CHANNEL_1       1
#define PCA_CHANNEL_2       2
#define PCA_CHANNEL_3       3
#define PCA_CHANNEL_4       4
#define PCA_CHANNEL_5       5
#define PCA_CHANNEL_6       6
#define PCA_CHANNEL_7       7
#define PCA_CHANNEL_8       8
#define PCA_CHANNEL_9       9
#define PCA_CHANNEL_10      10
#define PCA_CHANNEL_11      11
#define PCA_CHANNEL_12      12
#define PCA_CHANNEL_13      13
#define PCA_CHANNEL_14      14
#define PCA_CHANNEL_15      15

// ============================================================================================= //
// Library Functions
// ============================================================================================= //

/**
 * Write bytes in PCA9685
 * 
 * @param reg: register.
 * @param value: value.
 *
 * Example:
 * PCA9685_WriteByte(0x00, 0xff);
 */
static void PCA9685_WriteByte(UBYTE reg, UBYTE value)
{
    I2C_Write_Byte(reg, value);
}

/**
 * read byte in PCA9685.
 *
 * @param reg: register.
 *
 * Example:
 * UBYTE buf = PCA9685_ReadByte(0x00);
 */
static UBYTE PCA9685_ReadByte(UBYTE reg)
{
    return I2C_Read_Byte(reg);
}

/**
 * Set the PWM output.
 *
 * @param channel: 16 output channels.  //(0 ~ 15)
 * @param on: 12-bit register will hold avalue for the ON time.  //(0 ~ 4095)
 * @param off: 12-bit register will hold the value for the OFF time.  //(0 ~ 4095)
 *
 * @For more information, please see page 15 - page 19 of the datasheet.
 * Example:
 * PCA9685_SetPWM(0, 0, 4095);
 */
static void PCA9685_SetPWM(UBYTE channel, UWORD on, UWORD off)
{
    PCA9685_WriteByte(LED0_ON_L + 4*channel, on & 0xFF);
    PCA9685_WriteByte(LED0_ON_H + 4*channel, on >> 8);
    PCA9685_WriteByte(LED0_OFF_L + 4*channel, off & 0xFF);
    PCA9685_WriteByte(LED0_OFF_H + 4*channel, off >> 8);
}

/**
 * PCA9685 Initialize.
 * For the PCA9685, the device address can be controlled by setting A0-A5.
 * On our driver board, control is set by setting A0-A4, and A5 is grounded.
 * 
 * @param addr: PCA9685 address.  //0x40 ~ 0x5f
 *
 * Example:
 * PCA9685_Init(0x40);
 */
void PCA9685_Init(char addr)
{
    DEV_I2C_Init(addr);
    I2C_Write_Byte(MODE1, 0x00);
}

/**
 * Set the frequency (PWM_PRESCALE) and restart.
 * 
 * For the PCA9685, Each channel output has its own 12-bit 
 * resolution (4096 steps) fixed frequency individual PWM 
 * controller that operates at a programmable frequency 
 * from a typical of 40 Hz to 1000 Hz with a duty cycle 
 * that is adjustable from 0 % to 100 % 
 * 
 * @param freq: Output frequency.  //40 ~ 1000
 *
 * Example:
 * PCA9685_SetPWMFreq(50);
 */
void PCA9685_SetPWMFreq(UWORD freq)
{
    freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
    double prescaleval = 25000000.0;
    prescaleval /= 4096.0;
    prescaleval /= freq;
    prescaleval -= 1;

    UBYTE prescale = floor(prescaleval + 0.5);

    UBYTE oldmode = PCA9685_ReadByte(MODE1);
    UBYTE newmode = (oldmode & 0x7F) | 0x10; // sleep

    PCA9685_WriteByte(MODE1, newmode); // go to sleep
    PCA9685_WriteByte(PRESCALE, prescale); // set the prescaler
    PCA9685_WriteByte(MODE1, oldmode);
    DEV_Delay_ms(5);
    PCA9685_WriteByte(MODE1, oldmode | 0x80);  //  This sets the MODE1 register to turn on auto increment.
}

/**
 * Set channel output the PWM duty cycle.
 * 
 * @param channel: 16 output channels.  //(0 ~ 15)
 * @param pulse: duty cycle.  //(0 ~ 100  == 0% ~ 100%)
 *
 * Example:
 * PCA9685_SetPwmDutyCycle(1, 100);
 */
void PCA9685_SetPwmDutyCycle(UBYTE channel, UWORD pulse)
{
    PCA9685_SetPWM(channel, 0, pulse * (4096 / 100) - 1);
}

/**
 * Set channel output level.
 * 
 * @param channel: 16 output channels.  //(0 ~ 15)
 * @param value: output level, 0 low level, 1 high level.  //0 or 1
 *
 * Example:
 * PCA9685_SetLevel(3, 1);
 */
void PCA9685_SetLevel(UBYTE channel, UWORD value)
{
    if (value == 1)
        PCA9685_SetPWM(channel, 0, 4095);
    else
        PCA9685_SetPWM(channel, 0, 0);
}


// ============================================================================================= //
// End of File
// ============================================================================================= //