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
#define DEFAULTFREQ  1000

#define LEFTMOTOR	0
#define AIN1		1
#define AIN2		2
#define BIN1		3
#define BIN2		4
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

	registerMotorHat(MOTORHATADDR);
	setMotorHatFrequency(DEFAULTFREQ);
	commandMotors(HALT, 0, 0);

	printf("done.\n");
}

void commandMotors(MotorAction action, int leftSpeed, int rightSpeed)
{
	// Validate the speed inputs on both motors
	if (leftSpeed > 100) leftSpeed = 100;
	else if (leftSpeed < 0) leftSpeed = 0;

    if (rightSpeed > 100) rightSpeed = 100;
	else if (rightSpeed < 0) rightSpeed = 0;

	switch (action)
	{
		case (FORWARD):
			// Apply the current direction if different
			if (action != currentAction)
			{
				setLevel(AIN1, 0);
				setLevel(AIN2, 1);
				setLevel(BIN1, 1);
				setLevel(BIN2, 0);
				milliWait(3);
				currentAction = action;
			}
			// Set the speeds
			setDutyCycle(LEFTMOTOR, leftSpeed);
			setDutyCycle(RIGHTMOTOR, rightSpeed);
			break;
		case (ROTATELEFT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				setLevel(AIN1, 1);
				setLevel(AIN2, 0);
				setLevel(BIN1, 1);
				setLevel(BIN2, 0);
				milliWait(3);
				currentAction = action;
			}

			// Set the speeds (both using leftSpeed)
			setDutyCycle(LEFTMOTOR, leftSpeed);
			setDutyCycle(RIGHTMOTOR, leftSpeed);
			break;
		case (ROTATERIGHT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				setLevel(AIN1, 0);
				setLevel(AIN2, 1);
				setLevel(BIN1, 0);
				setLevel(BIN2, 1);
				milliWait(1);
				currentAction = action;
			}

			// Set the speeds (both using leftSpeed)
			setDutyCycle(LEFTMOTOR, leftSpeed);
			setDutyCycle(RIGHTMOTOR, leftSpeed);
			break;
		case (BACKWARD):
			// Apply the current direction if different
			if (action != currentAction)
			{
				setLevel(AIN1, 1);
				setLevel(AIN2, 0);
				setLevel(BIN1, 0);
				setLevel(BIN2, 1);
				milliWait(1);
				currentAction = action;
			}
			// Set the speeds
			setDutyCycle(LEFTMOTOR, leftSpeed);
			setDutyCycle(RIGHTMOTOR, rightSpeed);
			break;
		case (HALT):
			// Apply the current direction if different
			if (action != currentAction)
			{
				setLevel(AIN1, 0);
				setLevel(AIN2, 0);
				setLevel(BIN1, 0);
				setLevel(BIN2, 0);
				milliWait(1);
				currentAction = action;
			}
			// Set the speeds
			setDutyCycle(LEFTMOTOR, 0);
			setDutyCycle(RIGHTMOTOR, 0);
			break;
	}
	milliWait(1);
}

// ============================================================================================= //
// End of File
// ============================================================================================= //