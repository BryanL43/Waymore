/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: ir.h
*
* Description:: Library of functions and wrappers for ir
*		        sensor functionality to be called by brain.c
*
**************************************************************/

#ifndef _IR_H_
#define _IR_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include "../waymoreLib.h"

// ============================================================================================= //
//#include <stdio.h>
//#include <stdlib.h>
//#include <linux/i2c-dev.h>
//#include <sys/ioctl.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <stdint.h>

#define BUS "/dev/i2c-1"    // I2C bus device
#define I2CADDR 0x29        // I2C address of the RGB sensor
#define READREG 0x94        // Register to read from

#define BLACKVAL   0x000000
#define WHITEVAL   0xFFFFFF
#define REDVAL     0xFF0000
#define GREENVAL   0x00FF00
#define BLUEVAL    0x0000FF
#define YELLOWVAL  0xFFFF00
#define ORANGEVAL  0xFFA500
#define PURPLEVAL  0x800080

enum Color
{
    BLACK=BLACKVAL,
    WHITE=WHITEVAL,
    RED=REDVAL,
    GREEN=GREENVAL,
    BLUE=BLUEVAL,
    YELLOW=YELLOWVAL,
    ORANGE=ORANGEVAL,
    PURPLE=PURPLEVAL,
}

typedef struct ColorStruct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    char hex[7];
} ColorStruct;

int device;
Thread * rgbThread;
ColorStruct currentColor;

void configureRGB()
{
    // Initialize a temp variable to store hex configuration settings
    char config[2] = {0};

    // Put Enable register (0x80) into config[0]
    config[0] = 0x80;
    config[1] = 0x03;
    // Write 2 bytes of data to the device
    write(device, config, 2);

    // Put ALS time register (0x81) into config[0]
    config[0] = 0x81;
    // Set the integration time
    config[1] = 0xF6;
    // Write 2 bytes of data to the device
    write(device, config, 2);

    // Put Wait Time register (0x83) into config[0]
    config[0] = 0x83;
    // Set the wait time to 2.4 ms
    config[1] = 0xFF;
    // Write 2 bytes of data to the device
    write(device, config, 2);

    // Put Control register(0x8F) into config[0]
    config[0] = 0x8F;
    // Set the gain to 4x
    config[1] = 0x01;
    // Write 2 bytes of data to the device
    write(device, config, 2);
}

void startRGB()
{
    // Open the I2C bus
    if ((device = open(BUS, O_RDWR)) < 0)
    {
        printf("Failed to open the bus. \n");
        exit(1);
    }

    // Set the I2C address for all subsequent I2C device transfers
    ioctl(device, I2C_SLAVE, I2CADDR);

    // Initialize and configure the RGB sensor
    configureRGB();

    // Wait a moment for the sensor to initialize
    milliWait(500);

    // Start the thread
    rgbThread = startThread("RGB sensor thread", threadLoop);
}

void stopRGB()
{
    // Close the I2C bus
    close(device);
    stopThread(rgbThread);
}

ColorStruct setCurrentColor(int r, int g, int b)
{
    // Cap the values to 0-255 and store in currentColor
    currentColor.red = (r > 255.0) ? 255.0 : ((r < 0) ? 0 : r);
    currentColor.green = (g > 255.0) ? 255.0 : ((g < 0) ? 0 : g);
    currentColor.blue = (b > 255.0) ? 255.0 : ((b < 0) ? 0 : b);
    currentColor.hex = sprintf(currentColor.hex, "#%02x%02x%02x", (uint8_t)r, (uint8_t)g, (uint8_t)b);
}

void readFromSensor(char * dataArray)
{
    // Init an array to store raw data
    char data[8] = {0};

    // Trigger a read
    write(device, READREG, 1);

    // Read the results
    if(read(device, dataArray, 8) != 8)
    {
        fprintf(stderr, "Ran into error while reading from RGB sensor\n");
        exit(1);
    }

    // Convert the data
    uint16_t clear = (data[1] * 256 + data[0]);
    uint16_t red = (data[3] * 256 + data[2]);
    uint16_t green = (data[5] * 256 + data[4]);
    uint16_t blue = (data[7] * 256 + data[6]);

    uint32_t sum = clear;
    if (clear == 0) {
        currentColor = setCurrentColor(0, 0, 0);
        return;
    }

    int r, g, b;
    // Calculate the RGB values
    r = ((float)red / sum) * 255.0;
    g = ((float)green / sum) * 255.0;
    b = ((float)blue / sum) * 255.0;

    setCurrentColor(r, g, b);
}

void * threadLoop()
{
    while (rgbThread->running)
    {
        // Get a fresh reading from the RGB sensor
        readFromSensor();
        // Wait 5 milliseconds and repeat
        milliWait(5);
    }
}

Color getClosestColor()
{
    // Calculate the distance between the current color and each of the predefined colors
    int distances[8] = {
        abs(currentColor.red - BLACK.red) + abs(currentColor.green - BLACK.green) + abs(currentColor.blue - BLACK.blue),
        abs(currentColor.red - WHITE.red) + abs(currentColor.green - WHITE.green) + abs(currentColor.blue - WHITE.blue),
        abs(currentColor.red - RED.red) + abs(currentColor.green - RED.green) + abs(currentColor.blue - RED.blue),
        abs(currentColor.red - GREEN.red) + abs(currentColor.green - GREEN.green) + abs(currentColor.blue - GREEN.blue),
        abs(currentColor.red - BLUE.red) + abs(currentColor.green - BLUE.green) + abs(currentColor.blue - BLUE.blue),
        abs(currentColor.red - YELLOW.red) + abs(currentColor.green - YELLOW.green) + abs(currentColor.blue - YELLOW.blue),
        abs(currentColor.red - ORANGE.red) + abs(currentColor.green - ORANGE.green) + abs(currentColor.blue - ORANGE.blue),
        abs(currentColor.red - PURPLE.red) + abs(currentColor.green - PURPLE.green) + abs(currentColor.blue - PURPLE.blue)
    };

    // Find the index of the smallest distance
    int minIndex = 0;
    for (int i = 1; i < 8; i++)
    {
        if (distances[i] < distances[minIndex])
        {
            minIndex = i;
        }
    }

    // Return the color that corresponds to the smallest distance
    switch (minIndex)
    {
    case 0:
        return BLACK;
    case 1:
        return WHITE;
    case 2:
        return RED;
    case 3:
        return GREEN;
    case 4:
        return BLUE;
    case 5:
        return YELLOW;
    case 6:
        return ORANGE;
    case 7:
        return PURPLE;
    }
}

// ============================================================================================= //
#endif