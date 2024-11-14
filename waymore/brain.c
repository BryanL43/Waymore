/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: brain.c
*
* Description:: The point of entry for Waymore's final project
*               and the heart of its business logic
*
**************************************************************/

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include <stdio.h>
#include "waymoreLib.h"
#include "motors/motors.h"
#include "senses/ir.h"

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct SensoryData
{
    // IR Sensor Readings
    int obstacleReading;
    int leftLineReading;
    int middleLineReading;
    int rightLineReading;

    // RGB Sensor Readings
    //...

    // Camera Readings
    //...
} SensoryData;

// ============================================================================================= //
// Variables and states
// ============================================================================================= //

int running = TRUE;
SensoryData data;

// ============================================================================================= //
// Signals and Controls
// ============================================================================================= //

void signalHandler(int sig)
{
/*
**  Allows the safe stopping of the program via Ctrl-C,
**	making sure to uninitialize GPIO pin and stop the motor.
*/
	printf("\nSignal received - stopping gracefully...\n\n");
    running = FALSE;
}

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initializeLibraries()
{
    /*
    ** Any libraries needed should be called here, and nowhere else.
    */

    printf("Initializing each library...\n");

    // Initialize register access GPIO library
    initializeGPIO();

    // TODO: Initialize camera library
    //...

    // Initialize motor hat
    initializeMotorHat();
}

void uninitializeLibraries()
{
    /*
    **  Uninitialize any previously initialized libraries here.
    */

    printf("\nUninitializing each library...\n");
    uninitializeGPIO();
    uninitializeMotorHat();
}

void startSenses()
{
    /*
    **  This is where we will be calling the start() function of each
    **  sense. Use ir.h as a reference.
    */

    startIR();
    //TODO: startCamera();
    //TODO: startRGB();
    //...
}

void stopSenses()
{
    /*
    **  This is where we will be calling the stop() function of each
    **  sense. Use ir.h as a reference.
    */

    stopIR();
    //TODO: stopCamera();
    //TODO: stopRGB();
    //...
}

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void mainLoop()
{
    /*
    **  This is a simplified version of the type of main loop we'll need,
    **  where we start by collecting the latest data from our senses
    **  and then interpret the data and act on it.
    */

    while(running)
    {
        // Collect latest data from senses
        data.obstacleReading = getObstacleReading();
        data.leftLineReading = getLeftLineReading();
        //data.middleLineReading = getMiddleLineReading();
        data.rightLineReading = getRightLineReading();

        //TODO: implement other senses
        //...
        
        // Interpret data and make decision
        if(data.obstacleReading == FALSE)
        {
            // Example actions below - uncomment one at a time to try them out

            moveForward(100, 100);      // Full speed ahead (left speed %, right speed %)

            //moveForward(100, 75);       // High speed right turn (left speed %, right speed %)

            //moveForward(75, 100);       // High speed left turn (left speed %, right speed %)

            // rotateLeft(100);           // Rotate left (speed %)

            // rotateRight(100);          // Rotate right (speed %)
        }
        else
        {
            haltMotors();
        }

        // Wait 10ms and repeat
        milliWait(10);
    }
}

// ============================================================================================= //
// Main
// ============================================================================================= //

int main(int argc, char* argv[])
{
    printf("\nWelcome to Waymore.\n");
    // Initialize all the necessary libraries
    initializeLibraries();

    // Start the sensing threads
    startSenses();

	// Initialize Ctrl-C signal handler for safe stopping
	signal(SIGINT, signalHandler);
    printf("\nYou may now safely use Ctrl-C to exit.\n");

    // Begin the main loop (Ctrl-C to exit)
    mainLoop();

    // Stop the sensing threads
    stopSenses();

    // Uninitialize the libraries
    uninitializeLibraries();

    // Exit
    printf("Goodbye!\n");
    return 0;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //