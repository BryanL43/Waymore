
#include "PID.h"

double sensorPositions[LINESENSORCOUNT];
double timestep;
int speedLimit;
int speed;
PIDGains gain;

volatile double currentSignal;
volatile double currentError;

void initializePID(int timestepMicroseconds, int speedSetting)
{
    // Configure timestep duration and speed
    timestep = timestepMicroseconds/1e6;
    speedLimit = speedSetting;

    // Configure initial PID gain settings
    gain.proportional = 1;
    gain.integral = 0;
    gain.derivative = 0;

    // Procedurally apply "position" values to each sensor
    for (int i=0; i<LINESENSORCOUNT; i++)
    {
        sensorPositions[i] = (double)(i)-((double)(LINESENSORCOUNT-1) / 2);
        printf("sensor %d position: %f\n", i+1, sensorPositions[i]);
    }
}

void calculateError(int lineSensorReadings[])
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
        currentError = NAN;
    }

    currentError = weightedSum / activeSensors;
}

void calculatePID()
{
    static double previousError = 0;
    static double integral = 0;

    double error = isnan(currentError) ? 0 : currentError;

    // Define Proportional term
    double P = gain.proportional * error;
    P = isnan(error) ? P * 10 : P;

    // Define Integral term
    integral += error * timestep;
    double I = gain.integral * integral;

    // Define Derivative term
    double derivative = (previousError == 0) ? 0 : (error - previousError) / timestep;
    double D = gain.derivative * derivative;

    // Update previousError value for next iteration
    previousError = error;

    currentSignal = P+I+D;
}

double calculateControlSignal(int lineSensorReadings[])
{
    // Calculate error
    calculateError(lineSensorReadings);

    // Store the latest (valid) signal
    calculatePID();
}

void PIDmotorControl()
{
    double e = fabs(currentSignal);

    double speed = speedLimit/(1 - fabs(currentSignal));
    int speedLeft = speedLimit + currentSignal;
    int speedRight = speedLimit - currentSignal;

    printf("| c %f | l %d | r %d |\n", currentSignal, speedLeft, speedRight);
    // Call the motor control library function
    moveForward(speedLeft, speedRight);
}