#include "../headers/Camera.h"
#include "../headers/CameraSensor.hpp"


namespace {
    CameraSensor* camera = nullptr;
}

Camera* initializeCamera(const int pixelWidth, const int pixelHeight, const int slices) {
    // Configure camera with desired dimension, color format, & type of stream
    // All pixel format: https://libcamera.org/api-html/formats_8h_source.html
    // WARNING: OpenCV takes only RGB type format. YUV420, etc will require additional conversion

    printf("Initializing camera...\n");

    const uint_fast32_t width = pixelWidth;
    const uint_fast32_t height = pixelHeight;
    const libcamera::PixelFormat pixelFormat = libcamera::formats::XRGB8888;
    const libcamera::StreamRole role = libcamera::StreamRole::Raw;

    if (camera != nullptr) {
        std::cerr << "Camera is already initialized!" << std::endl;
        return nullptr;
    }

    // Initialize the camera
    camera = new CameraSensor(slices);
    int result = camera->configCamera(width, height, pixelFormat, role);
    if (result != 0) {
        delete camera;
        camera = nullptr;
        return nullptr;
    }

    printf("camera initialized.\n");

    return reinterpret_cast<Camera*>(camera);
}

void startCamera() {
    if (!camera) {
        std::cerr << "No camera initialized!" << std::endl;
        exit(1);
    }

    try {
        std::cout << "Starting camera..." << std::endl;
        camera->startCamera();
    } catch (const std::exception& e) {
        std::cerr << "Camera failed to start: " << e.what() << std::endl;
    }
}

void getCameraLineDistances(int * distanceBuffer) {
    if (!camera) {
        std::cerr << "No camera initialized!" << std::endl;
        return;
    }

    return camera->getLineDistances(distanceBuffer);
}

void getCameraLineConfidences(double * confidenceBuffer) {
    if (!camera) {
        std::cerr << "No camera initialized!" << std::endl;
        return;
    }

    return camera->getConfidences(confidenceBuffer);
}

void uninitializeCamera() 
{
    if (!camera)
    {
        std::cerr << "No camera initialized!" << std::endl;
        return;
    }
    else
    {
        delete camera;
        camera = nullptr;
    }
}
