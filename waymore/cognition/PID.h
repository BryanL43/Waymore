
#ifndef _PID_H_
#define _PID_H_

#include <math.h>
#include "../waymoreLib.h"
#include "../senses/ir.h"

typedef struct PIDGains
{
    double proportional;
    double integral;
    double derivative;
}PIDGains;

double sensorPositions[LINESENSORCOUNT];
double timestep;
PIDGains gain;

void initializePID(int timestepMicroseconds)
{
    // Configure timestep duration
    timestep = timestepMicroseconds/1e6;

    // Configure initial PID gain settings
    gain.proportional = 5.0;
    gain.integral = 3.0;
    gain.derivative = 5.0;

    // Procedurally apply "position" values to each sensor
    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        sensorPositions[i] = (double)(i)-((double)(LINESENSORCOUNT-1) / 2);
    }
}

double getError(int lineSensorReadings[])
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

double getPID(double error)
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

double getControlSignal(int lineSensorReadings[])
{
    // Store latest signal in case of gaps or errors
    static double latestSignal = 0;

    // Calculate error
    double error = getError(lineSensorReadings);
    if (isnan(error))
    {
        return latestSignal;
    }

    // Store the latest (valid) signal
    latestSignal = getPID(error);

    // Return the latest signal
    return latestSignal;
}

#endif