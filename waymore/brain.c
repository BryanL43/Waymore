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
#include "senses/rgb.h"

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct SensoryData
{
    // Obstacle and Line Sensor counts and readings
    IRreadings currentIR;

    // RGB Sensor Readings
    Color currentColor;

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

    // Initialize camera library
    //...

    // Initialize RGB sensor library
    initializeRGB();

    // Initialize motor hat
    //initializeMotorHat();
}

void uninitializeLibraries()
{
    /*
    **  Uninitialize any previously initialized libraries here.
    */

    printf("\nUninitializing each library...\n");

    uninitializeRGB();
    //uninitializeMotorHat();
    uninitializeGPIO();
}

void startSenses()
{
    /*
    **  This is where we will be calling the start() function of each
    **  sense. Use ir.h as a reference.
    */

    //startIR();
    //startCamera();
    startRGB();
    //...
}

void stopSenses()
{
    /*
    **  This is where we will be calling the stop() function of each
    **  sense. Use ir.h as a reference.
    */

    //stopIR();
    //stopCamera();
    stopRGB();
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
        // Collect latest IR data
        //IRreadings ir = getIRreadings();
        
        // Collect latest RGB data
        Color color = getClosestColor();
        printf("Color: %x", color);

        // Interpret data and make decision
        // if(reading == TRUE)
        // {
        //     // Example actions below - uncomment one at a time to try them out

        //     moveForward(100, 100);      // Full speed ahead (left speed %, right speed %)

        //     //moveForward(100, 75);       // High speed right turn (left speed %, right speed %)

        //     //moveForward(75, 100);       // High speed left turn (left speed %, right speed %)

        //     // rotateLeft(100);           // Rotate left (speed %)

        //     // rotateRight(100);          // Rotate right (speed %)
        // }
        // else
        // {
        //     haltMotors();
        // }

        // Wait a bit and repeat
        milliWait(1000);
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