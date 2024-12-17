// #include "../headers/Camera.h"
// #include "../headers/CameraSensor.hpp"

// namespace {
//     CameraSensor* camera = nullptr;
// }

// Camera* initializeCamera() 
// {
//     printf("Initializing camera...\n");

//     const uint_fast32_t width = CAMWIDTH;
//     const uint_fast32_t height = CAMHEIGHT;
//     const libcamera::PixelFormat pixelFormat = libcamera::formats::XRGB8888;
//     const libcamera::StreamRole role = libcamera::StreamRole::Raw;

//     if (camera != nullptr) {
//         std::cerr << "Camera is already initialized!" << std::endl;
//         return nullptr;
//     }

//     // Initialize the camera
//     camera = new CameraSensor(CAMSLICES);
//     int result = camera->configCamera(width, height, pixelFormat, role);
//     if (result != 0) {
//         delete camera;
//         camera = nullptr;
//         return nullptr;
//     }

//     printf("camera initialized.\n");

//     return reinterpret_cast<Camera*>(camera);
// }

// void startCamera() {
//     if (!camera) {
//         std::cerr << "No camera initialized!" << std::endl;
//         exit(1);
//     }

//     try {
//         std::cout << "Starting camera..." << std::endl;
//         camera->startCamera();
//     } catch (const std::exception& e) {
//         std::cerr << "Camera failed to start: " << e.what() << std::endl;
//     }
// }

// CameraData * getCameraDataRef() {
//     if (!camera) {
//         std::cerr << "No camera initialized!" << std::endl;
//         return nullptr;
//     }

//     return &camera->getCameraDataRef();
// }

// void uninitializeCamera() 
// {
//     if (!camera)
//     {
//         std::cerr << "No camera initialized!" << std::endl;
//         return;
//     }
//     else
//     {
//         delete camera;
//         camera = nullptr;
//     }
// }
