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

volatile int running = TRUE;
SenseData * senseData;

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

    // Initialize interfaces
    initializeGPIO();
    initializeI2C();
    initializeRGB(INTEGRATE2_4MS, GAIN16X);
    initializeMotorHat();

    // Initialize senses
    initializeLineSensors();
    //initializeCamera();
    initializeLidar();


    senseData = (SenseData *) malloc(sizeof(SenseData));
    senseData->lineSensorData = getLineSensorDataRef();
    //senseData->cameraData = getCameraDataRef();
    senseData->lidarData = getLidarDataRef();

    initializeCognition();
}

void uninitialize()
{
    /*
    **  Uninitialize any previously initialized libraries here.
    */

    printf("\nUninitializing each library...\n");

    uninitializeRGB();
    uninitializeLidar();
    //uninitializeCamera();
    uninitializeLineSensors();
    uninitializeGPIO();
    free(senseData);
    senseData = NULL;
}

void start()
{
    /*
    **  This is where we will be calling all start() functions
    */

    //startLineSensors();
    //startCamera();
    startLidar();
    milliWait(1000);
}

void stop()
{
    /*
    **  This is where we will be calling all stop() functions
    */

    // Camera and Lidar have no stop functions
    //stopLineSensors();
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
        int noObstacles = senseData->lidarData->degreeDistances[60] > 800;

        refreshLineSensorData();
        double linePosition = calculateLinePosition(senseData->lineSensorData);

        int lineBelow = (!isnan(linePosition));

        if(lineBelow)
            lineFollow(linePosition);
        else
            lineCorner();

        if (strcmp(readColor().colorName, "Red") == 0) {
            running = FALSE;
            continue;
        }

        microWait(1);
    }

    // Stop the motors and exit
    commandMotors(HALT, 0, 0);
}

// ============================================================================================= //
// Main
// ============================================================================================= //

int main(int argc, char* argv[])
{
    // Initialize Ctrl-C signal handler for safe stopping
	signal(SIGINT, signalHandler);

    printf("\nWelcome to Waymore.\n");
    // Initialize all the necessary libraries
    initialize();

    // Start the sensing threads
    start();

    printf("\nUse Ctrl-C to exit.\nStarting in\n3...\n");
    milliWait(333);
    printf("2...\n");
    milliWait(333);
    printf("1...\n");
    milliWait(333);

    // Begin the main loop (Ctrl-C to exit)
    mainLoop();

    // Stop the sensing threads
    stop();

    // Uninitialize the libraries
    uninitialize();

    // Exit
    printf("Goodbye!\n");
    return 0;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //