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

// GPIO pins connected in ascending order, left to right (blue/purple wires)
int lineSensorPins[] = {5, 17, 22, 23, 24, 25, 27};

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
            ** When the pins are HIGH (1), that means NO LINE.
            ** When the pins are LOW (0), that means it's over black.
            */
            lineReadings[i] = getPinLevel(lineSensorPins[i]) == HIGH ? FALSE : TRUE;
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