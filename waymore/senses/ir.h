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

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct IRreadings
{
    int obstacleSensorReadings[OBSTACLESENSORCOUNT];
    int lineSensorReadings[LINESENSORCOUNT];
}IRreadings;

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

Thread * thread;
IRreadings * readings;

int obstacleSensorPins[] = {0};
int lineSensorPins[] = {27, 17, 18, 23};


// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void * threadLoopIR()
{
    while (thread->running)
    {
        // Read current sensor values into state variables
        for (int i = 0; i < OBSTACLESENSORCOUNT; i++)
        {

            /*
            ** We know that when pin is HIGH, there is no obstacle,
            ** so we will return FALSE when HIGH and TRUE when LOW.
            */ 
            readings->obstacleSensorReadings[i] = getPinLevel(obstacleSensorPins[i]) ? FALSE : TRUE;
        }
        for (int i = 0; i < LINESENSORCOUNT; i++)
        {
            /*
            ** TODO:
            **      Find out whether black line yields HIGH or LOW values.
            **      for now it's just returning raw value.
            */

            readings->lineSensorReadings[i] = getPinLevel(obstacleSensorPins[i]) ? FALSE : TRUE;
        }

        // Wait 10 microseconds and repeat
        microWait(10);
    }
}


// ============================================================================================= //
// Start and Stop Functions
// ============================================================================================= //

void startIR()
{
    readings = (IRreadings*) malloc(sizeof(IRreadings));
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
    free(readings);
    readings = NULL;
}


// ============================================================================================= //
// Functions for external use
// ============================================================================================= //

IRreadings getIRreadings()
{
    // Return by readings by value for current snapshot
    return *readings;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif