
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

double lineSensorPositions[LINESENSORCOUNT];

PIDGains gain;
double timestep;

void initializePID()
{
    printf("Initializing PID controller...");

    // Configure timestep duration and speed
    timestep = TIMESTEPMICROSECONDS/1e6;

    // Configure initial PID gain settings
    gain.proportional = 1;
    gain.integral = 0;
    gain.derivative = 0;

    // Procedurally apply "position" values to each line sensor
    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i)-((double)(LINESENSORCOUNT-1) / 2);
        printf("sensor %d position: %f\n", i+1, lineSensorPositions[i]);
    }

    printf("done.\n");
}

double calculateControlSignal(double error)
{
    static double previousError = 0;
    static double integral = 0;

    // Define Proportional term
    double P = gain.proportional * error;

    // Define Integral term
    integral += error * timestep;
    double I = gain.integral * integral;

    // Define Derivative term
    double derivative = (previousError == 0) ? 0 : (error - previousError) / timestep;
    double D = gain.derivative * derivative;

    // Update previousError value for next iteration
    previousError = error;

    return P+I+D;
}

void PIDmotorControl(int controlSignal)
{
    // Initialize both speeds at max
    int speedLeft = SPEEDSETTING, speedRight = SPEEDSETTING;

    // Normalize the control signal to be between 0 and speedsetting
    double max = ((double)CAMWIDTH)/2, speedlim = (double)SPEEDSETTING;
    double normControl = fabs(controlSignal)/max*speedlim;

    printf("normalized abs val control: %f (float) | %d (int)\n",
            normControl, (int)normControl);

    // If error is greater than zero, we must go LEFT, so slow down the left tire
    if (controlSignal > 0)
    {
        speedLeft = speedLeft - (int)normControl;
    }

    // If error less than zero, we must go RIGHT, so slow down the right tire
    if (controlSignal < 0)
    {
        speedRight = speedRight - (int)normControl;
    }

    printf("L: %d\tR: %d\n", speedLeft, speedRight);

    moveForward(speedLeft, speedRight);
}