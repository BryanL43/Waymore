
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

#define STRAIGHTAHEAD 180   // degrees
#define LINEREFERENCE 0     // unitless

// Speed control
const int maxSpeed = 80;
const int minSpeed = 50;
int baseSpeed = minSpeed;
const int camErrorThresh = 30;

// Obstacle avoidance
const int lassoObstacleDistance = 500; // in 
static int lassoObstacleAngle = 180;

static double               lineSensorPositions[LINESENSORCOUNT] = {};
static PIDGains             gain;

static CurrentAction        currentAction       = LineFollowing;
static LastLineLocation     lastLineLocation    = DeadCenter;
static LassoPath            lassoPath           = LassoRight;
static double               currentIntegral     = 0;

// ============================================================================================= //
// Functions for Brain
// ============================================================================================= //

void initializeCognition()
{
    printf("Initializing Cognitive Functions...");

    gain.proportional   = 25.0;
    gain.integral       = 0.01;
    gain.derivative     = 5.0;

    // Procedurally apply "position" values to each line sensor
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        lineSensorPositions[i] = (double)(i) - ((double)(LINESENSORCOUNT - 1) / 2);
    }

    printf("done.\n");
}

void makeDecision(SenseData * senseData)
{
    /*======================================================================================
    |   Current Action          Obstacle Ahead      Line Below      Next Action
    |=======================================================================================
    |   Following Line          True                True            Avoiding Obstacle
    |   Following Line          True                False           Follow Line (Cornering)
    |   Following Line          False               True            Follow Line (PID)
    |   Following Line          False               False           Follow Line (Cornering)
    |
    |   Avoiding Obstacle       True                True            Avoiding Obstacle
    |   Avoiding Obstacle       True                False           Avoiding Obstacle
    |   Avoiding Obstacle       False               True            Following Line
    |   Avoiding Obstacle       False               False           Lassoing Obstacle
    |
    |   Lassoing Obstacle       True                True            Avoiding Obstacle
    |   Lassoing Obstacle       True                False           Avoiding Obstacle
    |   Lassoing Obstacle       False               True            Following Line
    |   Lassoing Obstacle       False               False           Lassoing Obstacle    
    |
    |   [Cornering states can only be exited by finding the line again]
    |=====================================================================================*/
    
    ObstacleData * obstacle = scanForObstacleAhead(senseData->lidarData);
    double linePosition = calculateLinePosition(senseData->lineSensorData);

    int obstacleAhead = (obstacle != NULL);
    int lineBelow = !isnan(linePosition);

    switch(currentAction)
    {
        case LineFollowing:
            if (obstacleAhead && lineBelow) {
                // We need to avoid the obstacle
                currentAction = AvoidingObstacle;
                printf("Avoiding an Obstacle!\n");
                // Store the current angle to the obstacle
                lassoObstacleAngle = obstacle->closestAngle;
                // Avoid the obstacle
                avoidObstacle(senseData->lidarData);

            } else if (obstacleAhead && !lineBelow) {
                // We've lost the line, may not need to avoid the obstacle
                currentAction = LineCornering;
                lineCorner();

            } else if (!obstacleAhead && lineBelow) {
                // We're still on the line
                currentAction = LineFollowing;
                lineFollow(linePosition, senseData->cameraData);

            } else {
                // We've lost the line!
                currentAction = LineCornering;
                lineCorner();
            }
            break;

        case AvoidingObstacle:
            if (obstacleAhead && lineBelow) {
                // We're still avoiding the obstacle
                currentAction = AvoidingObstacle;
                avoidObstacle(senseData->lidarData);

            } else if (obstacleAhead && !lineBelow) {
                // We're still avoiding the obstacle
                currentAction = AvoidingObstacle;
                avoidObstacle(senseData->lidarData);

            } else if (!obstacleAhead && lineBelow) {
                // No lassoing necessary - we're back on the line
                currentAction = LineFollowing;
                printf("Back on the Line!\n");
                lineFollow(linePosition, senseData->cameraData);

            } else {
                // Enter the lasso orbit
                currentAction = LassoingObstacle;
                printf("Lassoing an Obstacle!\n");
                lassoObstacle(senseData->lidarData);
            }
            break;

        case LassoingObstacle:
            if (obstacleAhead && lineBelow) {
                currentAction = AvoidingObstacle;
                printf("Avoiding an Obstacle!\n");
                // Update the angle to the obstacle
                lassoObstacleAngle = obstacle->closestAngle;
                avoidObstacle(senseData->lidarData);

            } else if (obstacleAhead && !lineBelow) {
                currentAction = AvoidingObstacle;
                perror("WARNING: Avoiding an Obstacle after Lassoing!");

                // Update the angle to the obstacle and start avoiding
                lassoObstacleAngle = obstacle->closestAngle;
                avoidObstacle(senseData->lidarData);

            } else if (!obstacleAhead && lineBelow) {
                currentAction = LineFollowing;
                printf("Back on the Line!\n");
                switch (lassoPath)
                {
                    case LassoRight:
                        commandMotors(ROTATERIGHT, 50, 50);
                        break;
                    case LassoLeft:
                        commandMotors(ROTATELEFT, 50, 50);
                        break;
                    milliWait(50); // To ensure we go the correct direction
                }

            } else {
                currentAction = LassoingObstacle;
                lassoObstacle(senseData->lidarData);
            }
            break;

        case LineCornering:
            if (lineBelow) {
                // We're back on the line!
                currentAction = LineFollowing;
                lineFollow(linePosition, senseData->cameraData);

            } else {
                // Still cornering...
                currentAction = LineCornering;
                lineCorner();
            }
            break;
    }
}

