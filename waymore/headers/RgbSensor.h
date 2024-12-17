/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: RgbSensor.h
*
* Description:: Declarations of functions and wrappers
*		        for TCS34725 RGB sensor functionality.
*
**************************************************************/

#ifndef _RGB_H_
#define _RGB_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <math.h> // used for fmin, fmax, and fmod. NOTE: need to compile with -lm
// #include "WaymoreLib.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define BUS "/dev/i2c-1"    // I2C bus device
#define RGBADDR 0x29        // I2C address of the RGB sensor
#define FUZZYTHRESHOLD 15   // Fuzzy error for color matching

// ============================================================================================= //
// Definitions of Types
// ============================================================================================= //

typedef enum TCS34725_Atime
{
    INTEGRATE2_4MS = 0XFF,
    INTEGRATE24MS = 0xF6,
    INTEGRATE50MS = 0xEB,
    INTEGRATE101MS = 0xD5,
    INTEGRATE154MS = 0xC0,
    INTEGRATE700MS = 0x00,
}Atime;

typedef enum TCS34725_Gain
{
    GAIN1X = 0x00,
    GAIN4X = 0x01,
    GAIN16X = 0x02,
    GAIN60X = 0x03,
}Gain;

typedef struct RGB
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}RGB;

typedef struct HSVData
{
    double hue;
    double saturation;
    double value;
    char colorName[8];
}HSVData;

typedef enum HsvColor
{
    REDLOW = 0,
    YELLOW = 60,
    GREEN = 120,
    CYAN = 180,
    BLUE = 240,
    MAGENTA = 300,
    REDHIGH = 360,
}HsvColor;

// ============================================================================================= //
// Functions intended for public use
// ============================================================================================= //

void initializeRGB(enum TCS34725_Atime, enum TCS34725_Gain);
HSVData readColor();
void uninitializeRGB();

// ============================================================================================= //
// End of File
// ============================================================================================= //

#endif