#include "../headers/FrameProcessor.hpp"

FrameProcessor::FrameProcessor(int sliceCount) : slices(sliceCount)
{
    // Allocate the return distance array
    distances = new int[slices]{};
    confidences = new double[slices]{};
}

FrameProcessor::~FrameProcessor()
{
    // Close any OpenCV windows
    cv::destroyAllWindows();

    // Free the distances memory
    delete[] distances;
}


void FrameProcessor::processFrame(cv::Mat &frame, unsigned int height, unsigned int width,
                                  const uint8_t* buffer) 
{
    // Create an OpenCV Mat from the mapped buffer
    frame = cv::Mat(height, width, CV_8UC4, const_cast<uint8_t*>(buffer));

    // Crop the frame to the bottom 3/4ths height (Removes background interference)
    int cropStartY = height / 4;
    cv::Rect roi(0, cropStartY, width, height - cropStartY);
    frame = frame(roi);

    // Convert to grayscale
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGRA2GRAY);

    // Preprocess the grayscale image: Gaussian blur to reduce noise
    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

    // Apply erosion
    int erosionSize = 10;
    cv::Mat erosionKernel = cv::getStructuringElement(
        cv::MORPH_RECT,
        cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1)
    );
    cv::erode(gray, gray, erosionKernel);

    // Apply dilation after erosion
    int dilationSize = 10;
    cv::Mat dilationKernel = cv::getStructuringElement(
        cv::MORPH_RECT, 
        cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1)
    );
    cv::dilate(gray, gray, dilationKernel);

    int darkGrayThreshold = 110;
    cv::threshold(gray, gray, darkGrayThreshold, 255, cv::THRESH_BINARY);

    // Compute slice height for the bottom half
    int sliceHeight = gray.rows / slices;
    std::vector<cv::Point> contourCenters; // To store the centers of the contours
    
    for (int i = 0; i < slices; i++) 
    {
        int startSliceY = i * sliceHeight;
        cv::Rect sliceROI(0, startSliceY, gray.cols, sliceHeight);
        cv::Mat slice = gray(sliceROI);

        // Process each slice and get the contour center
        cv::Point contourCenter = processSlice(slice, i, frame, sliceHeight);
        contourCenters.push_back(contourCenter);

        if (DEBUG_MODE) 
        {
            // Draw red slice center dot
            int sliceMiddleX = slice.cols / 2;
            int sliceMiddleY = sliceHeight / 2 + startSliceY;
            cv::circle(frame, cv::Point(sliceMiddleX, sliceMiddleY), 5, cv::Scalar(0, 0, 255), -1);

            // Draw pink line connecting the white dot to the red dot
            cv::line(frame, contourCenter, cv::Point(sliceMiddleX, contourCenter.y), cv::Scalar(255, 20, 147), 2);
        }
    }

    if (DEBUG_MODE)
    {
        // Draw blue lines connecting all white dots
        for (size_t i = 1; i < contourCenters.size(); ++i) 
        {
            cv::line(frame, contourCenters[i - 1], contourCenters[i], cv::Scalar(255, 0, 0), 2, cv::LINE_8, 0);
        }

        // Draw a blue line from the first to the last contour center
        if (contourCenters.size() > 1) 
        {
            cv::line(frame, contourCenters.front(), contourCenters.back(), cv::Scalar(255, 0, 0), 2, cv::LINE_8, 0);
        }
    }

    // // Display the processed result
    // cv::imshow("Camera Feed - Bottom Half", frame);
    // cv::waitKey(1);
}


cv::Point FrameProcessor::processSlice(cv::Mat &slice, int sliceIndex, cv::Mat &frame,
                                        int sliceHeight) 
{
    // Apply threshold & morphological closing to clean up noise and fill small gaps
    cv::Mat thresh;
    cv::threshold(slice, thresh, 110, 255, cv::THRESH_BINARY_INV);
    cv::morphologyEx(thresh, thresh, cv::MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), 2);

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // No contours found; return the center of the slice for continuity
    if (contours.empty()) 
    {
        return cv::Point(slice.cols / 2, sliceHeight / 2 + sliceIndex * sliceHeight);
    }

    // Find the largest contour
    auto mainContour = *std::max_element(contours.begin(), contours.end(),
        [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) 
        {
            return cv::contourArea(a) < cv::contourArea(b);
        }
    );

    // Calculate the center of the largest contour
    cv::Moments M = cv::moments(mainContour);
    int contourCenterX = (M.m00 != 0) ? static_cast<int>(M.m10 / M.m00) : slice.cols / 2;
    int contourCenterY = sliceHeight / 2;

    // Calculate distance from the center of the slice to the contour's center
    int sliceMiddleX = slice.cols / 2;
    int distance = sliceMiddleX - contourCenterX;

    // Calculate extent of the contour
    double extent = cv::contourArea(mainContour) / static_cast<double>(cv::boundingRect(mainContour).area());

    distances[sliceIndex] = distance;
    confidences[sliceIndex] = extent;

    if (DEBUG_MODE) 
    {
        // Draw the green contour and white center dot
        cv::Rect sliceROI(0, sliceIndex * sliceHeight, slice.cols, sliceHeight);
        cv::drawContours(frame(sliceROI), std::vector<std::vector<cv::Point>>{mainContour}, -1, cv::Scalar(0, 255, 0), 2);
        cv::circle(frame, cv::Point(contourCenterX, contourCenterY + sliceIndex * sliceHeight), 5, cv::Scalar(255, 255, 255), -1);

        // Display the calculated distance and extent
        cv::putText(frame, "Dist: " + std::to_string(distance),
                    cv::Point(contourCenterX + 20, contourCenterY + sliceIndex * sliceHeight - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(200, 0, 200), 2);
        cv::putText(frame, "Weight: " + std::to_string(extent),
                    cv::Point(contourCenterX + 20, contourCenterY + sliceIndex * sliceHeight + 20),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 0, 200), 1);
    }

    // Return the center of the contour
    return cv::Point(contourCenterX, contourCenterY + sliceIndex * sliceHeight);
}

void FrameProcessor::printTimeBetween(struct timespec * previous, struct timespec * current)
{
	long secondComponent = current->tv_sec - previous->tv_sec;
	long nanoComponent = current->tv_nsec - previous->tv_nsec;
	unsigned long nanoSince = secondComponent*1000000000L + nanoComponent;
    double microSince = nanoSince/1000.0;
	printf("Duration between times: %.2f microseconds\n", microSince);
}

struct timespec FrameProcessor::currentTime()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts;
}

void FrameProcessor::getDistances(int * distanceBuffer)
{
    std::copy(distances, distances+sizeof(distances), distanceBuffer);
}

void FrameProcessor::getConfidences(double * confidenceBuffer)
{
    std::copy(confidences, confidences+sizeof(confidences), confidenceBuffer);
}


// static struct timespec previous;
// // Measured things go here
// struct timespec current = currentTime();
// printTimeBetween(&previous, &current);
// previous = current;