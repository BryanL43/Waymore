#ifndef _LIDARSENSOR_H_
#define _LIDARSENSOR_H_

#include "WaymoreLib.h" // Grant access to gpio pin functionality
#include "sl_lidar.h" 
#include "sl_lidar_driver.h"
#include "Lidar.h"

#include <iostream>
#include <thread>
#include <atomic>

#define MAXOBSTACLES 5
#define DISTANCETHRESH 50

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

using namespace sl;

class LidarSensor {
public:
    LidarSensor(const std::string& device, int baudrate, int MOTOCTL_GPIO);
    void startLidar();
    double (*getLidarData())[4];
    int getValidObstacles();
    ~LidarSensor();
    
private:
    std::atomic<bool> shuttingDown;
    int MOTOCTL_GPIO;
    ILidarDriver* lidar;
    double obstacleData[MAXOBSTACLES][4];
    int validObstacles;
    double distThresh = DISTANCETHRESH;

    std::thread lidarThread;

    void lidarThreadRoutine();

    // Non-copyable, non-movable
    LidarSensor(const LidarSensor&) = delete;
    LidarSensor& operator=(const LidarSensor&) = delete;
};

#endif