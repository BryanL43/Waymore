
/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: irPID.h
*
* Description:: Declarations for PID controller
*				functionality
*
**************************************************************/

#ifndef _COGNITION_H_
#define _COGNITION_H_

#include "WaymoreLib.h"
#include "Brain.h"

typedef struct PIDGains
{
    double proportional;
    double integral;
    double derivative;
}PIDGains;

typedef enum LastLineLocation
{
    LEFTOFCAR,
    DEADCENTER,
    RIGHTOFCAR
}LastLineLocation;

typedef enum CurrentState
{
    NORMAL,
    CORNERINGLEFT,
    CORNERINGRIGHT,
    OBSTACLEAVOIDANCE
}CurrentState;

void initializeCognition();

double calculateLineError(int * lineSensorReadings);
double calculateControlSignal(double error);

double calculateCameraError(double * cameraLineDistances);
double calculateSpeed(double cameraError);

void applyControlSignal(double controlSignal, double speed);

#endif