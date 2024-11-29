/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: ir.c
*
* Description:: Function implementations and states for ir
*		        sensor functionality
*
**************************************************************/

#include "ir.h"

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

Thread * thread;
int lineReadings[LINESENSORCOUNT];
int lineSensorPins[] = {17, 27, 22, 18, 23, 24};

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
            lineReadings[i] = getPinLevel(lineSensorPins[i]);
        }
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
    return lineReadings;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //