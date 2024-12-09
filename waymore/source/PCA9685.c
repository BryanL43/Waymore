/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: WaveShare Team / Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: PCA9685.c
*
* Description:: Implementation of modified version of
*               WaveShare's motor demo libraries, adapted to
*               the needs of our project.
*
**************************************************************/

#include "PCA9685.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define MODE1               0x00
#define DEFAULTMODE         0x00
#define SUBADR1             0x02
#define SUBADR2             0x03
#define SUBADR3             0x04
#define PRESCALE            0xFE
#define ON_LOWBYTE          0x06
#define ON_HIGHBYTE         0x07
#define OFF_LOWBYTE         0x08
#define OFF_HIGHBYTE        0x09

// ============================================================================================= //
// Definitions of Internal Variables
// ============================================================================================= //

uint8_t deviceAddress;

// ============================================================================================= //
// Library Functions
// ============================================================================================= //

int PCA9685_initialize(uint8_t ADDR)
{
    if (registerDeviceI2C(ADDR) < 0)
    {
        fprintf(stderr, "Failed to initialize PCA9685 at address 0x%02X\n", ADDR);
        return -1;
    }

    deviceAddress = ADDR;

    // Set PCA9685 to default mode
    char buffer[2] = {DEFAULTMODE, 0x00};
    return writeBytesI2C(deviceAddress, buffer, 2) == 2 ? 0 : -1;
}

static int PCA9685_setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    if (channel > 15) return -1; // Invalid channel

    // Prepare a buffer for the ON and OFF registers
    char buffer[5];
    buffer[0] = ON_LOWBYTE + 4 * channel; // Start register address
    buffer[1] = on & 0xFF;               // ON low byte
    buffer[2] = on >> 8;                 // ON high byte
    buffer[3] = off & 0xFF;              // OFF low byte
    buffer[4] = off >> 8;                // OFF high byte

    // Write all four bytes in a single I2C call
    return writeBytesI2C(deviceAddress, buffer, 5) == 5 ? 0 : -1;
}

int PCA9685_setPwmFrequency(uint16_t frequency)
{
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

    // Calculate prescaler value
    double prescaleval = 25000000.0 / (4096 * frequency) - 1.0;
    uint8_t prescale = (uint8_t)(prescaleval + 0.5);

    // Read the current MODE1 register value
    char reg = MODE1;
    char oldmode;
    if (writeBytesI2C(deviceAddress, &reg, 1) != 1 || readBytesI2C(deviceAddress, &oldmode, 1) != 1)
    {
        fprintf(stderr, "PCA9685 Error: Failed to read MODE1 register at 0x%02X\n", deviceAddress);
        return -1;
    }

    // Enter sleep mode
    char buffer[2];
    buffer[0] = MODE1;
    buffer[1] = (oldmode & 0x7F) | 0x10; // Set sleep bit
    if (writeBytesI2C(deviceAddress, buffer, 2) != 2)
    {
        fprintf(stderr, "PCA9685 Error: Failed to enter sleep mode at 0x%02X\n", deviceAddress);
        return -1;
    }

    // Set prescaler
    buffer[0] = PRESCALE;
    buffer[1] = prescale;
    if (writeBytesI2C(deviceAddress, buffer, 2) != 2)
    {
        fprintf(stderr, "PCA9685 Error: Failed to set prescale value at 0x%02X\n", deviceAddress);
        return -1;
    }

    // Restart PCA9685
    buffer[0] = MODE1;
    buffer[1] = oldmode; // Clear sleep bit
    if (writeBytesI2C(deviceAddress, buffer, 2) != 2)
    {
        fprintf(stderr, "PCA9685 Error: Failed to restore MODE1 register at 0x%02X\n", deviceAddress);
        return -1;
    }

    buffer[1] = oldmode | 0x80; // Set restart bit
    if (writeBytesI2C(deviceAddress, buffer, 2) != 2)
    {
        fprintf(stderr, "PCA9685 Error: Failed to restart PCA9685 at 0x%02X\n", deviceAddress);
        return -1;
    }

    return 0;
}

int PCA9685_setPwmDutyCycle(uint8_t channel, uint8_t dutyCycle)
{
    if (dutyCycle > 100)
    {
        fprintf(stderr, "PCA9685 Warning: Adjusting duty cycle from %d to max of 100!\n", dutyCycle);
        dutyCycle = 100;
    }
    uint16_t pulse = (dutyCycle * 4096) / 100;
    return PCA9685_setPWM(channel, 0, pulse);
}

int PCA9685_setLevel(uint8_t channel, uint8_t value)
{
    return PCA9685_setPWM(channel, 0, value ? 4095 : 0);
}

// ============================================================================================= //
// End of File
// ============================================================================================= //
