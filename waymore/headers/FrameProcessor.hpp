#ifndef _FRAME_PROCESSOR_HPP_
#define _FRAME_PROCESSOR_HPP_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Camera.h"

class FrameProcessor {
    public:
        FrameProcessor();
        void processFrame(const uint8_t* buffer, CameraData * cameraData);
        ~FrameProcessor();

    private:
        cv::Point * contourcenters;
        cv::Mat erosionKernel;
        cv::Mat dilationKernel;

        double processSlice(cv::Mat &slice, int sliceIndex);
};

#endif