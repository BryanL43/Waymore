
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
//static  LinePrediction      linePrediction;
static  LastLineLocation    lastLineLocation;
static  CurrentState        currentState;

static double currentIntegral = 0;

void initializePID()
{
    printf("Initializing PID controller:\n");

    gain.proportional = 25.0;
    gain.integral = 0.5;
    gain.derivative = 1.0;

    lastLineLocation = DEADCENTER;
    currentState = NORMAL;

    // Procedurally apply "position" values to each line sensor
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i) - ((double)(LINESENSORCOUNT - 1) / 2);
        printf("sensor %d position: %.2f\n", i + 1, lineSensorPositions[i]);
    }
    printf("done.\n");
}

double calculateError(int * lineSensorReadings)
{
    double sum = 0.0;
    int activeSensorCount = 0;
    //printf("Line readings: ");
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        //printf(" %d", lineSensorReadings[i]);
        if (lineSensorReadings[i] == TRUE)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }
    //printf("\n");

    // Return "not a number" in cases where completely off the line.
    if (activeSensorCount == 0) return NAN;

    double error = sum / activeSensorCount;

    // Interpret which side of the line we are currently on
    if (error < 0){
        lastLineLocation = LEFTOFCAR;
        //printf("Line is to the left\n");
    }
    else if (error > 0)
    {
        lastLineLocation = RIGHTOFCAR;
        //printf("Line is to the right\n");
    }
    else
    {
        lastLineLocation = DEADCENTER;
        //printf("Line is dead center\n");
    }

    //printf("error: %.2f\n", error);
    
    return error;
}

void interpretCameraDistances(double * cameraLineDistances)
{
    // TODO: use camera distances to decide when to enter/exit enumerated states
    return;
}

double validateError(double error)
{
    int isnan = isnan(error);
    double verr = error;

    switch (lastLineLocation)
    {
        case LEFTOFCAR:
            //printf("Line is to the left\n");

            // If the line was last seen to the left of the car,
            // We should have a positive number for an error.
            // If we don't, we should change that.
            if (isnan) currentState = CORNERINGLEFT;
            else currentState = NORMAL;
            break;

        case RIGHTOFCAR:
            //printf("Line is to the right\n");
            if(isnan) currentState = CORNERINGRIGHT;
            else currentState = NORMAL;
            
            break;

        case DEADCENTER:
            //printf("Line is dead center\n");

            // If the car was last seen dead center on the line,
            // We should probably not have any integral left hanging around.
            // We should also set the state to NORMAL unless we've entered
            // obstacle avoidance mode.
            currentIntegral = 0;
            if(currentState != OBSTACLEAVOIDANCE) currentState = NORMAL;

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
    double I = gain.integral * currentIntegral;

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
    switch (currentState)
    {
        case NORMAL:
            // Calculate speeds
            double speedLeft = BASESPEED + controlSignal;
            double speedRight = BASESPEED - controlSignal;

            // convert to ints, with rounding
            int left = (int)(speedLeft + 0.5);
            int right = (int)(speedRight + 0.5);

            // Print & check:
            //printf("L: %d\tR: %d\n", left, right);

            // Send power to the wheels
            commandMotors(FORWARD, left, right);
            break;

        case CORNERINGLEFT:
            commandMotors(ROTATELEFT, 50, 50);
            break;
        
        case CORNERINGRIGHT:
            commandMotors(ROTATERIGHT, 50, 50);
            break;

        case OBSTACLEAVOIDANCE:
            // Do stuff
            break;
    }
}