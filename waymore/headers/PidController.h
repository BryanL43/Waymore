
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

void initializePID();

double calculateControlSignal();

void PIDmotorControl();

#endif