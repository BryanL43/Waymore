/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: rgb.h
*
* Description:: Library of functions and wrappers for RGB
*		        sensor functionality to be called by brain.c
*
**************************************************************/

#ifndef _RGB_H_
#define _RGB_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "../waymoreLib.h"

// ============================================================================================= //
//#include <stdio.h>
//#include <stdlib.h>

//#include <fcntl.h>
//#include <unistd.h>
//#include <stdint.h>

#define BUS "/dev/i2c-1"    // I2C bus device
#define I2CADDR 0x29        // I2C address of the RGB sensor

#define BLACK   0x000000
#define WHITE   0xFFFFFF
#define RED     0xFF0000
#define GREEN   0x00FF00
#define BLUE    0x0000FF
#define YELLOW  0xFFFF00
#define ORANGE  0xFFA500
#define PURPLE  0x800080

typedef struct Color
{
    uint32_t value;
    char name[12];
}Color;

typedef struct ColorReading
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint32_t hex;
} ColorReading;

int device;
Thread * rgbThread;
ColorReading current;
Color closest;

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

ColorReading setCurrentReading(int r, int g, int b)
{
    // Cap the values to 0-255 and store in current
    current.red = (r > 255.0) ? 255.0 : ((r < 0) ? 0 : r);
    current.green = (g > 255.0) ? 255.0 : ((g < 0) ? 0 : g);
    current.blue = (b > 255.0) ? 255.0 : ((b < 0) ? 0 : b);
    current.hex = (r << 16) | (g << 8) | b;
}

void setClosestColor()
{
    // Calculate the distance between the current color and each of the predefined colors
    int distances[8] = {
        // Black
        abs(current.red - 0x00) + abs(current.green - 0x00) + abs(current.blue - 0x00),
        // White
        abs(current.red - 0xFF) + abs(current.green - 0xFF) + abs(current.blue - 0xFF),
        // Red
        abs(current.red - 0xFF) + abs(current.green - 0x00) + abs(current.blue - 0x00),
        // Green
        abs(current.red - 0x00) + abs(current.green - 0xFF) + abs(current.blue - 0x00),
        // Blue
        abs(current.red - 0x00) + abs(current.green - 0x00) + abs(current.blue - 0xFF),
        // Yellow
        abs(current.red - 0xFF) + abs(current.green - 0xFF) + abs(current.blue - 0x00),
        // Orange
        abs(current.red - 0xFF) + abs(current.green - 0xA5) + abs(current.blue - 0x00),
        // Purple
        abs(current.red - 0x80) + abs(current.green - 0x00) + abs(current.blue - 0x80)
    };

    // Find the index of the smallest distance
    int idx = 0;
    for (int i = 1; i < 8; i++)
    {
        if (distances[i] < distances[idx])
        {
            idx = i;
        }
    }

    // Return the color that corresponds to the smallest distance
    switch (idx)
    {
        case 0:
            closest.value = BLACK;
            strcpy(closest.name, "Black");
            break;
        case 1:
            closest.value = WHITE;
            strcpy(closest.name, "White");
            break;
        case 2:
            closest.value = RED;
            strcpy(closest.name, "Red");
            break;
        case 3:
            closest.value = GREEN;
            strcpy(closest.name, "Green");
            break;
        case 4:
            closest.value = BLUE;
            strcpy(closest.name, "Blue");
            break;
        case 5:
            closest.value = YELLOW;
            strcpy(closest.name, "Yellow");
            break;
        case 6:
            closest.value = ORANGE;
            strcpy(closest.name, "Orange");
            break;
        case 7:
            closest.value = PURPLE;
            strcpy(closest.name, "Purple");
            break;
    }
}

void readFromSensor()
{
    // Init an array to store raw data
    char data[8] = {0};

    // Need this in memory for the write function signature
    uint8_t reg = 0x94;

    // Trigger a read
    write(device, &reg, 1);

    // Read the results
    if(read(device, data, 8) != 8)
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
    int r, g, b;
    // Calculate the RGB values
    r = ((float)red / sum) * 255.0;
    g = ((float)green / sum) * 255.0;
    b = ((float)blue / sum) * 255.0;

    setCurrentReading(r, g, b);
}

void * threadLoop()
{
    while (rgbThread->running)
    {
        // Get a fresh reading from the RGB sensor
        readFromSensor();
        setClosestColor();
        // Wait 5 milliseconds and repeat
        milliWait(5);
    }
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

Color getColor()
{
    return closest;
}

// ============================================================================================= //
#endif