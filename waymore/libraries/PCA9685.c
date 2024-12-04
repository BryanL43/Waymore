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

static void PCA9685_WriteByte(UBYTE reg, UBYTE value)
{
    I2C_Write_Byte(reg, value);
}

static UBYTE PCA9685_ReadByte(UBYTE reg)
{
    return I2C_Read_Byte(reg);
}

static void PCA9685_SetPWM(UBYTE channel, UWORD on, UWORD off)
{
    PCA9685_WriteByte(LED0_ON_L + 4*channel, on & 0xFF);
    PCA9685_WriteByte(LED0_ON_H + 4*channel, on >> 8);
    PCA9685_WriteByte(LED0_OFF_L + 4*channel, off & 0xFF);
    PCA9685_WriteByte(LED0_OFF_H + 4*channel, off >> 8);
}

void PCA9685_Init(char addr)
{
    DEV_I2C_Init(addr);
    I2C_Write_Byte(MODE1, 0x00);
}

void PCA9685_SetPWMFreq(UWORD freq)
{
    // Adjust frequency slightly to account for hardware limitations
    freq *= 0.9;

    // Calculate prescale value based on the frequency provided
    double prescaleval = 25000000.0;  // 25MHz clock frequency
    prescaleval /= 4096.0;            // 12-bit precision (fixed for PCA9685)
    prescaleval /= freq;              // Desired frequency
    prescaleval -= 1.0;

    UBYTE prescale = (UBYTE)floor(prescaleval + 0.5);

    // Read the current mode register value
    UBYTE oldmode = PCA9685_ReadByte(MODE1);
    
    // Set the new mode to sleep mode to change the frequency
    UBYTE sleepmode = (oldmode & 0x7F) | 0x10;
    PCA9685_WriteByte(MODE1, sleepmode); // Enter sleep mode to change the prescale

    // Set the prescaler value
    PCA9685_WriteByte(PRESCALE, prescale);

    // Restart the oscillator without needing an explicit delay
    UBYTE restartmode = oldmode | 0x80;
    PCA9685_WriteByte(MODE1, oldmode);  // Wake up from sleep mode

    // Ensure the oscillator is stable before enabling auto increment
    while ((PCA9685_ReadByte(MODE1) & 0x10) != 0) {
        // Busy wait until the oscillator restarts and stabilizes
    }

    // Enable auto increment for subsequent register access
    PCA9685_WriteByte(MODE1, restartmode);
}

// where channel is the motor channel and pulse is 0 to 1000:
void PCA9685_SetPwmDutyCycle(UBYTE channel, UWORD pulse)
{
    PCA9685_SetPWM(channel, 0, pulse * (4096 / 1000) - 1);
}

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