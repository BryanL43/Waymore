#ifndef _FRAME_PROCESSOR_HPP_
#define _FRAME_PROCESSOR_HPP_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

class FrameProcessor {
    public:
        FrameProcessor(int sliceCount);

        ~FrameProcessor();

        void processFrame(cv::Mat &frame, unsigned int height, unsigned int width,
                            const uint8_t* buffer);

        void getDistances(double * distanceBuffer);

        void getConfidences(double * confidenceBuffer);

    private:
        int slices;
        double * distances;
        double * confidences;

        // Main functionality
        cv::Point processSlice(cv::Mat &slice, int sliceIndex, cv::Mat &frame, int sliceHeight);

        // Performance measuring functionality
        struct timespec currentTime();
        void printTimeBetween(struct timespec * previous, struct timespec * current);
};

#endif