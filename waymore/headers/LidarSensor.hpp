#ifndef _LIDAR_SENSOR_HPP_
#define _LIDAR_SENSOR_HPP_

#include "Lidar.h"
#include "sl_lidar.h" 
#include "sl_lidar_driver.h"

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

#define MAXOBSTACLES 5

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
    std::atomic<bool> shuttingDown;
    ILidarDriver* lidar;
    LidarData * lidarData;
    int distanceThresh;
    
    std::thread lidarThread;

    void lidarThreadRoutine();

    // Non-copyable, non-movable
    LidarSensor(const LidarSensor&) = delete;
    LidarSensor& operator=(const LidarSensor&) = delete;
};

#endif