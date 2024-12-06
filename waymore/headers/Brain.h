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

#define TIMESTEP_MS 5.0
#define CAMWIDTH 640
#define CAMHEIGHT 480
#define CAMSLICES 4

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct SensoryData
{
    // Obstacle and Line Sensor counts and readings
    int * lineSensorReadings;

    // Camera Readings
    int * cameraLineDistances;
    double * cameraLineConfidences;

    // Lidar Readings
    //...
} SensoryData;

// ============================================================================================= //
// Signals and Controls
// ============================================================================================= //

void signalHandler(int sig);

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initialize();

void uninitialize();

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