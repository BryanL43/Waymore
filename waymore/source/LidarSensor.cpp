#include "../headers/LidarSensor.hpp"
#define LIDARDEVICE "/dev/ttyS0"
#define BAUDRATE 115200
#define MOTOCTLGPIO 6

// ============================================================================================= //
// Constructor
// ============================================================================================= //
LidarSensor::LidarSensor()
{
    isRunning = false;

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
        while (isRunning) { 
            sl_lidar_response_measurement_node_hq_t nodes[8192];
            size_t count = _countof(nodes);

            sl_result op_result = lidar->grabScanDataHq(nodes, count);
            if (SL_IS_OK(op_result)) {
                lidar->ascendScanData(nodes, count);

                // Initialize LidarData structure with zeroed distances
                LidarData currentData = {0.0};

                for (size_t i = 0; i < count; i++) {
                    // Calculate angle and distance
                    double angle = (nodes[i].angle_z_q14 * 90.0f) / 16384.0f; // Angle in degrees
                    double distance = nodes[i].dist_mm_q2 / 4.0f;             // Distance in millimeters

                    // Ensure angle wraps around correctly (0-359 degrees)
                    int degreeIndex = (int)(angle + 0.5) % 360; // Round angle to the nearest degree

                    // Assign distance to the corresponding degree index
                    if (degreeIndex >= 0 && degreeIndex < 360) {
                        currentData.degreeDistances[degreeIndex] = distance;
                    }
                }

                // Update the shared lidar data
                *lidarData = currentData;
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
    isRunning = false;
    lidarThread.join();

    lidar->stop();
    
    setPinLevel(MOTOCTLGPIO, LOW);

    delete lidar;
}