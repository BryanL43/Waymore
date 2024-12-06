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

#include "../headers/Brain.h"

// ============================================================================================= //
// Variables and states
// ============================================================================================= //

int running = TRUE;
SensoryData senseData;

// ============================================================================================= //
// Signals and Controls
// ============================================================================================= //

void signalHandler(int sig)
{
/*
**  Allows the safe stopping of the program via Ctrl-C,
**	making sure to uninitialize GPIO pin and stop the motor.
*/
    running = FALSE;
    printf("\nSignal received - stopping gracefully...\n\n");
}

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initialize()
{
    /*
    ** Any libraries needed should be called here, and nowhere else.
    */

    // Initialize register access GPIO library
    initializeGPIO();

    // Initialize camera library
    // initializeCamera(CAMWIDTH, CAMHEIGHT, CAMSLICES);

    // Initialize lidar library
    // ...

    // Initialize motor hat
    initializeMotorHat();

    // Initialize PID controller
    initializePID();

    // senseData.cameraLineDistances = malloc(sizeof(int)*CAMSLICES);
    // senseData.cameraLineConfidences = malloc(sizeof(double)*CAMSLICES);
}

void uninitialize()
{
    /*
    **  Uninitialize any previously initialized libraries here.
    */

    printf("\nUninitializing each library...\n");

    uninitializeMotorHat();
    uninitializeGPIO();
    // uninitializeCamera();

    // free(senseData.cameraLineDistances);
    // free(senseData.cameraLineConfidences);
}

void startSenses()
{
    /*
    **  This is where we will be calling the start() function of each
    **  sense.
    */

    startIR();
    // startCamera();
    //...
}

void stopSenses()
{
    /*
    **  This is where we will be calling the stop() function of each
    **  sense.
    */

    stopIR();
    //...
}

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void mainLoop()
{
    /*
    **  start by collecting the latest data from our senses
    **  and then interpret the senseData and act on it.
    */

    while(running)
    {
        senseData.lineSensorReadings = getLineSensorReadings();
        
        // getCameraLineDistances(senseData.cameraLineDistances);
        // getCameraLineConfidences(senseData.cameraLineConfidences);
        
        double error = calculateLineSensorError(senseData.lineSensorReadings);
        //double error = calculateCameraError(senseData.cameraLineDistances);
        double controlSignal = calculateControlSignal(error);
        // int speedLimit = calculateSpeedLimit(senseData.cameraLineConfidences);
        applyControlSignal(controlSignal, 300);
        milliWait(TIMESTEP_MS);
    }
}

// ============================================================================================= //
// Main
// ============================================================================================= //

int main(int argc, char* argv[])
{
    printf("\nWelcome to Waymore.\n");
    // Initialize all the necessary libraries
    initialize();

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
    uninitialize();

    // Exit
    printf("Goodbye!\n");
    return 0;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //