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
    initializeMotorHat();

    // Initialize senses
    initializeLineSensors();
    initializeCamera();
    initializeLidar();

    senseData = (SenseData *) malloc(sizeof(SenseData));
    senseData->lineSensorData = getLineSensorDataRef();
    senseData->cameraData = getCameraDataRef();
    senseData->lidarData = getLidarDataRef();

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
    free(senseData);
    senseData = NULL;
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

void printTestStatements()
{
        // Print the line sensor data
        printf("Line Sensors: ");
        for (int i = 0; i < LINESENSORCOUNT; i++)
        {
            printf("%d ", senseData->lineSensorData->levels[i]);
        }
        printf("\n");

        // Print the camera data
        printf("Camera distances: ");
        for (int i = 0; i < CAMSLICES; i++)
        {
            printf("%f ", senseData->cameraData->distances[i]);
        }
        printf("\n");

        // Print the lidar data
        for (int i = 0; i < senseData->lidarData->validObstacles; i++)
        {
            printf("Obstacle %d: angle %.2f, distance %.2f\n", i,
                senseData->lidarData->obstacles[i].closestAngle,
                senseData->lidarData->obstacles[i].closestDistance
                );
        }
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
        makeDecision(senseData);
        unsigned long elapsed = microSecondsSince(&start);
        milliWait(TIMESTEP_MS - (elapsed / 1000));
        // elapsed = microSecondsSince(&start);
        // printf("Elapsed time: %lu\n", elapsed);
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