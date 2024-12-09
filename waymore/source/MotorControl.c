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

#include "../headers/MotorControl.h"

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
// Definitions of Private Variables and States
// ============================================================================================= //

MotorAction currentAction;

// ============================================================================================= //
// Public Facing Functions
// ============================================================================================= //

void initializeMotorHat()
{
	/*
	**	Initializes the Motor given an I2C address and a frequency parameter.
	*/

	printf("Initializing waveshare motor HAT...");

	int res = DEV_ModuleInit();
	if (res != 0)
	{
		perror("initializeMotorHat: Error initializing DEV_Module - exiting.");
		exit(1);
	}

	PCA9685_Initialize(MOTORHATADDR);
	PCA9685_setPwmFrequency(MOTORHATFREQ);

	milliWait(1);

	commandMotors(HALT, 0, 0);

	printf("Motor HAT initialized.\n");
}

void commandMotors(MotorAction action, int leftSpeed, int rightSpeed)
{
	// Validate the speed inputs on both motors
	if (leftSpeed > 1000) leftSpeed = 1000;
	else if (leftSpeed < 0) leftSpeed = 0;

    if (rightSpeed > 1000) rightSpeed = 1000;
	else if (rightSpeed < 0) rightSpeed = 0;

	switch (action)
	{
		case (FORWARD):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_setLevel(AIN1, 1);
				PCA9685_setLevel(AIN2, 0);
				PCA9685_setLevel(BIN1, 0);
				PCA9685_setLevel(BIN2, 1);
				currentAction = action;
			}
			// Set the speeds
			PCA9685_setPwmDutyCycle(LEFTMOTOR, leftSpeed*0.80);
			PCA9685_setPwmDutyCycle(RIGHTMOTOR, rightSpeed);
			break;
		case (ROTATELEFT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_setLevel(AIN1, 0);
				PCA9685_setLevel(AIN2, 1);
				PCA9685_setLevel(BIN1, 0);
				PCA9685_setLevel(BIN2, 1);
				currentAction = action;
			}

			// Set the speeds (both using leftSpeed)
			PCA9685_setPwmDutyCycle(LEFTMOTOR, leftSpeed);
			PCA9685_setPwmDutyCycle(RIGHTMOTOR, leftSpeed);
			break;
		case (ROTATERIGHT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_setLevel(AIN1, 1);
				PCA9685_setLevel(AIN2, 0);
				PCA9685_setLevel(BIN1, 1);
				PCA9685_setLevel(BIN2, 0);
				currentAction = action;
			}

			// Set the speeds (both using leftSpeed)
			PCA9685_setPwmDutyCycle(LEFTMOTOR, leftSpeed);
			PCA9685_setPwmDutyCycle(RIGHTMOTOR, leftSpeed);
			break;
		case (BACKWARD):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_setLevel(AIN1, 0);
				PCA9685_setLevel(AIN2, 1);
				PCA9685_setLevel(BIN1, 1);
				PCA9685_setLevel(BIN2, 0);
				currentAction = action;
			}
			// Set the speeds
			PCA9685_setPwmDutyCycle(LEFTMOTOR, leftSpeed);
			PCA9685_setPwmDutyCycle(RIGHTMOTOR, rightSpeed);
			break;
		case (HALT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_setLevel(AIN1, 0);
				PCA9685_setLevel(AIN2, 0);
				PCA9685_setLevel(BIN1, 0);
				PCA9685_setLevel(BIN2, 0);
				currentAction = action;
			}
			// Set the speeds
			PCA9685_setPwmDutyCycle(LEFTMOTOR, 0);
			PCA9685_setPwmDutyCycle(RIGHTMOTOR, 0);
			break;
	}
	milliWait(1);
}

void uninitializeMotorHat()
{
	commandMotors(HALT, 0, 0);
}

// ============================================================================================= //
// End of File
// ============================================================================================= //