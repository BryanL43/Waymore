/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: brain.c
*
* Description:: Implementations for Waymore's brain
*
**************************************************************/

#include "brain.h"

// ============================================================================================= //
// Variables and states
// ============================================================================================= //

int running = TRUE;
SensoryData senseData;
PrioritizedSense senseInCharge;

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
    // ...

    // Initialize lidar library
    // ...

    // Initialize motor hat
    initializeMotorHat();

    // Initialize PID controller
    initializePID(TIMESTEPMICROSECONDS, SPEEDSETTING);

    senseInCharge = CAMERA;
}

void uninitializeLibraries()
{
    /*
    **  Uninitialize any previously initialized libraries here.
    */

    printf("\nUninitializing each library...\n");

    uninitializeMotorHat();
    uninitializeGPIO();
}

void startSenses()
{
    /*
    **  This is where we will be calling the start() function of each
    **  sense. Use ir.h as a reference.
    */

    startIR();
    //startCamera();
    //startLidar();
    //...
}

void stopSenses()
{
    /*
    **  This is where we will be calling the stop() function of each
    **  sense. Use ir.h as a reference.
    */

    stopIR();
    //stopCamera();
    //startLidar();
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
    **  and then interpret the senseData and act on it.
    */

    while(running)
    {
        senseData.lineReadings = getLineReadings();
        //senseData.cameraFrame = getCameraFrame(); // or whatever Bryan cooked up
        //senseData.lidarReadings = getLidarReadings(); // or whatever Sukrit cooked up

        // If camera is sensorInCharge:
        // 1. Check if lidar is reading an obstacle in front FOV and within a given distance
        // 2. Check if camera is reading NULL, NaN, or whatever it reads when the line is gone
        //      a. if missing line && reading obstacle in front, switch senseInCharge to LIDAR
        //      b. if we have a line still, we chillin
        // 3. Check if camera is reading a corner straight ahead. If so, set some kind of IR
        //    sensor waiting state to trigger a rotating turn as soon as it reads the corner.
        // 4. If the above states have not been entered, it's business as usual. Generate an
        //    error from the curved line with the camera and apply it to the motors.

        // If lidar is sensorInCharge:
        // To Do: figure out exact lidar path following logic

        //...

        PIDmotorControl(calculateControlSignal(senseData.lineReadings));

        // Wait a bit and repeat
        microWait(TIMESTEPMICROSECONDS);
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