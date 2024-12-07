
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
static int previousError = 0;

void initializePID()
{
    printf("Initializing PID controller...");

    // PID FOR STRAIGHT LINE
    // gain.proportional = 120.0;
    // gain.integral = 0.1;
    // gain.derivative = 7;

    // PID FOR CURVE LINE
    // gain.proportional = 220.0;
    // gain.integral = 2;
    // gain.derivative = 2;

    // PID FOR SHARP TURN
    // gain.proportional = 10000.0;
    // gain.integral = 2;
    // gain.derivative = 2;

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
    printf("**************** [ camera distances ] ****************\n");
    for (int i = 0; i < CAMSLICES; i++)
    {
        printf("Camera slice: %d; value: %d\n", i, cameraLineDistances[i]);
        sum += (double)cameraLineDistances[i];
    }
    printf("**************** [ end of camera distances ] ****************\n");
    return sum / (double)CAMSLICES;
}

double calculateControlSignal(double IRError, double cameraError)
{
    static double integral = 0;

    // cameraError = fabs(cameraError);
    // if (cameraError > 0 && cameraError <= 60) {
    //     previousError = 0;
    //     gain.derivative = 5;
    //     gain.proportional = 90;
    // } else if (cameraError > 60 && cameraError <= 120) {
    //     previousError = 0;
    //     gain.proportional = 150;
    //     gain.derivative = 3;
    //     gain.integral = 2;
    // } else if (cameraError > 180) {
    //     previousError = 0;
    //     gain.proportional = 500;
    //     gain.derivative = 3;
    //     gain.integral = 3;
    // }

    double P = gain.proportional * IRError;

    // Integral term with anti-windup
    integral += IRError * TIMESTEP_MS;
    double max_integral = 100.0;
    if (integral > max_integral)
        integral = max_integral;
    if (integral < -max_integral)
        integral = -max_integral;
    double I = gain.integral * integral;

    // Derivative term with smoothing
    static double smoothedError = 0;
    double alpha = 0.1;
    smoothedError = alpha * IRError + (1 - alpha) * smoothedError;
    double D = gain.derivative * (smoothedError - previousError) / TIMESTEP_MS;

    // Update previous error
    previousError = smoothedError;

    // PID output clamping
    double pid = P + I + D;
    double max_pid = 255.0;
    if (pid > max_pid)
        pid = max_pid;
    if (pid < -max_pid)
        pid = -max_pid;

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
    int left = speedLeft;
    int right = speedRight;
    // int left = 325, right = 300; // Motor not going straight :((

    // Finalized Control Signal
    printf("L: %d\tR: %d\n", left, right);

    commandMotors(FORWARD, 300, 0);
}