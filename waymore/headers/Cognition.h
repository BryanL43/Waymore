
/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: Cognition.h
*
* Description:: Declarations for Cognitive system
*
**************************************************************/

#ifndef _COGNITION_H_
#define _COGNITION_H_

#include "Brain.h"

// ============================================================================================= //
// Definitions of Structure and Enumerations
// ============================================================================================= //

typedef struct SenseData SenseData; // Forward declaration of struct from Brain.h

typedef struct PIDGains
{
    double proportional;
    double integral;
    double derivative;
}PIDGains;

typedef enum LastLineLocation
{
    LeftOfCar,
    DeadCenter,
    RightOfCar
}LastLineLocation;

typedef enum CurrentState
{
    LineFollowing,
    LineCornering,
    AvoidingObstacle,
    LassoingObstacle
}CurrentState;

typedef enum LassoPath
{
    LassoRight,
    LassoLeft,
}LassoPath;

// ============================================================================================= //
// Functions for Brain
// ============================================================================================= //

void initializeCognition();

// ============================================================================================= //
// Line Sensor Related Functions
// ============================================================================================= //

double calculateLinePosition(LineSensorData * lineSensorData);

// ============================================================================================= //
// Camera Related Functions
// ============================================================================================= //

// double calculateCameraError(CameraData * cameraData);
// double calculateBaseSpeed(double cameraError);

// ============================================================================================= //
// PID Error Calculation
// ============================================================================================= //

double calculateError(double readings, double reference);

// ============================================================================================= //
// Enumerated Control Functions
// ============================================================================================= //

void lineFollow(double linePosition); //, CameraData * cameraData
void lineCorner();

// ============================================================================================= //
// End of File
// ============================================================================================= //

#endif