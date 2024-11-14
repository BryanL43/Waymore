/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: ir.h
*
* Description:: Library of functions and wrappers for ir
*		        sensor functionality to be called by brain.c
*
**************************************************************/

#ifndef _IR_H_
#define _IR_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include "../waymoreLib.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

// Counts for each sensor type currently on rig
#define OBSTACLESENSORCOUNT 0
#define LINESENSORCOUNT 4

// GPIO pin numbers for sensors from left to right, where left is the front left sensor
#define LINESENSOR1 27
#define LINESENSOR2 17
#define LINESENSOR3 18
#define LINESENSOR4 23

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct IRreadings
{
    int obstacleSensorCount;
    int obstacleSensorReadings[];
    int lineSensorCount;
    int * lineSensorReadings;
}

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

Thread * thread;
IRreadings readings;
int * obstacleSensorLevels;
int * lineSensorLevels;

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void * threadLoopIR()
{
    while (thread->running)
    {
        // Read current sensor values into state variables
        obstacleSensorLevel     = getPinLevel(OBSTACLEPIN);
        leftLineSensorLevel     = getPinLevel(LEFTLINEPIN);
        //middleLineSensorLevel   = getPinLevel(MIDDLELINEPIN);
        rightLineSensorLevel    = getPinLevel(RIGHTLINEPIN);

        // Wait 10 milliseconds and repeat
        milliWait(10);
    }
}


// ============================================================================================= //
// Start and Stop Functions
// ============================================================================================= //

void startIR()
{
    IRreadings readings;
    obstacleSensorLevels[];
    int lineSensorLevels[];

    thread = startThread("IR sensor thread", threadLoopIR);
    if(thread == NULL)
    {
        fprintf(stderr, "Failed to start the IR sensor thread. Exiting.\n");
        exit(1);
    }
}

void stopIR()
{
    stopThread(thread);
    thread = NULL;
}


// ============================================================================================= //
// Functions for external use
// ============================================================================================= //

int getObstacleReading(int sensor)
{
    /*
    ** We know that when pin is HIGH, there is no obstacle,
    ** so we will return FALSE when HIGH and TRUE when LOW.
    */ 
    return obstacleSensorLevel ? FALSE : TRUE;
}

int getLineReading(int sensor)
{
    /*
    ** TODO: We need to figure out whether black == HIGH or LOW
    */
    return lineSensorLevel[sensor];
}


// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif