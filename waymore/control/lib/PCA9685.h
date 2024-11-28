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

#include "DEV_Config.h"
#include <math.h>
#include <stdio.h>

// ============================================================================================= //
// Library Functions
// ============================================================================================= //

static void PCA9685_WriteByte(UBYTE reg, UBYTE value);

static UBYTE PCA9685_ReadByte(UBYTE reg);

static void PCA9685_SetPWM(UBYTE channel, UWORD on, UWORD off);

void PCA9685_Init(char addr);

void PCA9685_SetPWMFreq(UWORD freq);

void PCA9685_SetPwmDutyCycle(UBYTE channel, UWORD pulse);

void PCA9685_SetLevel(UBYTE channel, UWORD value);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif