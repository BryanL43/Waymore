#include "../headers/CameraSensor.hpp"

// ============================================================================================= //
// Constructor and Configuration
// ============================================================================================= //
CameraSensor::CameraSensor(const int horizontalSlices)
{
    // Loads the library's cam manager for cam acquisition
    camManager = std::make_unique<CameraManager>();
    camManager->start();

    // Identifies all cameras attached to the device
    auto attachedCameras = camManager->cameras();
    if (attachedCameras.empty()) {
        std::cerr << "No cameras were identified on the system." << std::endl;
        camManager->stop();
        exit(EXIT_FAILURE);
    }

    // Acquire only the first cam (only option we have) & put a lock on it
    cam = attachedCameras.front();
    if (cam->acquire() != 0) {
        std::cerr << "Failed to acquire cam." << std::endl;
        camManager->stop();
        exit(EXIT_FAILURE);
    }

    // Create & config frame processor
    frameProcessor = std::make_unique<FrameProcessor>();

    // Create the camera data struct
    cameraData = new CameraData;
}

int CameraSensor::configCamera( const uint_fast32_t width,
                                const uint_fast32_t height,
                                const PixelFormat pixelFormat,
                                const StreamRole role) 
{
    // Create configuration profile for the camera
    config = cam->generateConfiguration({ role });
    StreamConfiguration &streamConfig = config->at(0);
    //std::cout << "Default configuration is: " << streamConfig.toString() << std::endl;

    // Adjust & validate the desired configuration
    streamConfig.size.width = width;
    streamConfig.size.height = height;
    streamConfig.pixelFormat = pixelFormat;
    config->validate();

    if (cam->configure(config.get()) != 0) {
        std::cerr << "Failed to configure camera: " << cam->id() << std::endl;
        return -EINVAL;
    }

    // Allocate the buffers & map memory
    allocator = std::make_unique<FrameBufferAllocator>(cam);
    for (StreamConfiguration &cfg : *config) {
        Stream* stream = cfg.stream();
        if (allocator->allocate(stream) < 0) {
            std::cerr << "Failed to allocate buffers for stream" << std::endl;
            return -ENOMEM;
        }

        for (const std::unique_ptr<FrameBuffer>& buffer : allocator->buffers(stream)) {
            for (unsigned int i = 0; i < buffer->planes().size(); i++) {
                const FrameBuffer::Plane &plane = buffer->planes()[i];
                void* data_ = mmap(NULL, plane.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                                   plane.fd.get(), 0);
                if (data_ == MAP_FAILED) {
                    throw std::runtime_error("Failed to map buffer for plane");
                }
                mappedBuffers[buffer.get()].push_back(
                    libcamera::Span<uint8_t>(static_cast<uint8_t*>(data_), plane.length));
            }
            frameBuffers[stream].push(buffer.get());
        }
    }

    return 0;
}

// ============================================================================================= //
// Starting the Camera
// ============================================================================================= //

void CameraSensor::startCamera() {
    prepareRequests();

    cam->requestCompleted.connect(this, &CameraSensor::fillRequest);
    cam->start();

    cam->queueRequest(requests.front().get());

    running = true;
}

void CameraSensor::prepareRequests() 
{
    // Acquire the allocated buffers for streams stored in CameraConfiguration by libcamera
    // to create the requests (we can percieve request as a promise and fullfill event)
    for (StreamConfiguration &cfg : *config) {
        Stream* stream = cfg.stream();
        
        std::unique_ptr<Request> request = cam->createRequest();
        if (!request) {
            std::cerr << "Can't create request" << std::endl;
            throw std::runtime_error("Failed to make a request");
        }
        requests.push_back(std::move(request));

        // Seperate the frame buffer associated with the stream
        FrameBuffer* buffer = frameBuffers[stream].front();

        if (requests.back()->addBuffer(stream, buffer) < 0) {
            throw std::runtime_error("Failed to add buffer to request");
        }
    }
}

// ============================================================================================= //
// Main Operation of the Camera
// ============================================================================================= //

void CameraSensor::fillRequest(Request * request)
{
    if(!running) return;

    // Make sure the request hasn't been cancelled
    if (request->status() == Request::RequestCancelled) return;

    // Iterate through the buffers for this request, rendering & recycling the buffers
    for (auto& [stream, buffer] : request->buffers()) {
        if (buffer->metadata().status == FrameMetadata::FrameSuccess) {
            try {
                // Render the frame, then reuse the buffer and re-queue the request
                processFrame(buffer);
                request->reuse(Request::ReuseBuffers);
                std::this_thread::yield();
                cam->queueRequest(request);
            } catch (const std::exception& e) {
                std::cerr << "Error trying to render frame: " << e.what() << std::endl;
                return;
            }
        }
    }
}

void CameraSensor::processFrame(const libcamera::FrameBuffer *buffer) 
{
    try {
        // Find the mapped buffer associated with the given FrameBuffer
        auto item = mappedBuffers.find(const_cast<libcamera::FrameBuffer*>(buffer));
        if (item == mappedBuffers.end()) {
            std::cerr << "Mapped buffer not found, cannot display frame" << std::endl;
            return;
        }

        // Retrieve the pre-mapped buffer
        const std::vector<libcamera::Span<uint8_t>> &retrievedBuffers = item->second;
        if (retrievedBuffers.empty() || retrievedBuffers[0].data() == nullptr) {
            std::cerr << "Mapped buffer is empty or data is null, cannot display frame" << std::endl;
            return;
        }
        frameProcessor->processFrame(retrievedBuffers[0].data(), cameraData);
    } catch (const std::exception &e) {
        std::cerr << "Error rendering frame: " << e.what() << std::endl;
    }
}

CameraData& CameraSensor::getCameraDataRef() 
{
    return * cameraData;
}

// ============================================================================================= //
// Stopping and Destroying the Camera instance
// ============================================================================================= //

CameraSensor::~CameraSensor()
{
    if (!cam) {
        std::cerr << "Camera is not initialized or already stopped." << std::endl;
        return;
    }

    running = false;
    // Wait for a short duration to make sure the current fillRequest() function has been exited
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Stop the camera
    cam->stop();

    // Clear the requests
    requests.clear();

    // Release exclusive control of the camera device
    cam->release();

    // Free memory mappings
    for (auto& [buffer, spans] : mappedBuffers) {
        for (auto& span : spans) {
            munmap(span.data(), span.size());
        }
    }
    mappedBuffers.clear();

    // Free frame buffer allocations
    if (allocator) {
        for (StreamConfiguration &cfg : *config) {
            allocator->free(cfg.stream());
        }
        allocator.reset();
    }

    delete cameraData;

    cam.reset();
    camManager->stop();
    camManager.reset();
}