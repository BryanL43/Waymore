#include "../headers/FrameProcessor.hpp"

FrameProcessor::FrameProcessor(int sliceCount) : slices(sliceCount) {
    // Allocate the return distance and confidence arrays
    distances = new double[slices]();

    // Precompute erosion and dilation kernels
    erosionKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(17, 17));
    dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(11, 11));
}

FrameProcessor::~FrameProcessor() {
    // Free dynamically allocated memory
    delete[] distances;

    // Close any OpenCV windows
    cv::destroyAllWindows();
}

void FrameProcessor::processFrame(unsigned int height, unsigned int width, const uint8_t* buffer) {
    // Create an OpenCV Mat directly from the buffer
    cv::Mat frame(height, width, CV_8UC4, const_cast<uint8_t*>(buffer));

    // Crop the frame to remove the top quarter
    frame = frame(cv::Rect(0, height / 5, width, 4 * height / 5));

    // Convert to grayscale and preprocess
    cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
    cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);
    cv::erode(frame, frame, erosionKernel);
    cv::dilate(frame, frame, dilationKernel);
    cv::threshold(frame, frame, 100, 255, cv::THRESH_BINARY);

    // Calculate slice height for processing
    int sliceHeight = frame.rows / slices;

    for (int i = 0; i < slices; i++) {
        // Define the region of interest for the slice
        cv::Rect sliceROI(0, i * sliceHeight, frame.cols, sliceHeight);
        cv::Mat slice = frame(sliceROI);

        // Process each slice
        processSlice(slice, i);
    }
}

void FrameProcessor::processSlice(cv::Mat& slice, int sliceIndex) {
    // Apply threshold and morphological closing
    cv::Mat thresh;
    cv::threshold(slice, thresh, 110, 255, cv::THRESH_BINARY_INV);
    cv::morphologyEx(thresh, thresh, cv::MORPH_CLOSE, erosionKernel, cv::Point(-1, -1), 2);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Skip empty contours immediately
    if (contours.empty()) {
        distances[sliceIndex] = std::numeric_limits<double>::quiet_NaN();
        return;
    }

    // Find the largest valid contour while skipping invalid ones
    const std::vector<cv::Point>* largestContour = nullptr;
    double largestArea = 0.0;

    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 100.0 && area > largestArea) { // Only consider contours with a minimum area
            largestContour = &contour;
            largestArea = area;
        }
    }

    // If no valid contour is found, set NaN for this slice
    if (!largestContour) {
        distances[sliceIndex] = std::numeric_limits<double>::quiet_NaN();
        return;
    }

    // Calculate the center of the largest contour
    cv::Moments M = cv::moments(*largestContour);
    int contourCenterX = (M.m00 != 0) ? static_cast<int>(M.m10 / M.m00) : slice.cols / 2;

    // Calculate distance from the center of the slice to the contour's center
    distances[sliceIndex] = static_cast<double>(slice.cols / 2 - contourCenterX);
}

void FrameProcessor::getDistances(double* distanceBuffer) {
    std::copy(distances, distances + slices, distanceBuffer);
}
