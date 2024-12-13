#include "../headers/LidarSensor.hpp"

// ============================================================================================= //
// Constructor
// ============================================================================================= //
LidarSensor::LidarSensor(const std::string& device, int baudrate, int MOTOCTL_GPIO)
    : MOTOCTL_GPIO(MOTOCTL_GPIO), lidar(nullptr) {

    distThresh = 5.0;
    shuttingDown.store(false);

    // Initialize the Lidar port & device
    Result<IChannel*> channel = createSerialPortChannel(device.c_str(), baudrate);
    if (!channel) {
        std::cerr << "Failed to create serial port channel for " << device << std::endl;
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
                    double angle = (nodes[i].angle_z_q14 * 90.0f) / 16384.0f;
                    double distance = nodes[i].dist_mm_q2 / 4.0f;

                    // Repeatedly search for obstacles within distance and map its left-most point
                    // of said obstacle to its right-most point
                    if (distance < distThresh && obstacleCount < MAXOBSTACLES) {
                        // Acquired left-most point
                        if (isnan(leftObstAngle)) {
                            leftObstAngle = angle;
                            rightObstAngle = angle; // Default rightObstAngle in case of error
                            closestAngle = angle;
                            closestDist = distance;
                        } else {
                            // Still seeking right-most point
                            if (!isnan(angle)) {
                                rightObstAngle = angle;
                                if (distance < closestDist) {
                                    closestDist = distance;
                                }
                            } else { // We have finished mapping 1 obstacle
                                obstacleData[count][0] = closestAngle;
                                obstacleData[count][1] = closestDist;
                                obstacleData[count][2] = leftObstAngle;
                                obstacleData[count][3] = rightObstAngle;
                                obstacleCount++;

                                // Reset for any potential additional obstacle
                                leftObstAngle = NAN;
                                rightObstAngle = NAN;
                                closestAngle = NAN;
                                closestDist = 99999;
                            }
                        }
                    }
                }

                validObstacles = obstacleCount;

            } else {
                setPinLevel(MOTOCTL_GPIO, LOW);
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

    setPinDirection(MOTOCTL_GPIO, OUT);
    setPinLevel(MOTOCTL_GPIO, LOW);
    setPinLevel(MOTOCTL_GPIO, HIGH);
    std::cout << "Lidar motor started." << std::endl;

    lidar->startScan(0, 1);

    lidarThread = std::thread(&LidarSensor::lidarThreadRoutine, this);
}

// ============================================================================================= //
// Retrieve the Lidar data
// ============================================================================================= //
double (*LidarSensor::getLidarData())[4] {
    std::cout << "Something wong: " << validObstacles << std::endl; 
    return obstacleData;
}

int LidarSensor::getValidObstacles() {
    return validObstacles;
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
    
    setPinLevel(MOTOCTL_GPIO, LOW);
    std::cout << "Lidar motor stopped." << std::endl;

    delete lidar;
    
    std::cout << "Successfully stopped and deleted Lidar!" << std::endl;
}