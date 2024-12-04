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

	// Initialize the DEV_Config libraries
	int res = DEV_ModuleInit();
	if (res != 0)
	{
		perror("initializeMotorHat: Error initializing DEV_Module - exiting.");
		exit(1);
	}

	PCA9685_Init(MOTORHATADDR);
	PCA9685_SetPWMFreq(MOTORHATFREQ);

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
				PCA9685_SetLevel(AIN1, 1);
				PCA9685_SetLevel(AIN2, 0);
				PCA9685_SetLevel(BIN1, 0);
				PCA9685_SetLevel(BIN2, 1);
				currentAction = action;
			}
			// Set the speeds
			PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftSpeed*0.80);
			PCA9685_SetPwmDutyCycle(RIGHTMOTOR, rightSpeed);
			break;
		case (ROTATELEFT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_SetLevel(AIN1, 0);
				PCA9685_SetLevel(AIN2, 1);
				PCA9685_SetLevel(BIN1, 0);
				PCA9685_SetLevel(BIN2, 1);
				currentAction = action;
			}

			// Set the speeds (both using leftSpeed)
			PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftSpeed);
			PCA9685_SetPwmDutyCycle(RIGHTMOTOR, leftSpeed);
			break;
		case (ROTATERIGHT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_SetLevel(AIN1, 1);
				PCA9685_SetLevel(AIN2, 0);
				PCA9685_SetLevel(BIN1, 1);
				PCA9685_SetLevel(BIN2, 0);
				currentAction = action;
			}

			// Set the speeds (both using leftSpeed)
			PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftSpeed);
			PCA9685_SetPwmDutyCycle(RIGHTMOTOR, leftSpeed);
			break;
		case (BACKWARD):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_SetLevel(AIN1, 0);
				PCA9685_SetLevel(AIN2, 1);
				PCA9685_SetLevel(BIN1, 1);
				PCA9685_SetLevel(BIN2, 0);
				currentAction = action;
			}
			// Set the speeds
			PCA9685_SetPwmDutyCycle(LEFTMOTOR, leftSpeed);
			PCA9685_SetPwmDutyCycle(RIGHTMOTOR, rightSpeed);
			break;
		case (HALT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				PCA9685_SetLevel(AIN1, 0);
				PCA9685_SetLevel(AIN2, 0);
				PCA9685_SetLevel(BIN1, 0);
				PCA9685_SetLevel(BIN2, 0);
				currentAction = action;
			}
			// Set the speeds
			PCA9685_SetPwmDutyCycle(LEFTMOTOR, 0);
			PCA9685_SetPwmDutyCycle(RIGHTMOTOR, 0);
			break;
	}
	milliWait(1);
}

void uninitializeMotorHat()
{
	commandMotors(HALT, 0, 0);
    DEV_ModuleExit();
}

// ============================================================================================= //
// End of File
// ============================================================================================= //