
#ifndef _PID_H_
#define _PID_H_

#include <math.h>
#include "../waymoreLib.h"
#include "../senses/ir.h"
#include "../motors/motors.h"

typedef struct PIDGains
{
    double proportional;
    double integral;
    double derivative;
}PIDGains;

double sensorPositions[LINESENSORCOUNT];
double timestep;
double controlSignal = 0;
int speedLimit;
PIDGains gain;

void initializePID(int timestepMicroseconds, int speedSetting)
{
    // Configure timestep duration and speed
    timestep = timestepMicroseconds/1e6;
    speedLimit = speedSetting;

    // Configure initial PID gain settings
    gain.proportional = 0.20;
    gain.integral = 1/speedSetting; // this seems to work for speeds up to 50%
    gain.derivative = 0.10;

    // Procedurally apply "position" values to each sensor
    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        sensorPositions[i] = (double)(i)-((double)(LINESENSORCOUNT-1) / 2);
        printf("sensor %d position: %f\n", i+1, sensorPositions[i]);
    }
}

double calculateError(int lineSensorReadings[])
{
    double weightedSum = 0;
    int activeSensors = 0;

    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        if(lineSensorReadings[i] == 1)
        {
            weightedSum += sensorPositions[i];
            activeSensors++;
        }
    }

    if (activeSensors == 0)
    {
        return NAN;
    }

    return weightedSum / activeSensors;
}

double calculatePID(double error)
{
    static double previousError = 0;
    static double integral = 0;

    // Define Proportional term
    double P = gain.proportional * error;

    // Define Integral term
    if (error == 0)
    {
        integral = 0;
    }else{
        integral += error * timestep;
    }
    double I = gain.integral * integral;

    // Define Derivative term
    double derivative = (previousError == 0) ? 0 : (error - previousError) / timestep;
    double D = gain.derivative * derivative;

    // Update previousError value for next iteration
    previousError = error;

    return P+I+D;
}

double getControlSignal(int lineSensorReadings[])
{
    // Calculate error
    double error = calculateError(lineSensorReadings);
    if (isnan(error))
    {
        return controlSignal;
    }

    // Store the latest (valid) signal
    controlSignal = calculatePID(error);

    // Return the latest signal
    return controlSignal;
}

void PIDmotorControl(double controlSignal)
{
    // First filter out signals greater than |1|
    if(controlSignal > 1) controlSignal = 1;
    else if(controlSignal < -1) controlSignal = -1;

    // Initialize speeds at speed limit
    int speedLeft = speedLimit;
    int speedRight = speedLimit;

    // Slow down one wheel's speed
	if (controlSignal < 0)
        // If signal is less than zero, add to 1
        speedLeft -= speedLimit * (1 + controlSignal);
	else if (controlSignal > 0)
        // If signal is greater than zero, sub from 1
        speedRight -=  speedLimit * (1 - controlSignal);

    // Call the motor control library function
    moveForward(speedLeft, speedRight);
}

#endif