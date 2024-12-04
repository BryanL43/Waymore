#ifndef _CAMERA_H
#define _CAMERA_H

#include <stdint.h> // uint8_t, uint_fast32_t
#include <stdlib.h> // EXIT_FAILURE
#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

typedef void Camera; // Intermediate for C compatibility

Camera * initializeCamera(const int pixelWidth, const int pixelHeight, const int slices);
void startCamera();
void getCameraLineDistances(int * distanceBuffer);
void getCameraLineConfidences(double * confidenceBuffer);
void uninitializeCamera();

#ifdef __cplusplus
}
#endif

#endif