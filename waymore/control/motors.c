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

#include "motors.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define MOTORHATADDR 0x40
#define MOTORHATFREQ 1000	// Frequency Range is 40Hz - 1000Hz

#define LEFTMOTOR	0
#define AIN1	1
#define AIN2	2
#define BIN1	3
#define BIN2	4
#define RIGHTMOTOR	5

// ============================================================================================= //
// Primary Motor Functions
// ============================================================================================= //

void moveForward(int leftSpeed, int rightSpeed)
{
    /*
    **  Makes motors move in the forward direction, given a speed parameter for each motor.
	**	To turn, set one speed lower than the other when calling in brain.c
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientation.
    */

    // Validate the speed inputs on both motors
	if (leftSpeed > 100) leftSpeed = 100;
	else if (leftSpeed < 0) leftSpeed = 0;

    if (rightSpeed > 100) rightSpeed = 100;
	else if (rightSpeed < 0) rightSpeed = 0;

	// Make both wheels to move forward
	PCA9685_SetLevel(AIN1, 1);
	PCA9685_SetLevel(AIN2, 0);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 1);

	// Set the speeds
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftSpeed);
	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, rightSpeed);
}

void moveBackward(int leftSpeed, int rightSpeed)
{
    /*
    **  Makes motors move in the backward direction, given a speed parameter for each motor.
	**	To turn, set one speed lower than the other while calling in brain.c
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientation.
    */

    // Validate the speed inputs on both motors
	if (leftSpeed > 100) leftSpeed = 100;
	else if (leftSpeed < 0) leftSpeed = 0;

    if (rightSpeed > 100) rightSpeed = 100;
	else if (rightSpeed < 0) rightSpeed = 0;

	// Make both wheels to move backward
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 1);
	PCA9685_SetLevel(BIN1, 1);
	PCA9685_SetLevel(BIN2, 0);

	// Set the speeds
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftSpeed);
	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, rightSpeed);
}

void rotateRight(int speed)
{
	/*
    **  Makes left motor move forward and right motor move backward, rotating in place.
	**	Takes a speed parameter.
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientations.
    */

	// Validate the speed input
	if (speed > 100) speed = 100;
	else if (speed < 0) speed = 0;

	// Make wheels move in opposite directions
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 1);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 1);

	// Set the speeds
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, speed);
	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, speed);
}

void rotateLeft(int speed)
{
	/*
    **  Makes right motor move forward and left motor move backward, rotating in place.
	**	Takes a speed parameter.
	**
	**	NOTE:
	**		levels may need to be reversed depending on motor orientations.
    */

	// Validate the speed input
	if (speed > 100) speed = 100;
	else if (speed < 0) speed = 0;

	// Make wheels move in opposite directions
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 1);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 1);

	// Set the speeds
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, speed);
	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, speed);
}

void haltMotors()
{
    /*
    **  Halts Motors by setting voltage levels and duty cycles to 0.
    */

   	// Set power levels to 0
	PCA9685_SetLevel(AIN1, 0);
	PCA9685_SetLevel(AIN2, 0);
	PCA9685_SetLevel(BIN1, 0);
	PCA9685_SetLevel(BIN2, 0);

	// Set the speeds
	PCA9685_SetPwmDutyCycle(LEFTMOTOR, 0);
	PCA9685_SetPwmDutyCycle(RIGHTMOTOR, 0);
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