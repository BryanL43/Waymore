/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: brain.h
*
* Description:: Declarations for Waymore's brain
*
**************************************************************/

#ifndef _BRAIN_H_
#define _BRAIN_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include <stdio.h>

// Include gpio & other basic functionality
#include "WaymoreLib.h"

// Include senses
#include "LineSensors.h"
#include "Camera.h"

// Include cognitive functions
#include "PidController.h"

// Include motor control
#include "MotorControl.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

// Change to modulate the wait time between each cycle
#define TIMESTEPMICROSECONDS 100000

// Change to set the speed of the vehicle, 0-100
#define SPEEDSETTING 33

// Set Camera resolution
#define CAMWIDTH 640
#define CAMHEIGHT 480
#define CAMSLICES 3

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct SensoryData
{
    // Obstacle and Line Sensor counts and readings
    int * lineReadings;

    // Camera Readings
    int * cameraLineDistances;

    // Lidar Readings
    //...
} SensoryData;

typedef enum PrioritizedSense
{
    CAMERA,
    LIDAR
}PrioritizedSense;

// ============================================================================================= //
// Signals and Controls
// ============================================================================================= //

void signalHandler(int sig);

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initializeLibraries();

void uninitializeLibraries();

void startSenses();

void stopSenses();

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void mainLoop();

// ============================================================================================= //
// End of File
// ============================================================================================= //

#endif