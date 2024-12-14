
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

const int maxSpeed = 100;
const int minSpeed = 50;
int baseSpeed = minSpeed;

const int camErrorThresh = 80;

static  double              lineSensorPositions[LINESENSORCOUNT] = {};
static  PIDGains            gain;
static  LastLineLocation    lastLineLocation;
static  CurrentState        currentState;

static double currentIntegral = 0;

const int lineErrorBufferLength = 5;
RingBuffer * lineErrorBuffer;

void initializeCognition()
{
    printf("Initializing Cognitive Functions...");

    gain.proportional = 30.0;
    gain.integral = 0.01;
    gain.derivative = 5.0;

    lastLineLocation = DEADCENTER;
    currentState = NORMAL;

    // Procedurally apply "position" values to each line sensor
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i) - ((double)(LINESENSORCOUNT - 1) / 2);
    }

    // Initialize the ring buffer
    lineErrorBuffer = newRingBuffer(lineErrorBufferLength);

    printf("done.\n");
}

double calculateLineError(int * lineSensorLevels)
{
    double sum = 0.0;
    int activeSensorCount = 0;
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        if (lineSensorLevels[i] == TRUE)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }

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
    
    pushRingBuffer(lineErrorBuffer, error);
 
    //double mean = getMeanRingBuffer(lineErrorBuffer);
    double median = getMedianRingBuffer(lineErrorBuffer);

    return median;
}

double calculateCameraError(double * cameraLineDistance)
{
    double sum = 0.0;
    for (int i=0; i<CAMSLICES; i++)
    {
        if(isnan(cameraLineDistance[i])) return NAN;
        sum += (i+1)/CAMSLICES*cameraLineDistance[i];
    }
    return sum;
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

double calculateSpeed(double cameraError)
{
    double speed = minSpeed;
    int isWithinBounds = (cameraError < camErrorThresh) && (cameraError > (-1*camErrorThresh));
    if (!isnan(cameraError) && isWithinBounds)
    {
        speed = maxSpeed;
    }
    return speed;
}

void applyControlSignal(double controlSignal, double speed)
{
    switch (currentState)
    {
        case NORMAL:
            // Calculate speeds
            double speedLeft = speed + controlSignal;
            double speedRight = speed - controlSignal;

            // convert to ints, with rounding
            int left = (int)(speedLeft + 0.5);
            int right = (int)(speedRight + 0.5);

            // Send power to the wheels
            commandMotors(FORWARD, left, right);
            break;

        case CORNERINGLEFT:
            commandMotors(ROTATELEFT, 60, 40);
            break;
        
        case CORNERINGRIGHT:
            commandMotors(ROTATERIGHT, 40, 60);
            break;

        case OBSTACLEAVOIDANCE:
            // Do stuff
            break;
    }
}

void uninitializeCognition()
{
    destroyRingBuffer(lineErrorBuffer);
}