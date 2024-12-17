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
static LidarData sweepdata;
void LidarSensor::lidarThreadRoutine() {
    try {
        while (!shuttingDown.load()) { 
            sl_lidar_response_measurement_node_hq_t nodes[8192];
            size_t count = _countof(nodes);

            sl_result op_result = lidar->grabScanDataHq(nodes, count);
            if (SL_IS_OK(op_result)) {
                lidar->ascendScanData(nodes, count);

                // 360 Lidar scan setup
                sweepdata = {0};
                double leftObstAngle = NAN, rightObstAngle = NAN, closestAngle = NAN;
                double closestDist = 1e5;

                for (size_t i = 0; i < count; i++) {
                    // Limit the number of obstacles to MAXOBSTACLES
                    if (sweepdata.validObstacles >= MAXOBSTACLES) 
                        break;

                    // Filter out invalid angles
                    double angle = (nodes[i].angle_z_q14 * 90.0f) / 16384.0f;
                    if(angle < 30 || angle > 330) 
                        continue;

                    // Filter out invalid distances
                    double distance = nodes[i].dist_mm_q2 / 4.0f;
                    if (distance > 2000 || distance < 150)
                        continue;
                    

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
                        sweepdata.obstacles[sweepdata.validObstacles].closestAngle = closestAngle;
                        sweepdata.obstacles[sweepdata.validObstacles].closestDistance = closestDist;
                        sweepdata.obstacles[sweepdata.validObstacles].leftObstacleAngle = leftObstAngle;
                        sweepdata.obstacles[sweepdata.validObstacles].rightObstacleAngle = rightObstAngle;
                        sweepdata.validObstacles ++;

                        // Reset scoped vars for the next obstacle
                        leftObstAngle = NAN;
                        rightObstAngle = NAN;
                        closestAngle = NAN;
                        closestDist = 1e5;
                    }
                }
                *lidarData = sweepdata;
            } else {
                // Stop the motor and throw an exception
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