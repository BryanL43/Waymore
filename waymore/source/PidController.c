
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

#include "../headers/PidController.h"

#define TIMESTEP_MS 1.0
#define MAXSPEED 1000.0

double lineSensorPositions[LINESENSORCOUNT];
int maxPixelError = CAMWIDTH/2;
PIDGains gain;


void initializePID()
{
    printf("Initializing PID controller...");

    // Configure initial PID gain settings
    gain.proportional = 1.0;
    gain.integral = 0.0;
    gain.derivative = 0.0;

    // Procedurally apply "position" values to each line sensor
    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i)-((double)(LINESENSORCOUNT-1) / 2);
        printf("sensor %d position: %f\n", i+1, lineSensorPositions[i]);
    }

    printf("done.\n");
}

double calculateLineSensorError(int * lineSensorReadings)
{
    double sum = 0.0;
    double activeSensorCount = 0.0;
    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        if (lineSensorReadings[i] == 0)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }

    double error = sum/activeSensorCount;
    return error;
}

double calculateControlSignal(double error)
{
    static double previousError = 0;
    static double integral = 0;

    // Define Proportional term
    double P = gain.proportional * error;

    // Define Integral term
    integral += error * TIMESTEP_MS;
    double I = gain.integral * integral;

    // Define Derivative term
    double derivative = (previousError == 0) ? 0 : (error - previousError) / TIMESTEP_MS;
    double D = gain.derivative * derivative;

    // Update previousError value for next iteration
    previousError = error;

    double pid = P + I + D;

    if (pid < (-1*MAXSPEED)) pid = -1*MAXSPEED;
    if (pid > MAXSPEED) pid = MAXSPEED;

    return pid;
}

int calculateSpeedLimit(int * cameraLineDistances)
{
    double avgDistance = 0.0;
    for (int i=0; i<CAMSLICES; i++)
    {
        avgDistance += cameraLineDistances[i];
    }
    avgDistance = avgDistance/CAMSLICES;
    double percentageOfMax = avgDistance/maxPixelError;
    int speed = (int)(percentageOfMax*10);
    return speed;
}

void applyControlSignal(double controlSignal, int speedLimit)
{
    printf("Control signal: %.2f\n", controlSignal);

    // Initialize both speeds at max
    double speedLeft = speedLimit, speedRight = speedLimit;
    double normControl = speedLimit - (fabs(controlSignal) * speedLimit / MAXSPEED);

    printf("normalized abs val control: %f (double)\n", normControl);

    // If error is greater than zero, we must go LEFT, so slow down the left tire
    if (controlSignal > 0)
    {
        speedLeft = speedLeft - normControl;
    }

    // If error less than zero, we must go RIGHT, so slow down the right tire
    if (controlSignal < 0)
    {
        speedRight = speedRight - normControl;
    }

    int left = speedLeft, right = speedRight;

    printf("L: %d\tR: %d\n", left, right);

    //commandMotors(FORWARD, left, right);
}