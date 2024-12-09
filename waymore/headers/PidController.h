
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

#ifndef _IR_PID_H_
#define _IR_PID_H_

#include <math.h>
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

typedef enum LinePrediction
{
    STRAIGHT,
    CURVE,
    CORNERLEFT,
    CORNERRIGHT,
    MISSING
}LinePrediction;

typedef enum CurrentState
{
    NORMAL,
    CORNERINGLEFT,
    CORNERINGRIGHT,
    OBSTACLEAVOIDANCE
}CurrentState

void initializePID();

double calculateError(int * lineSensorReadings);

void interpretCameraDistances(double * cameraLineDistances);

double calculateControlSignal(double error);

void applyControlSignal(double controlSignal);

#endif