// ============================================================================================= //
// Lidar Related Functions
// ============================================================================================= //

ObstacleData * scanForObstacleAhead(LidarData * lidarData)
{
    ObstacleData * obstacle = NULL;
    double angle = NAN;

    int obstacleIndex = -1;
    for(int i=0; i<lidarData->validObstacles; i++)
    {
        double a = lidarData->obstacles[i].closestAngle;
        double d = lidarData->obstacles[i].closestDistance;

        // Obstacle within 30 degree fan & nearer than 300mm from front of car
        if(a > 165 && a < 195 && d < 500)
        {
            obstacleIndex = i;
            angle = a;
            break;
        }
    }
    // If we have no angle stored by this point, we can return NULL
    // to resume normal operation.
    if (isnan(angle)) return NULL;

    // If we get here, we have an obstacle to avoid..
    // Next we should try to discern the shortest path around the obstacle.
    double diffL = fabs(STRAIGHTAHEAD - lidarData->obstacles[obstacleIndex].leftObstacleAngle);
    double diffR = fabs(STRAIGHTAHEAD - lidarData->obstacles[obstacleIndex].rightObstacleAngle);
    if (diffL <= diffR)
    {
        lassoPath = LassoLeft;
    }
    else
    {
        lassoPath = LassoRight;
    }

    // Return the obstacle we're avoiding
    return &lidarData->obstacles[obstacleIndex];
}

ObstacleData * trackObstacle(LidarData * lidarData)
{
    // Find the obstacle in the lidar data
    for (int i=0; i<lidarData->validObstacles; i++)
    {
        // Fuzzy compare each obstacle's angle with the currently stored lasso angle
        if (fuzzyMatchDouble(lidarData->obstacles[i].closestAngle, lassoObstacleAngle, 15.0))
        {
            // Update the lasso angle for next time and return the obstacle
            lassoObstacleAngle = lidarData->obstacles[i].closestAngle;
            return &lidarData->obstacles[i];
        }
    }

    // If we get here, we have lost the obstacle
    perror("WARNING: Lost track of the obstacle!");
    return NULL;
}

// ============================================================================================= //
// Line Sensor Related Functions
// ============================================================================================= //

