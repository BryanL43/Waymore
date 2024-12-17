/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: WaveShare Team / Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: MotorHAT.c
*
* Description:: Implementation for MotorHAT library of functions
*
**************************************************************/

#include "MotorHAT.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define PRESCALE            0xFE

#define MODE1               0x00
#define DEFAULTVAL          0x00

#define SUBADR1             0x02
#define SUBADR2             0x03
#define SUBADR3             0x04

#define ON_LOWBYTE          0x06
#define ON_HIGHBYTE         0x07
#define OFF_LOWBYTE         0x08
#define OFF_HIGHBYTE        0x09

// ============================================================================================= //
// Definitions of Internal Variables
// ============================================================================================= //

const int frequencyHz = 1200;

// ============================================================================================= //
// Private Functions
// ============================================================================================= //

static void setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    writeByteI2C(MOTORHATADDR, ON_LOWBYTE + 4 * channel, on & 0xFF);
    writeByteI2C(MOTORHATADDR, ON_HIGHBYTE + 4 * channel, (on >> 8) & 0xFF);
    writeByteI2C(MOTORHATADDR, OFF_LOWBYTE + 4 * channel, off & 0xFF);
    writeByteI2C(MOTORHATADDR, OFF_HIGHBYTE + 4 * channel, (off >> 8) & 0xFF);
}

// ============================================================================================= //
// Public Functions
// ============================================================================================= //

void initializeMotorHat()
{
	/*
	**	Initializes the Motor given an I2C address and a frequency parameter.
	*/

	printf("Initializing waveshare motor HAT...");

    registerDeviceI2C(MOTORHATADDR);

    // Set PCA9685 to default mode
    writeByteI2C(MOTORHATADDR, MODE1, DEFAULTVAL);

    setMotorHatFrequency(frequencyHz);

	printf("done.\n");
}

int setMotorHatFrequency(uint32_t frequency)
{
    printf("Setting frequency...");

    if (frequency < 24)
    {
        fprintf(stderr, "PCA9685 Warning: adjusting %d Hz to device's minimum (24 Hz)!\n", frequency);
        frequency = 24;
    }

    if (frequency > 1526)
    {
        fprintf(stderr, "PCA9685 Warning: adjusting %d Hz to device's maximum (1526 Hz)!\n", frequency);
        frequency = 1526;
    }

    uint8_t oldmode, newmode;

    // Calculate prescaler value
    double prescaleval = 25000000.0 / (4096 * frequency) - 1.0;
    uint8_t prescale = (uint8_t)(prescaleval + 0.5);

    // Read the current MODE1 register value
    oldmode = readByteI2C(MOTORHATADDR, MODE1);
    newmode = (oldmode & 0x7F) | 0x10;

    // Make the device go to sleep so we can set the prescaler
    if (writeByteI2C(MOTORHATADDR, MODE1, newmode))
    {
        fprintf(stderr, "Motor HAT Error: Failed to go to sleep\n");
        return -1;
    }

    // Set the prescaler
    if (writeByteI2C(MOTORHATADDR, PRESCALE, prescale) != 0)
    {
        fprintf(stderr, "Motor HAT Error: Failed to set prescaler\n");
        return -1;
    }

    // Go back to old mode
    if (writeByteI2C(MOTORHATADDR, MODE1, oldmode) != 0)
    {
        fprintf(stderr, "Motor HAT Error: Failed to wake up\n");
        return -1;
    }

    milliWait(5); // Add a small wait to make time for stabilization

    // Enable auto-increment in the register pointer
    if (writeByteI2C(MOTORHATADDR, MODE1, oldmode | 0x80) != 0)
    {
        fprintf(stderr, "Motor HAT Error: Failed to turn on auto increment\n");
        return -1;
    }

    return 0;
}

void setDutyCycle(uint8_t motor, uint32_t pulse)
{
    setPWM(motor, 0, pulse * (4096 / 100) - 1);
}

void setLevel(uint8_t wire, uint32_t value)
{
    if (value == 1)
        setPWM(wire, 0, 4095);
    else
        setPWM(wire, 0, 0);
}

// ============================================================================================= //
// End of File
// ============================================================================================= //
