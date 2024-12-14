#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <stdint.h> // uint8_t, uint_fast32_t
#include <stdlib.h> // EXIT_FAILURE
#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

#define CAMSLICES 4
#define CAMWIDTH 640
#define CAMHEIGHT 480

typedef void Camera; // Intermediate for C compatibility

typedef struct CameraData
{
    double distances[CAMSLICES];
} CameraData;

Camera * initializeCamera();
void startCamera();
CameraData * getCameraDataRef();
void uninitializeCamera();

#ifdef __cplusplus
}
#endif

#endif