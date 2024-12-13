#ifndef _FRAME_PROCESSOR_HPP_
#define _FRAME_PROCESSOR_HPP_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

class FrameProcessor {
    public:
        FrameProcessor(int sliceCount);

        ~FrameProcessor();

        void processFrame(unsigned int height, unsigned int width, const uint8_t* buffer);

        void getDistances(double * distanceBuffer);

    private:
        int slices;
        double * distances;
        cv::Point * contourcenters;
        cv::Mat erosionKernel;
        cv::Mat dilationKernel;

        // Main functionality
        void processSlice(cv::Mat &slice, int sliceIndex);

        // Performance measuring functionality
        struct timespec currentTime();
        void printTimeBetween(struct timespec * previous, struct timespec * current);
};

#endif