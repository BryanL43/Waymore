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
LineSensorData * lineSensorData;

// GPIO pins connected in ascending order, left to right (blue/purple wires)
int lineSensorPins[] = {5, 17, 22, 23, 24, 25, 27};

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initializeLineSensors()
{
    lineSensorData = malloc(sizeof(LineSensorData));
    if (lineSensorData == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for line sensor data. Exiting.\n");
        exit(1);
    }
}

void uninitializeLineSensors()
{
    free(lineSensorData);
    lineSensorData = NULL;
}

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
            ** When the pins are HIGH (1), that means LINE.
            ** When the pins are LOW (0), that means NO LINE.
            */
            lineSensorData->levels[i] = getPinLevel(lineSensorPins[i]);
        }
        microWait(1);
    }

    return NULL;
}

// ============================================================================================= //
// Start and Stop Functions
// ============================================================================================= //

void startLineSensors()
{
    lineSensorThread = startThread("IR sensor thread", lineSensorThreadLoop);

    if(lineSensorThread == NULL)
    {
        fprintf(stderr, "Failed to start the IR sensor thread. Exiting.\n");
        exit(1);
    }
}

void stopLineSensors()
{
    stopThread(lineSensorThread);
    lineSensorThread = NULL;
}

// ============================================================================================= //
// Functions for external use
// ============================================================================================= //

LineSensorData * getLineSensorDataRef()
{
    return lineSensorData;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //