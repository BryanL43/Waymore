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
#define LINESENSORCOUNT 4


// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

Thread * thread;
int lineReadings[LINESENSORCOUNT];
int lineSensorPins[] = {27, 17, 18, 23};


// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void * threadLoopIR()
{
    while (thread->running)
    {
        for (int i = 0; i < LINESENSORCOUNT; i++)
        {
            /*
            ** TODO:
            **      Find out whether black line yields HIGH or LOW values.
            **      for now it's just returning raw value.
            */
            lineReadings[i] = getPinLevel(lineSensorPins[i]) ? FALSE : TRUE;
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

int * getLineReadings()
{
    // Return by readings by value for current snapshot
    return lineReadings;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif