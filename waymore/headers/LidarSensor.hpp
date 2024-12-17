#ifndef _LIDAR_SENSOR_HPP_
#define _LIDAR_SENSOR_HPP_

#include "Lidar.h"
#include "sl_lidar.h" 
#include "sl_lidar_driver.h"

#include <iostream>
#include <thread>

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

using namespace sl;

class LidarSensor {
public:
    LidarSensor();
    void startLidar();
    LidarData& getLidarDataRef();
    ~LidarSensor();
    
private:
    bool isRunning;
    ILidarDriver* lidar;
    LidarData * lidarData;
    int distanceThresh;
    
    std::thread lidarThread;

    void lidarThreadRoutine();

    // Delete Copy & Move Constructor
    LidarSensor(const LidarSensor&) = delete;
    LidarSensor(LidarSensor&&) = delete;

    // Delete Copy & Move Assignment Operator
    LidarSensor& operator=(const LidarSensor&) = delete;
    LidarSensor& operator=(LidarSensor&&) = delete;
};

#endif