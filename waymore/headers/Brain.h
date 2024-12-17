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

// Include gpio & other basic functionality
#include "WaymoreLib.h"

// Include senses
#include "LineSensors.h"
//#include "Camera.h"
#include "Lidar.h"
#include "RgbSensor.h"

// Include cognitive functions
#include "Cognition.h"

// Include motor control
#include "MotorControl.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define TIMESTEP_MS 1 // 1000 Hz control loop

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct SenseData
{
    LineSensorData * lineSensorData;
    //CameraData * cameraData;
    LidarData * lidarData;
    HSVData * hsvData;
}SenseData;

// ============================================================================================= //
// Signals and Controls
// ============================================================================================= //

void signalHandler(int sig);

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initialize();

void uninitialize();

void start();

void stop();

// ============================================================================================= //
// Main Loop & Business Logic
// ============================================================================================= //

void mainLoop();

// ============================================================================================= //
// End of File
// ============================================================================================= //

#endif