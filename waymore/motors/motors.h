/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: motors.h
*
* Description:: Library of functions and wrappers for motor
*				functionality to be called by brain.c
*
**************************************************************/

#ifndef _MOTORS_H_
#define _MOTORS_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include <stdlib.h>
#include <signal.h>
#include "lib/PCA9685.h"
#include "lib/DEV_Config.h"


// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define MOTORHATADDR 0x40
#define MOTORHATFREQ 1000

#define LEFTMOTOR	0
#define AIN1	1
#define AIN2	2
#define BIN1	3
#define BIN2	4
#define RIGHTMOTOR	5

// ============================================================================================= //
// Primary Motor Functions
// ============================================================================================= //

void moveForward(UWORD leftspeed, UWORD rightspeed)
{
    /*
    **  Makes motors move in the forward direction, given a speed parameter for each motor.
	**	To turn, set one speed lower than the other when calling in brain.c
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientation.
    */

    // Validate the speed inputs on both motors
	if (leftspeed > 100) leftspeed = 100;
    if (rightspeed > 100) rightspeed = 100;

    // Apply the speeds to the motors
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftspeed);
	PCA9685_SetLevel(AIN1, 1);
	PCA9685_SetLevel(AIN2, 0);

	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, rightspeed);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 1);
}

void moveBackward(UWORD leftspeed, UWORD rightspeed)
{
    /*
    **  Makes motors move in the backward direction, given a speed parameter for each motor.
	**	To turn, set one speed lower than the other while calling in brain.c
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientation.
    */

    // Validate the speed inputs on both motors
	if (leftspeed > 100) leftspeed = 100;
    if (rightspeed > 100) rightspeed = 100;

    // Apply the speeds to the motors
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftspeed);
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 1);

	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, rightspeed);
	PCA9685_SetLevel(BIN1, 1);
	PCA9685_SetLevel(BIN2, 0);
}

void rotateRight(UWORD speed)
{
	/*
    **  Makes left motor move forward and right motor move backward, rotating in place.
	**	Takes a speed parameter.
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientations.
    */

	// Validate the speed inputs on both motors
	if (speed > 100) speed = 100;

    // Apply the speeds to the motors
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, speed);
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 1);

	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, speed);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 1);
}

void rotateLeft(UWORD speed)
{
	/*
    **  Makes right motor move forward and left motor move backward, rotating in place.
	**	Takes a speed parameter.
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientations.
    */

	// Validate the speed inputs on both motors
	if (speed > 100) speed = 100;

    // Apply the speeds to the motors
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, speed);
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 1);

	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, speed);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 1);
}

void haltMotors()
{
    /*
    **  Halts Motors by setting both duty cycles and voltage levels to 0.
    */

	PCA9685_SetPwmDutyCycle(LEFTMOTOR, 0);
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 0);

	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, 0);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 0);
}


// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initializeMotorHat()
{
    /*
    **	Initializes the Motor given an I2C address and a frequency parameter.
    */

	// Initialize the DEV_Config libraries
	int res = DEV_ModuleInit();
	if (res != 0)
	{
		perror("initializeMotorHat: Error initializing DEV_Module - exiting.");
		exit(1);
	}

	PCA9685_Init(MOTORHATADDR);
	PCA9685_SetPWMFreq(MOTORHATFREQ);

	printf("Motor HAT has been initialized.\n");
}

void uninitializeMotorHat()
{
    haltMotors();
    DEV_ModuleExit();
}


// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif