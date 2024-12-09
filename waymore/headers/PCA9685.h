/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: WaveShare Team / Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: PCA9685.h
*
* Description:: Modified version of WaveShare's motor demo
*               libaries, adapted to the needs of our project.
*
**************************************************************/

#ifndef __PCA9685_H_
#define __PCA9685_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include <math.h>
#include "WaymoreLib.h"

// ============================================================================================= //
// Library Functions
// ============================================================================================= //

int PCA9685_initialize(uint8_t ADDR);

int PCA9685_setPwmFrequency(uint16_t frequency);

int PCA9685_setPwmDutyCycle(uint8_t channel, uint8_t dutyCycle);

int PCA9685_setLevel(uint8_t channel, uint8_t value);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif