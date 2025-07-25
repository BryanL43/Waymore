/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: LineSensors.h
*
* Description:: Library of functions and wrappers for ir
*		        sensor functionality to be called by brain.c
*
**************************************************************/

#ifndef _LINE_SENSORS_H_
#define _LINE_SENSORS_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include "WaymoreLib.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

// Counts for each sensor type currently on rig
#define LINESENSORCOUNT 7

// ============================================================================================= //
// Definitions of Structs
// ============================================================================================= //

typedef struct LineSensorData{
    int levels[LINESENSORCOUNT];
} LineSensorData;

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

void initializeLineSensors();
void uninitializeLineSensors();

// ============================================================================================= //
// Start and Stop Functions
// ============================================================================================= //

void startLineSensors();
void stopLineSensors();

// ============================================================================================= //
// Functions for external use
// ============================================================================================= //

LineSensorData * getLineSensorDataRef();
void refreshLineSensorData();

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif