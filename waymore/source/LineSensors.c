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

#include "../headers/LineSensors.h"

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

Thread * lineSensorThread;
int lineReadings[LINESENSORCOUNT];
int lineSensorPins[] = {17, 27, 22, 25, 23, 24};

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void * lineSensorThreadLoop(void * args)
{
    (void)args;

    while (lineSensorThread->running)
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

    return NULL;
}

// ============================================================================================= //
// Start and Stop Functions
// ============================================================================================= //

void startIR()
{
    lineSensorThread = startThread("IR sensor thread", lineSensorThreadLoop);

    if(lineSensorThread == NULL)
    {
        fprintf(stderr, "Failed to start the IR sensor thread. Exiting.\n");
        exit(1);
    }
}

void stopIR()
{
    stopThread(lineSensorThread);
    lineSensorThread = NULL;
}

// ============================================================================================= //
// Functions for external use
// ============================================================================================= //

int * getLineSensorReadings()
{
    return lineReadings;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //