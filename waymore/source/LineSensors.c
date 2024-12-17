#include "../headers/LineSensors.h"

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

Thread *lineSensorThread = NULL;
LineSensorData *lineSensorData = NULL;

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

    // Initialize sensor levels to zero
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorData->levels[i] = 0;
    }
}

void uninitializeLineSensors()
{
    if (lineSensorData != NULL)
    {
        free(lineSensorData);
        lineSensorData = NULL;
    }
}

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void *lineSensorThreadLoop(void *args)
{
    (void)args;

    if (lineSensorThread == NULL)
    {
        fprintf(stderr, "Error: lineSensorThread is NULL\n");
        pthread_exit(NULL);
    }

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
        microWait(100);
    }
    return NULL;
}

// ============================================================================================= //
// Start and Stop Functions
// ============================================================================================= //

void startLineSensors()
{
    lineSensorThread = startThread("IR sensor thread", lineSensorThreadLoop);
    if (lineSensorThread == NULL)
    {
        fprintf(stderr, "Error: Failed to start the IR sensor thread. Exiting.\n");
        exit(1);
    }
}

void stopLineSensors()
{
    if (lineSensorThread != NULL)
    {
        stopThread(lineSensorThread);
        lineSensorThread = NULL;
    }
}

// ============================================================================================= //
// Functions for external use
// ============================================================================================= //

LineSensorData * getLineSensorDataRef()
{
    return lineSensorData;
}

void refreshLineSensorData()
{
    if (lineSensorData == NULL)
    {
        fprintf(stderr, "Error: Line sensor data is NULL!\n");
        exit(1);
    }

    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        /*
        ** When the pins are HIGH (1), that means LINE.
        ** When the pins are LOW (0), that means NO LINE.
        */
        lineSensorData->levels[i] = getPinLevel(lineSensorPins[i]);
    }
}
