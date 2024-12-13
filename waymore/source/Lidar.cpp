#include "../headers/Lidar.h"
#include "../headers/LidarSensor.hpp"

namespace {
    LidarSensor* lidar = nullptr;
}

Lidar* initializeLidar(const char* device, int baudrate, int MOTOCTL_GPIO) {
    if (lidar != nullptr) {
        std::cerr << "Lidar is already initialized!" << std::endl;
        return nullptr;
    }

    lidar = new LidarSensor(device, baudrate, MOTOCTL_GPIO);
    
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

void getLidarData(LidarData* data) {
    if (!data) {
        std::cerr << "Lidar API: Invalid data pointer" << std::endl;
        return;
    }

    double** ld = lidar->getLidarData();
    int vO = lidar->getValidObstacles();

    for (int i = 0; i < vO; i++) {
        data->obstacles->closestAngle = ld[i][0];
        data->obstacles->closestDistance = ld[i][1];
        data->obstacles->leftObstacleAngle = ld[i][2];
        data->obstacles->rightObstacleAngle = ld[i][3];
    }

    data->validObstacles = vO;
}

void uninitializeLidar() {
    if (!lidar) {
        std::cerr << "No lidar initialized!" << std::endl;
        return;
    }

    delete lidar;
    lidar = nullptr;
}