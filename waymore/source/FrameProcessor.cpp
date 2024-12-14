#include "../headers/FrameProcessor.hpp"

FrameProcessor::FrameProcessor()
{
    // Precompute erosion and dilation kernels
    erosionKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(17, 17));
    dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(11, 11));
}

FrameProcessor::~FrameProcessor() {
    erosionKernel.release();
    dilationKernel.release();
}

void FrameProcessor::processFrame(const uint8_t* buffer, CameraData * cameraData) {
    cv::setNumThreads(1);

    // Create an OpenCV Mat directly from the buffer
    cv::Mat frame(CAMHEIGHT, CAMWIDTH, CV_8UC4, const_cast<uint8_t*>(buffer));

    // Crop the frame to remove the top quarter
    frame = frame(cv::Rect(0, CAMHEIGHT / 5, CAMWIDTH, 4 * CAMHEIGHT / 5));

    // Convert to grayscale and apply Gaussian blur
    cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
    cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);

    // At this point, do NOT erode, dilate, or threshold the entire frame.
    // We will handle thresholding and morphology per slice in processSlice().

    // Calculate slice height for processing
    int sliceHeight = frame.rows / CAMSLICES;

    for (int i = 0; i < CAMSLICES; i++) {
        // Define the region of interest for the slice
        cv::Rect sliceROI(0, i * sliceHeight, frame.cols, sliceHeight);
        cv::Mat slice = frame(sliceROI);

        // Process each slice
        cameraData->distances[i] = processSlice(slice, i);
    }
}

double FrameProcessor::processSlice(cv::Mat& slice, int /*sliceIndex*/) {
    // Threshold and morphological operations for contour detection
    cv::Mat thresh;
    // Apply threshold and invert it
    cv::threshold(slice, thresh, 110, 255, cv::THRESH_BINARY_INV);

    // Apply morphological closing to refine the shapes
    cv::morphologyEx(thresh, thresh, cv::MORPH_CLOSE, erosionKernel, cv::Point(-1, -1), 2);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // If no contours, return NaN
    if (contours.empty()) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    // Find the largest valid contour by area
    const std::vector<cv::Point>* largestContour = nullptr;
    double largestArea = 0.0;

    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 100.0 && area > largestArea) {
            largestContour = &contour;
            largestArea = area;
        }
    }

    // If no valid contour is found, set NaN for this slice
    if (!largestContour) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    // Calculate the center of the largest contour
    cv::Moments M = cv::moments(*largestContour);
    int contourCenterX = (M.m00 != 0) ? static_cast<int>(M.m10 / M.m00) : slice.cols / 2;

    // Calculate distance from the center of the slice to the contour's center
    return static_cast<double>(slice.cols / 2 - contourCenterX);
}