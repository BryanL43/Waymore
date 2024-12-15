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

#define LEFTMOTOR	0
#define AIN1		1
#define AIN2		2
#define BIN1		3
#define BIN2		4
#define RIGHTMOTOR	5

// ============================================================================================= //
// Definitions of Private Variables and States
// ============================================================================================= //

const int frequencyHz = 1500;

MotorAction currentAction = FORWARD;
int currentLeftSpeed = 0;
int currentRightSpeed = 0;

// ============================================================================================= //
// Private functions
// ============================================================================================= //

void commandMotors(MotorAction newAction, int newLeftSpeed, int newRightSpeed)
{
	// Validate the speed inputs on both motors
	if (newLeftSpeed > 100) newLeftSpeed = 100;
	else if (newLeftSpeed < 0) newLeftSpeed = 0;

    if (newRightSpeed > 100) newRightSpeed = 100;
	else if (newRightSpeed < 0) newRightSpeed = 0;

	switch (newAction)
	{
		case (FORWARD):
			// Apply the new direction if different
			if (currentAction != newAction)
			{
				setLevel(AIN1, 0);
				setLevel(AIN2, 1);
				setLevel(BIN1, 1);
				setLevel(BIN2, 0);
				currentAction = newAction;
			}
			// Set the speeds
			if(newLeftSpeed != currentLeftSpeed)
			{
				setDutyCycle(LEFTMOTOR, newLeftSpeed);
				currentLeftSpeed = newLeftSpeed;
			}
			if(newRightSpeed != currentRightSpeed)
			{
				setDutyCycle(RIGHTMOTOR, newRightSpeed);
				currentRightSpeed = newRightSpeed;
			}
			break;

		case (ROTATELEFT):
			// Apply the new direction if different
			if (currentAction != newAction)
			{
				setLevel(AIN1, 1);
				setLevel(AIN2, 0);
				setLevel(BIN1, 1);
				setLevel(BIN2, 0);
				currentAction = newAction;
			}

			// Set the speeds
			if(newLeftSpeed != currentLeftSpeed)
			{
				setDutyCycle(LEFTMOTOR, newLeftSpeed);
				currentLeftSpeed = newLeftSpeed;
			}
			if(newRightSpeed != currentRightSpeed)
			{
				setDutyCycle(RIGHTMOTOR, newRightSpeed);
				currentRightSpeed = newRightSpeed;
			}
			break;

		case (ROTATERIGHT):
			// Apply the new direction if different
			if (currentAction != newAction)
			{
				setLevel(AIN1, 0);
				setLevel(AIN2, 1);
				setLevel(BIN1, 0);
				setLevel(BIN2, 1);
				currentAction = newAction;
			}

			// Set the speeds
			if(newLeftSpeed != currentLeftSpeed)
			{
				setDutyCycle(LEFTMOTOR, newLeftSpeed);
				currentLeftSpeed = newLeftSpeed;
			}
			if(newRightSpeed != currentRightSpeed)
			{
				setDutyCycle(RIGHTMOTOR, newRightSpeed);
				currentRightSpeed = newRightSpeed;
			}
			break;

		case (BACKWARD):
			// Apply the new direction if different
			if (currentAction != newAction)
			{
				setLevel(AIN1, 1);
				setLevel(AIN2, 0);
				setLevel(BIN1, 0);
				setLevel(BIN2, 1);
				currentAction = newAction;
			}

			// Set the speeds
			if(newLeftSpeed != currentLeftSpeed)
			{
				setDutyCycle(LEFTMOTOR, newLeftSpeed);
				currentLeftSpeed = newLeftSpeed;
			}
			if(newRightSpeed != currentRightSpeed)
			{
				setDutyCycle(RIGHTMOTOR, newRightSpeed);
				currentRightSpeed = newRightSpeed;
			}
			break;

		case (HALT):
			// Apply the new direction if different
			if (currentAction != newAction)
			{
				setLevel(AIN1, 0);
				setLevel(AIN2, 0);
				setLevel(BIN1, 0);
				setLevel(BIN2, 0);
				currentAction = newAction;
			}

			// Set the speeds
			if(newLeftSpeed != currentLeftSpeed)
			{
				setDutyCycle(LEFTMOTOR, newLeftSpeed);
				currentLeftSpeed = newLeftSpeed;
			}
			if(newRightSpeed != currentRightSpeed)
			{
				setDutyCycle(RIGHTMOTOR, newRightSpeed);
				currentRightSpeed = newRightSpeed;
			}
			break;
	}
}

// ============================================================================================= //
// Public functions
// ============================================================================================= //

void initializeMotorHat()
{
	/*
	**	Initializes the Motor given an I2C address and a frequency parameter.
	*/

	printf("Initializing waveshare motor HAT...");

	registerMotorHat(MOTORHATADDR);
	setMotorHatFrequency(frequencyHz);

	commandMotors(HALT, 0, 0);

	printf("done.\n");
}

// ============================================================================================= //
// End of File
// ============================================================================================= //