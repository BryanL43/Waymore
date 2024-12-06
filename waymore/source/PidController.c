
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

#define MAXSPEED 300.0

double lineSensorPositions[LINESENSORCOUNT];
int maxPixelDist = CAMWIDTH / 2;
PIDGains gain;

void initializePID()
{
    printf("Initializing PID controller...");

    // Configure initial PID gain settings
    gain.proportional = 100.0;
    gain.integral = 0.1;
    gain.derivative = 0.5;

    // Procedurally apply "position" values to each line sensor
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i) - ((double)(LINESENSORCOUNT - 1) / 2);
        printf("sensor %d position: %f\n", i + 1, lineSensorPositions[i]);
    }
    printf("done.\n");
}

double calculateLineSensorError(int *lineSensorReadings)
{
    double sum = 0.0;
    double activeSensorCount = 0.0;
    // printf("Line sensor values:");
    printf("----------------------------\n");
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        // printf(" %d", lineSensorReadings[i]);
        if (lineSensorReadings[i] == 0)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }

    double error = (activeSensorCount == 0.0) ? 0.0 : (sum / activeSensorCount);
    return error;
}

double calculateCameraError(int *cameraLineDistances)
{
    double sum = 0.0;
    for (int i = 0; i < CAMSLICES; i++)
    {
        sum += (double)cameraLineDistances[i];
    }
    return sum / (double)CAMSLICES;
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

    return pid;
}

int calculateSpeedLimit(double *cameraLineConfidences)
{
    double avgConf = 0.0;
    // printf("Line Weights: ");
    for (int i = 0; i < CAMSLICES; i++)
    {
        // printf(" %.2f", cameraLineConfidences[i]);
        avgConf += cameraLineConfidences[i];
    }
    // printf("\n");

    // Calculate average distance
    avgConf = fabs(avgConf) / (double)CAMSLICES;

    int speed = (double)MAXSPEED * avgConf;

    return speed;
}

void applyControlSignal(double controlSignal, int speedLimit)
{
    // Initial control signal:
    printf("Control Signal: %.2f\n", controlSignal);

    // Initial speeds:
    double speedLeft = MAXSPEED, speedRight = MAXSPEED;
    printf("Speeds: L %.2f\tR %.2f\n", speedLeft, speedRight);

    // Normalized signal to 0-1000:
    // double normControl = (fabs(controlSignal) * (double)speedLimit / MAXSPEED);
    double normControl = (fabs(controlSignal));
    printf("Normalized Control Signal: %.2f\n", normControl);

    // APPLY CONTROL SIGNAL:
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

    // Finalized Control Signal
    printf("L: %d\tR: %d\n", left, right);

    commandMotors(FORWARD, left, right);
}