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

uint8_t deviceAddress;

// ============================================================================================= //
// Private Functions
// ============================================================================================= //

static void setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    char buffer[5]; // First byte is the starting register, followed by four data bytes

    // Starting register address for the given channel
    buffer[0] = ON_LOWBYTE + 4 * channel;

    // Data bytes for ON and OFF
    buffer[1] = on & 0xFF;        // ON low byte
    buffer[2] = (on >> 8) & 0xFF; // ON high byte
    buffer[3] = off & 0xFF;       // OFF low byte
    buffer[4] = (off >> 8) & 0xFF; // OFF high byte

    // Send all data in one I²C write
    if (bcm2835_i2c_write(buffer, sizeof(buffer)) != BCM2835_I2C_REASON_OK)
    {
        fprintf(stderr, "Failed to set PWM for channel %d\n", channel);
    }
}

// ============================================================================================= //
// Public Functions
// ============================================================================================= //

int registerMotorHat(uint8_t ADDR)
{
    registerDeviceI2C(ADDR);
    deviceAddress = ADDR;

    // Set PCA9685 to default mode
    if (writeByteI2C(ADDR, MODE1, DEFAULTVAL) != 0) {
        fprintf(stderr, "Failed to configure MODE1 register\n");
        return -1;
    }

    milliWait(1);

    return 0;
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
    oldmode = readByteI2C(deviceAddress, MODE1);
    newmode = (oldmode & 0x7F) | 0x10;

    // Make the device go to sleep so we can set the prescaler
    if (writeByteI2C(deviceAddress, MODE1, newmode))
    {
        fprintf(stderr, "Motor HAT Error: Failed to go to sleep\n");
        return -1;
    }

    // Set the prescaler
    if (writeByteI2C(deviceAddress, PRESCALE, prescale) != 0)
    {
        fprintf(stderr, "Motor HAT Error: Failed to set prescaler\n");
        return -1;
    }

    // Go back to old mode
    if (writeByteI2C(deviceAddress, MODE1, oldmode) != 0)
    {
        fprintf(stderr, "Motor HAT Error: Failed to wake up\n");
        return -1;
    }

    milliWait(5); // Add a small wait to make time for stabilization

    // Enable auto-increment in the register pointer
    if (writeByteI2C(deviceAddress, MODE1, oldmode | 0x80) != 0)
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
