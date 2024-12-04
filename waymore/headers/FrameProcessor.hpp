#ifndef _FRAME_PROCESSOR_HPP_
#define _FRAME_PROCESSOR_HPP_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <mutex>

class FrameProcessor {
    public:
        FrameProcessor(int sliceCount, double meanIntensityMult,
                        int minThreshold, int maxThreshold, bool debug);
        ~FrameProcessor();

        void processFrame(cv::Mat &frame, unsigned int height, unsigned int width,
                            const uint8_t* buffer);
        void getDistances(int * distanceBuffer);

    private:
        int slices;
        double meanIntensityMult;
        int minThreshold;
        int maxThreshold;
        bool debugMode = false;
        int * distances;

        // Main functionality
        cv::Point processSlice(cv::Mat &slice, int sliceIndex, cv::Mat &frame, int sliceHeight);

        // Performance measuring functionality
        struct timespec currentTime();
        void printTimeBetween(struct timespec * previous, struct timespec * current);
};

#endif