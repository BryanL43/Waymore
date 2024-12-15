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

    // Initialize interfaces
    initializeGPIO();
    initializeI2C();
    initializeMotorHat();

    // Initialize senses
    initializeLineSensors();
    initializeCamera();
    initializeLidar();

    senseData.lineSensorData = getLineSensorDataRef();
    senseData.cameraData = getCameraDataRef();
    senseData.lidarData = getLidarDataRef();

    // Initialize cognition
    initializeCognition();
}

void uninitialize()
{
    /*
    **  Uninitialize any previously initialized libraries here.
    */

    printf("\nUninitializing each library...\n");

    uninitializeLidar();
    uninitializeCamera();
    uninitializeLineSensors();
    uninitializeGPIO();
}

void start()
{
    /*
    **  This is where we will be calling all start() functions
    */

    startLineSensors();
    startCamera();
    startLidar();
}

void stop()
{
    /*
    **  This is where we will be calling all stop() functions
    */

    // Camera and Lidar have no stop functions
    stopLineSensors();
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
        struct timespec start = currentTime();
        double irError = calculateLineError(senseData.lineSensorData->levels);
        double camError = calculateCameraError(senseData.cameraData->distances);
        double nearestObject = senseData.lidarData->obstacles[0].closestAngle;

        printf("IR Error: %0.2f\tCam Error: %0.2f\n", irError, camError);

        double controlSignal = calculateControlSignal(irError);
        double speed = calculateSpeed(camError);

        applyControlSignal(controlSignal, speed);
        unsigned long elapsed = microSecondsSince(&start);

        microWait((TIMESTEP_MS*1000) - elapsed);
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