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

#include "MotorHAT.h"
#include "WaymoreLib.h"

// ============================================================================================= //
// Definitions of Structures or States
// ============================================================================================= //

typedef enum MotorAction
{
    FORWARD,
    ROTATELEFT,
    ROTATERIGHT,
    BACKWARD,
    HALT,
}MotorAction;

// ============================================================================================= //
// Public facing functions
// ============================================================================================= //

void commandMotors(MotorAction action, int leftSpeed, int rightSpeed);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif