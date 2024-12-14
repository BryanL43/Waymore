#include "../headers/Lidar.h"
#include "../headers/LidarSensor.hpp"

namespace {
    LidarSensor* lidar = nullptr;
}

Lidar* initializeLidar() {
    if (lidar != nullptr) {
        std::cerr << "Lidar is already initialized!" << std::endl;
        return nullptr;
    }

    lidar = new LidarSensor();
    
    printf("Lidar initialized.\n");

    return reinterpret_cast<Lidar*>(lidar);
}

void startLidar() {
    if (!lidar) {
        std::cerr << "No lidar initialized" << std::endl;
        exit(1);
    }

    try {
        std::cout << "Starting Lidar..." << std::endl;

        lidar->startLidar();
    } catch (const std::exception& e) {
        std::cerr << "Lidar failed to start: " << e.what() << std::endl;
    }
}

LidarData * getLidarDataRef()
{
    return &lidar->getLidarDataRef();
}

void uninitializeLidar() {
    if (!lidar) {
        std::cerr << "No lidar initialized!" << std::endl;
        return;
    }

    delete lidar;
    lidar = nullptr;
}