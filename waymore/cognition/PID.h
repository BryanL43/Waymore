
#ifndef _PID_H_
#define _PID_H_

#include <math.h>
#include "../waymoreLib.h"
#include "../senses/ir.h"
#include "../control/motors.h"

typedef struct PIDGains
{
    double proportional;
    double integral;
    double derivative;
}PIDGains;

void initializePID(int timestepMicroseconds, int speedSetting);

void calculateError(int lineSensorReadings[]);

void calculatePID();

double calculateControlSignal(int lineSensorReadings[]);

void PIDmotorControl();

#endif