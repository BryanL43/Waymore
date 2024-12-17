
/**************************************************************
 * Class:: CSC-615-01 Spring 2024
 * Name:: Waymore Team
 * Student ID:: ...
 * Github-Name:: ...
 * Project:: Final Project
 *
 * File:: irPID.c
 *
 * Description:: Implementations for PID controller
 *				functionality
 *
 **************************************************************/

#include "../headers/Cognition.h"

#define STRAIGHTAHEAD 180   // degrees
#define LINEREFERENCE 0     // unitless

// Speed control
int baseSpeed = 30; // it was 90
// const int camErrorThresh = 30;

// Obstacle avoidance
const double obstacleAvoidanceDistance = 800.0;
const double lassoObstacleDistance = 500.0;
const double fanDegrees = 30.0;

static double               lineSensorPositions[LINESENSORCOUNT] = {};
static PIDGains             gain;
static LastLineLocation     lastLineLocation    = DeadCenter;
static LassoPath            lassoPath           = LassoRight;
static double               currentIntegral     = 0;

// ============================================================================================= //
// Functions for Brain
// ============================================================================================= //

void initializeCognition()
{
    printf("Initializing Cognitive Functions...");

    gain.proportional   = 30.0;
    gain.integral       = 0.50;
    gain.derivative     = 10.0;

    // Procedurally apply "position" values to each line sensor
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i) - ((double)(LINESENSORCOUNT - 1) / 2);
    }

    printf("done.\n");
}

// ============================================================================================= //
// Line Sensor Related Functions
// ============================================================================================= //

double calculateLinePosition(LineSensorData * lineData)
{
    refreshLineSensorData();

    double sum = 0.0;
    int activeSensorCount = 0;
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        if (lineData->levels[i] == TRUE)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }
    if (activeSensorCount == 0) return NAN;
    double position = sum / activeSensorCount;

    // Interpret which side of the line we are currently on
    if      (position < 0)  lastLineLocation = LeftOfCar;
    else if (position > 0)  lastLineLocation = RightOfCar;
    else                    lastLineLocation = DeadCenter;

    return position;
}

// ============================================================================================= //
// Camera Related Functions
// ============================================================================================= //

// double calculateCameraError(CameraData * cameraData)
// {
//     double sum = 0.0;
//     for (int i=0; i<CAMSLICES; i++)
//     {
//         if(isnan(cameraData->distances[i])) return NAN;
//         sum += (i+1)/CAMSLICES*cameraData->distances[i];
//     }
//     return sum;
// }

// ============================================================================================= //
// PID Error Calculation
// ============================================================================================= //

double calculateError(double measurement, double reference)
{
    static double previousMeasurement = 0.0;

    // Classic Formula for Control System Error:
    //      Error = reference - feedback
    //
    // We are using a PID controller, so we will be calculating
    // our feedback as the sum of the Proportional, Integral, and Derivative
    // components of the feedback, and then subtracting that from the reference
    // to get our error from the desired value or reference.

    // Catch NaN values before calculations
    if (isnan(measurement)) return NAN;

    // Calculate P
    double P = gain.proportional * measurement;

    // Calculate I
    currentIntegral += measurement * TIMESTEP_MS;
    double I = gain.integral * currentIntegral;

    // Calculate D
    double D = gain.derivative * (measurement - previousMeasurement) / TIMESTEP_MS;

    // Update previous readings
    previousMeasurement = measurement;

    // Calculate the error
    double error = reference - (P + I + D);
    return error;
}

// ============================================================================================= //
// Enumerated Control Functions
// ============================================================================================= //

void lineFollow(double linePosition) //, CameraData * cameraData
{
    //double cameraError = calculateCameraError(cameraData);
    //double speed = calculateBaseSpeed(cameraError);
    double error = calculateError(linePosition, LINEREFERENCE);

    // Calculate speeds
    double speedLeft = baseSpeed - error;
    double speedRight = baseSpeed + error;

    // convert to ints, with rounding
    int left = (int)(speedLeft + 0.5);
    int right = (int)(speedRight + 0.5);

    // Send power to the wheels
    commandMotors(FORWARD, left, right);
}

void lineCorner()
{
    if (lastLineLocation == LeftOfCar)
        commandMotors(ROTATELEFT, 50, 50);
    else
        commandMotors(ROTATERIGHT, 50, 50);
}

void avoidObstacle(LidarData * lidarData)
{
    switch(lassoPath)
    {
        case LassoRight:
            commandMotors(FORWARD, 30, 20);
            break;
        case LassoLeft:
            commandMotors(FORWARD, 20, 30);
            break;
    }
}

// ============================================================================================= //
// End of File
// ============================================================================================= //