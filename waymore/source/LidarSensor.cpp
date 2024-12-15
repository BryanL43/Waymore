#include "../headers/LidarSensor.hpp"
#define LIDARDEVICE "/dev/ttyS0"
#define BAUDRATE 115200
#define MOTOCTLGPIO 6
#define DISTANCETHRESH 1000

// ============================================================================================= //
// Constructor
// ============================================================================================= //
LidarSensor::LidarSensor()
{
    shuttingDown.store(false);

    lidarData = new LidarData;

    // Initialize the Lidar port & device
    Result<IChannel*> channel = createSerialPortChannel(LIDARDEVICE, BAUDRATE);
    if (!channel) {
        std::cerr << "Failed to create serial port channel for " << LIDARDEVICE << std::endl;
        return;
    }

    lidar = *createLidarDriver();
    if (!lidar) {
        std::cerr << "Error: Insufficient memory to create Lidar driver" << std::endl;
        return;
    }

    // Connect to the Lidar
    auto res = lidar->connect(*channel);
    if (SL_IS_OK(res)) {
        sl_lidar_response_device_info_t deviceInfo;
        res = lidar->getDeviceInfo(deviceInfo);

        if (SL_IS_OK(res)) {
            std::cout << "Model: " << deviceInfo.model
                << ", Firmware Version: " << (deviceInfo.firmware_version >> 8)
                << "."
                << (deviceInfo.firmware_version & 0xffu)
                << ", Hardware Version: " << deviceInfo.hardware_version
                << std::endl;
        } else {
            std::cerr << "Failed to get device information from LIDAR " << res << std::endl;
            return;
        }
    } else {
        std::cerr << "Failed to get device information from LIDAR " << res << std::endl;
        return;
    }
}

// ============================================================================================= //
// Thread routine for Lidar obstacle data acquistion
// ============================================================================================= //
void LidarSensor::lidarThreadRoutine() {
    try {
        while (!shuttingDown.load()) { 
            sl_lidar_response_measurement_node_hq_t nodes[8192];
            size_t count = _countof(nodes);

            sl_result op_result = lidar->grabScanDataHq(nodes, count);
            if (SL_IS_OK(op_result)) {
                lidar->ascendScanData(nodes, count);

                double leftObstAngle = NAN, rightObstAngle = NAN, closestAngle = NAN;
                double closestDist = 99999;
                int obstacleCount = 0;

                // 360 Lidar scan
                for (size_t i = 0; i < count; i++) {
                    if (obstacleCount >= MAXOBSTACLES) break;
                    double angle = (nodes[i].angle_z_q14 * 90.0f) / 16384.0f;
                    angle = fmod(angle, 360.0);
                    if(angle < 60 || angle > 300) continue;
                    double distance = nodes[i].dist_mm_q2 / 4.0f;

                    // if(angle > 175 && angle < 185) {
                    //     printf("Angle: %.2f, Distance: %.2f\n", angle, distance);
                    // }
                    
                    // Repeatedly search for obstacles within distance and map its left-most point
                    // of said obstacle to its right-most point
                    if (distance < DISTANCETHRESH) {
                        // Acquired left-most point
                        if (isnan(leftObstAngle)) {
                            leftObstAngle = angle;
                            rightObstAngle = angle; // Default rightObstAngle in case of error
                            closestAngle = angle;
                            closestDist = distance;
                        } else {
                            // Still seeking right-most point
                            rightObstAngle = angle;
                            if (distance < closestDist) {
                                closestDist = distance;
                            }
                        }
                    } else {
                        if(isnan(leftObstAngle)) continue;
                        // We have finished mapping this obstacle
                        lidarData->obstacles[obstacleCount].closestAngle = closestAngle;
                        lidarData->obstacles[obstacleCount].closestDistance = closestDist;
                        lidarData->obstacles[obstacleCount].leftObstacleAngle = leftObstAngle;
                        lidarData->obstacles[obstacleCount].rightObstacleAngle = rightObstAngle;
                        lidarData->validObstacles ++;

                        // Reset scoped vars for the next obstacle
                        leftObstAngle = NAN;
                        rightObstAngle = NAN;
                        closestAngle = NAN;
                        closestDist = 99999;
                    }
                }
            } else {
                setPinLevel(MOTOCTLGPIO, LOW);
                throw std::runtime_error("Lidar failed to acquire scanned data!");
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in lidar thread: " << e.what() << std::endl;
    }
}

// ============================================================================================= //
// Starting the Lidar
// ============================================================================================= //
void LidarSensor::startLidar() {
    if (!lidar) {
        std::cerr << "Fatal: No Lidar detected" << std::endl;
        return;
    }

    setPinDirection(MOTOCTLGPIO, OUT);
    setPinLevel(MOTOCTLGPIO, LOW);
    setPinLevel(MOTOCTLGPIO, HIGH);

    lidar->startScan(FALSE, TRUE);

    lidarThread = std::thread(&LidarSensor::lidarThreadRoutine, this);
}

// ============================================================================================= //
// Retrieve the Lidar data
// ============================================================================================= //
LidarData& LidarSensor::getLidarDataRef() 
{
    return * lidarData;
}

// ============================================================================================= //
// Stopping and Destroying the Lidar instance
// ============================================================================================= //
LidarSensor::~LidarSensor() {
    if (!lidar) {
        std::cerr << "Lidar is not initialized or already stopped." << std::endl;
        return;
    }

    // Set the shutting down flag to stop further traffic from startLidar()
    shuttingDown.store(true);
    lidarThread.join();

    lidar->stop();
    
    setPinLevel(MOTOCTLGPIO, LOW);

    delete lidar;
}