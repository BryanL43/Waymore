
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

#define BASESPEED 50
#define MAXSPEED 100

int maxPixelDist = CAMWIDTH / 2;

static  double              lineSensorPositions[LINESENSORCOUNT];
static  PIDGains            gain;
static  LinePrediction      linePrediction;
static  LastLineLocation    lastLineLocation;

static double currentIntegral = 0;

void initializePID()
{
    printf("Initializing PID controller...");

    gain.proportional = 1.0;
    gain.currentIntegral = 0.05;
    gain.derivative = 0.10;

    lineLocation = DEADCENTER;

    // Procedurally apply "position" values to each line sensor
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i) - ((double)(LINESENSORCOUNT - 1) / 2);
        printf("sensor %d position: %f\n", i + 1, lineSensorPositions[i]);
    }
    printf("done.\n");
}

double calculateError(int * lineSensorReadings)
{
    double sum = 0.0;
    int activeSensorCount = 0;
    // printf("Line readings: ");
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        // printf(" %d", lineSensorReadings[i]);
        if (lineSensorReadings[i] == 0)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }
    // printf("\n");

    // Return "not a number" in cases where completely off the line.
    if (activeSensorCount == 0) return NAN;

    double error = sum / activeSensorCount;

    // Interpret which side of the line we are currently on
    if      (error > 0) lineLocation = LEFTOFCAR;
    else if (error < 0) lineLocation = RIGHTOFCAR;
    else                lineLocation = DEADCENTER;

    return error;
}

void interpretCameraDistances(double * cameraLineDistances)
{
    // TODO: use camera distances to decide when to enter/exit enumerated states
    return;
}

double validateError(double error)
{
    double verr = 0.0;

    switch (lastLineLocation)
    {
        case LEFTOFCAR:
            // If the line was last seen to the left of the car,
            // We should have a positive number for an error.
            // If we don't, we should change that.
            if (isnan(error)) verr = 1;

            // If the current integral is going in the
            // wrong direction, zero it out.
            if (currentIntegral < 0) currentIntegral = 0;

            // Now we've made sure all signals will be going in
            // the leftwards direction.
            break;

        case RIGHTOFCAR:
            // If the line was last seen to the right of the car,
            // We should have a negative number for an error.
            // If we don't, we should change that.
            if(isnan(error)) verr = -1;

            // If the current integral is going in the
            // wrong direction, zero it out.
            if (currentIntegral > 0) currentIntegral = 0;

            // Now we've made sure all signals will be going
            // in the rightwards direction.
            break;

        case DEADCENTER:
            // If the car was last seen dead center on the line,
            // We should probably not have any integral left hanging around.
            currentIntegral = 0;

            // Also - if the current error is NAN, we're probably done..
            // So we can return the NAN value as a stop signal.
            if(isnan(error)) verr = NAN;
            break;
    }

    return verr;
}

double calculateControlSignal(double error)
{
    static int previousError = 0;

    error = validateError(error);
    if (isnan(error)) return error;

    // Calculate P
    double P = gain.proportional * error;

    // Calculate I
    currentIntegral += error * TIMESTEP_MS;
    double I = gain.currentIntegral * currentIntegral;

    // Calculate D
    double D = gain.derivative * (error - previousError) / TIMESTEP_MS;

    // Update previous error
    previousError = error;

    return P + I + D;
}


int calculateSpeedLimit(double * cameraLineDistances)
{
    // Develop only after completing a working PID and state machine!

    int speed = BASESPEED;
    // TODO: use a case switch with enumerated states to set speeds

    return speed;
}

void applyControlSignal(double controlSignal)
{
    // If the signal is nan, that's a stop flag.
    if (isnan(controlSignal)) 
    {
        commandMotors(HALT, 0, 0);
    }

    // Initial control signal:
    printf("Control Signal: %.2f\n", controlSignal);

    // Calculate speeds:
    double speedLeft = BASESPEED - controlSignal;
    double speedRight = BASESPEED + controlSignal;

    // Calculate integers with rounding:
    int left = (int)(speedLeft + 0.5);
    int right = (int)(speedRight + 0.5);

    // Print & check:
    printf("L: %d\tR: %d\n", left, right);

    // Send power to the wheels
    commandMotors(FORWARD, left, right);
}