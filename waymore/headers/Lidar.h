/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: Lidar.h
*
* Description:: Wrapper API for C++ Lidar functions in C.
*
**************************************************************/

#ifndef _LIDAR_H_
#define _LIDAR_H_

#include "WaymoreLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAXOBSTACLES 5

typedef struct {
    double closestAngle;
    double closestDistance;
    double leftObstacleAngle;
    double rightObstacleAngle;
} ObstacleData;

typedef struct {
    ObstacleData obstacles[MAXOBSTACLES];
    int validObstacles;
} LidarData;

typedef void Lidar; // Intermediate for C compatibility

Lidar * initializeLidar();
void startLidar();
LidarData * getLidarDataRef();
void uninitializeLidar();

#ifdef __cplusplus
}
#endif

#endif