double calculateLinePosition(LineSensorData * lineData)
{
    double sum = 0.0;
    int activeSensorCount = 0;
    for (int i = 0; i < LINESENSORCOUNT; i++)
    {
        if (lineData->levels[i] == TRUE)
        {
            sum += lineSensorPositions[i];
            activeSensorCount++;
        }
    }
    if (activeSensorCount == 0) return NAN;
    double position = sum / activeSensorCount;

    // Interpret which side of the line we are currently on
    if      (position < 0)  lastLineLocation = LeftOfCar;
    else if (position > 0)  lastLineLocation = RightOfCar;
    else                    lastLineLocation = DeadCenter;

    return position;
}

// ============================================================================================= //
// Camera Related Functions
// ============================================================================================= //

double calculateCameraError(CameraData * cameraData)
{
    double sum = 0.0;
    for (int i=0; i<CAMSLICES; i++)
    {
        if(isnan(cameraData->distances[i])) return NAN;
        sum += (i+1)/CAMSLICES*cameraData->distances[i];
    }
    return sum;
}

double calculateBaseSpeed(double cameraError)
{
    double speed = minSpeed;
    int isWithinBounds = (cameraError < camErrorThresh) && (cameraError > (-1*camErrorThresh));
    if (!isnan(cameraError) && isWithinBounds)
    {
        speed = maxSpeed;
    }
    return speed;
}

// ============================================================================================= //
// PID Error Calculation
// ============================================================================================= //

double calculateError(double measurement, double reference)
{
    static double previousMeasurement = 0.0;

    // Classic Formula for Control System Error:
    //      Error = reference - feedback
    //
    // We are using a PID controller, so we will be calculating
    // our feedback as the sum of the Proportional, Integral, and Derivative
    // components of the feedback, and then subtracting that from the reference
    // to get our error from the desired value or reference.

    // Catch NaN values before calculations
    if (isnan(measurement)) return NAN;

    // Calculate P
    double P = gain.proportional * measurement;

    // Calculate I
    currentIntegral += measurement * TIMESTEP_MS;
    double I = gain.integral * currentIntegral;

    // Calculate D
    double D = gain.derivative * (measurement - previousMeasurement) / TIMESTEP_MS;

    // Update previous readings
    previousMeasurement = measurement;

    // Calculate the error
    double error = reference - (P + I + D);
    return error;
}

// ============================================================================================= //
// Enumerated Control Functions
// ============================================================================================= //

void lineFollow(double linePosition, CameraData * cameraData)
{
    double cameraError = calculateCameraError(cameraData);
    double speed = calculateBaseSpeed(cameraError);
    double error = calculateError(linePosition, LINEREFERENCE);

    // Calculate speeds
    double speedLeft = speed + error;
    double speedRight = speed - error;

    // convert to ints, with rounding
    int left = (int)(speedLeft + 0.5);
    int right = (int)(speedRight + 0.5);

    // Send power to the wheels
    commandMotors(FORWARD, left, right);
}

void lassoObstacle(LidarData * lidarData)
{
    // Update what we know about the obstacle
    ObstacleData * obstacle = trackObstacle(lidarData);

    // Calculate the error
    double error = calculateError(obstacle->closestDistance, lassoObstacleDistance);

    // Calculate speeds
    double speedLeft = baseSpeed + error;
    double speedRight = baseSpeed - error;

    // convert to ints, with rounding
    int left = (int)(speedLeft + 0.5);
    int right = (int)(speedRight + 0.5);

    // Send power to the wheels
    commandMotors(FORWARD, left, right);
}

void lineCorner()
{
    if (lastLineLocation == LeftOfCar)
        commandMotors(ROTATELEFT, 50, 50);
    else
        commandMotors(ROTATERIGHT, 50, 50);
}

void avoidObstacle(LidarData * lidarData)
{
    // Update what we know about the obstacle
    trackObstacle(lidarData);

    switch(lassoPath)
    {
        case LassoRight:
            commandMotors(FORWARD, 75, 25);
            break;
        case LassoLeft:
            commandMotors(FORWARD, 25, 75);
            break;
    }
}

// ============================================================================================= //
// End of File
// ============================================================================================= //