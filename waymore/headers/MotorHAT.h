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

#ifndef _MOTOR_HAT_H_
#define _MOTOR_HAT_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include "WaymoreLib.h"

// ============================================================================================= //
// Library Functions
// ============================================================================================= //

int registerMotorHat(uint8_t ADDR);

int setMotorHatFrequency(uint32_t frequency);

void setDutyCycle(uint8_t motor, uint32_t pulse);

void setLevel(uint8_t wire, uint32_t value);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif