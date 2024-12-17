/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: waymoreLib.h
*
* Description:: Library of functions and wrappers for use
*				in all other .c and .h files
*
**************************************************************/

#include "../headers/WaymoreLib.h"

// ============================================================================================= //
// Definitions of (private) Constants
// ============================================================================================= //

// Pertaining to GPIO functionality
#define GPIOBASE 0xfe200000
#define MMAPBLOCKSIZE 4096

// Pertaining to I2C functionality
#define I2CBUS "/dev/i2c-1"

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

// Pertaining to GPIO functionality
volatile uint32_t * gpio;
int selectIdx	= SEL >> 2;
int setIdx	    = SET >> 2;
int clearIdx	= CLR >> 2;
int readIdx	    = RD  >> 2;

// Pertaining to I2C functionality
int i2cBus = -1;
volatile uint8_t currentI2cAddr = -1;

// ============================================================================================= //
// Validation functions
// ============================================================================================= //

void validatePin(int pin)
{
        if(pin < 0 || pin > 27)
        {
                fprintf(stderr,
                        "%d is an invalid GPIO pin number: ",
                        pin);
                exit(1);
        }
}

void validateLevel(int level)
{
        if(level != HIGH && level != LOW)
        {
                fprintf(stderr,
                        "%d is an invalid voltage level argument: "
                        "it must be ON/HIGH (1) or OFF/LOW (0).\n",
                        level);
                exit(1);
        }
}

void validateDirection(int direction)
{
        if(direction != READ && direction != WRITE)
        {
                fprintf(stderr,
                        "%d is an invalid direction argument: "
                        "it must be READ (0) or WRITE (1).\n",
                        direction);
                exit(1);
        }
}


// ============================================================================================= //
// GPIO Initialization and Uninitialization Functions
// ============================================================================================= //

int initializeGPIO()
{
	/*
	** initGPIO opens the /dev/mem device file which allows
	** the library access to the physical memory of the pi.
	** Then it maps this device into the virtual address space
	** with mmap & our volatile gpio variable.
	*/

	printf("Initializing gpio...");

	// Open the dev/mem file, catching errors
	int mem = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem == -1)
	{
		fprintf(stderr, "Failed to open dev/mem for memory mapping!\n");
		return -1;
	}

	// map the GPIO pins into memory, catching errors
	gpio = (volatile uint32_t *) mmap(NULL,
					  MMAPBLOCKSIZE,
					  PROT_READ | PROT_WRITE,
					  MAP_SHARED,
					  mem,
					  GPIOBASE);

	if (gpio == MAP_FAILED)
	{
		fprintf(stderr, "GPIO memory mapping failed!\n");
		if (close(mem) != 0)
		{
			fprintf(stderr, "Failed to close dev/mem!\n");
		}
		return -1;
	}

	// Close file and return
	if (close(mem) != 0)
	{
		fprintf(stderr, "Failed to close dev/mem!\n");
		return -1;
	}

	printf("done.\n");
	return 0;
}

int uninitializeGPIO()
{
	// Check that the memory is currently mapped
	if (gpio == NULL)
	{
		fprintf(stderr, "Warning: GPIO was already uninitialized!\n");
        return 1;
	}

	// Attempt to unmap the memory
	if (munmap((void *)gpio, MMAPBLOCKSIZE) < 0)
	{
		fprintf(stderr, "Error: munmap failed in uninitializeGPIO");
		return -1;
	}
	gpio = NULL;

	return 0;
}

int initializeI2C()
{
	printf("Initializing I2C...");

	if (bcm2835_init() == FALSE)
	{
		fprintf(stderr, "Failed to initialize I2C bus: BCM2835_init failed!\n");
		return -1;
	}
	else
	{
		bcm2835_i2c_begin(); // this is a void function.
	}

	printf("done.\n");
	return 0;
}

void registerDeviceI2C(uint8_t ADDR)
{
	bcm2835_i2c_setSlaveAddress(ADDR);
}

int uninitializeI2C()
{
	// Stop bcm i2c functionality (void function)
	bcm2835_i2c_end();

	// Close bcm library
    if(bcm2835_close() == FALSE)
    {
        fprintf(stderr, "Failed to close I2C bus: bcm2835_close() failed.\n");
        return -1;
    }

	return 0;
}

// ============================================================================================= //
// GPIO Primary Functions
// ============================================================================================= //

void setPinDirection(int pin, int direction)
{
	/*
	** setPinDirection allows the user to choose to set a pin
	** to either reading (in) or writing (out) configurations.
	*/

	// First validate the pin number
	validatePin(pin);

	// Then validate the direction
	validateDirection(direction);

	// Then calculate the register index of the given pin
	int registerIndex = pin / 10;

	// And the bit offset within the register for the pin
	int bitOffset 	  = (pin % 10) * 3;

	// Record the current register's bit configuration
	uint32_t bitConfig = gpio[registerIndex];

	// Make a mask which will clear the pin direction
	uint32_t clearMask  = ~(0b111 << bitOffset);

	// Make a mask which will set the pin direction to WRITE
	uint32_t writeMask = (1 << bitOffset);

	if (direction == WRITE)
	{
		// Clear and apply write mask
		gpio[registerIndex] = (bitConfig & clearMask) | writeMask;
	}
	else if (direction == READ)
	{
		// Read is the default while clear
		gpio[registerIndex] = (bitConfig & clearMask);
	}
}

void setPinLevel(int pin, int level)
{
	// Validate the inputs
	validatePin(pin);
	validateLevel(level);

	// Get the pin's bitmask
	uint32_t pinMask = (1 << pin);

	if (level == HIGH)
	{
		// send a 1 to the set pin value index
		gpio[setIdx] = pinMask;
	}
	else
	{
		// send a 1 to the clear pin value index
		gpio[clearIdx] = pinMask;
	}
}

int getPinLevel(int pin)
{
	// Validate the input
	validatePin(pin);

	// Get the pin's bitmask
	uint32_t pinMask = (1 << pin);

	// Get the current level
	uint32_t level = gpio[readIdx] & pinMask;
	return (level != 0) ? HIGH : LOW;
}

int readByteI2C(uint8_t ADDR, uint8_t reg)
{
	char buffer[2] = {0};
    
    if (currentI2cAddr != ADDR)
    {
        // Set the address to the desired device if necessary (void function)
        currentI2cAddr = ADDR;
        bcm2835_i2c_setSlaveAddress(ADDR);
    }
    
    if (bcm2835_i2c_read_register_rs((char*)&reg, buffer, 2) != BCM2835_I2C_REASON_OK)
    {
        fprintf(stderr, "Failed to read bytes from I2C address 0x%02x!\n", ADDR);
        return -1;
    }

	return buffer[0];
}

int readBytesI2C(uint8_t ADDR, char * destbuf, uint32_t len)
{
	if(currentI2cAddr != ADDR)
	{
		currentI2cAddr = ADDR;
		bcm2835_i2c_setSlaveAddress(ADDR);
	}

	if (bcm2835_i2c_read(destbuf, len) != BCM2835_I2C_REASON_OK)
	{
		fprintf(stderr, "Failed to read bytes from I2C address 0x%02x!\n", ADDR);
		return -1;
	}

	return 0;
}

int writeByteI2C(uint8_t ADDR, uint8_t reg, uint8_t value)
{
    if(currentI2cAddr != ADDR)
    {
        currentI2cAddr = ADDR;
        bcm2835_i2c_setSlaveAddress(ADDR);
    }

	char buffer[2] = {reg, value};

    if (bcm2835_i2c_write(buffer, 2) != BCM2835_I2C_REASON_OK)
    {
        fprintf(stderr, "Failed to write bytes to I2C address 0x%02x!\n", ADDR);
        return -1;
    }

	return 0;
}

int writeBytesI2C(uint8_t ADDR, const char * buf, uint32_t len)
{
	if(currentI2cAddr != ADDR)
	{
		currentI2cAddr = ADDR;
		bcm2835_i2c_setSlaveAddress(ADDR);
	}

	if (bcm2835_i2c_write(buf, len) != BCM2835_I2C_REASON_OK)
	{
		fprintf(stderr, "Failed to write bytes to I2C address 0x%02x!\n", ADDR);
		return -1;
	}

	return 0;
}

// ============================================================================================= //
// Threading Initialization and Uninitialization Functions
// ============================================================================================= //

Thread *startThread(const char *name, void *(*function)(void *))
{
    if (function == NULL)
    {
        fprintf(stderr, "Error: Function pointer is NULL for thread: %s\n", name);
        return NULL;
    }

    Thread *thread = malloc(sizeof(Thread));
    if (thread == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate memory for thread: %s\n", name);
        return NULL;
    }

    thread->name = strdup(name ? name : "unnamed");
    if (thread->name == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate memory for thread name.\n");
        free(thread);
        return NULL;
    }

    thread->running = 1;

    if (pthread_create(&thread->id, NULL, function, NULL) != 0)
    {
        fprintf(stderr, "Error: Failed to create thread: %s\n", thread->name);
        free(thread->name);
        free(thread);
        return NULL;
    }

    return thread;
}

void stopThread(Thread *thread)
{
    if (thread == NULL)
    {
        return;
    }

    thread->running = 0;

    pthread_join(thread->id, NULL);

    printf("'%s' has finished - freeing its resources\n", thread->name);

    if (thread->name != NULL)
        free(thread->name);

    free(thread);
}


// =============================================================================== //
// Time Related Functions
// =============================================================================== //

void nanoWait(uint64_t nanoseconds)
{
        /*
        ** nanoWait is a wrapper for nanosleep
        ** which hides the timespec configuration
        ** from the user and allows use with a single
        ** line and parameter (time in nanoseconds).
		**
		** This underlying function is used for precision
		** as well as its ability to be interrupted by a signal.
        */

        struct timespec ts;
        ts.tv_sec  = nanoseconds / 1000000000L;
        ts.tv_nsec = nanoseconds % 1000000000L;
        nanosleep(&ts, NULL);
}

void microWait(uint64_t microseconds)
{
	nanoWait(microseconds*1000);
}

void milliWait(uint64_t milliseconds)
{
	nanoWait(milliseconds*1000000);
}

struct timespec currentTime()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts;
}

unsigned long microSecondsSince(struct timespec * before)
{
	struct timespec now = currentTime();
	int64_t secondComponent = now.tv_sec - before->tv_sec;
	int64_t nanoComponent = now.tv_nsec - before->tv_nsec;
	int64_t nanoSince = secondComponent*1000000000L + nanoComponent;
	return nanoSince/1000;
}

void printTimeBetween(struct timespec * previous, struct timespec * current)
{
	int64_t secondComponent = current->tv_sec - previous->tv_sec;
	int64_t nanoComponent = current->tv_nsec - previous->tv_nsec;
	int64_t nanoSince = secondComponent*1000000000L + nanoComponent;
    double microSince = nanoSince/1000.0;
	printf("Duration between times: %.2f microseconds\n", microSince);
}

// ============================================================================================= //
// Ring Buffer Functions
// ============================================================================================= //

RingBuffer * newRingBuffer(int length)
{
	RingBuffer * rb = (RingBuffer*) malloc(sizeof(RingBuffer));
	rb->length = length;
	rb->buffer = (double *) malloc(length * sizeof(double));
	rb->head = 0;
	rb->tail = 0;
	for (int i = 0; i < length; i++)
	{
		rb->buffer[i] = NAN;
	}
	return rb;
}

int isEmptyRingBuffer(RingBuffer * rb)
{
	return (rb->head == rb->tail);
}

void pushRingBuffer(RingBuffer * rb, double value)
{
	rb->buffer[rb->head] = value;
	rb->head = (rb->head + 1) % rb->length;
	if (rb->head == rb->tail)
	{
		rb->tail = (rb->tail + 1) % rb->length;
	}
}

double getMeanRingBuffer(RingBuffer * rb)
{
	if (isEmptyRingBuffer(rb)) return NAN;
	double sum = 0.0;
	int count = 0;
	int i = rb->tail;
	while (i != rb->head) {
		if (!isnan(rb->buffer[i])) {
			sum += rb->buffer[i];
			count++;
		}
    	i = (i + 1) % rb->length;
	}
	return (count == 0) ? NAN : (sum / count);
}

int qsortCompareDouble(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    if (da < db) return -1;
    else if (da > db) return 1;
    return 0;
}

// Function to get the median of a RingBuffer while excluding NaN values
double getMedianRingBuffer(RingBuffer *rb)
{
    if (rb == NULL || rb->length == 0) {
        return NAN; // Handle empty or invalid ring buffer
    }

    // Temporary array for valid (non-NaN) values
    double validValues[rb->length];
    size_t validCount = 0;

    // Extract valid (non-NaN) values from the ring buffer
    for (size_t i = 0; i < rb->length; i++) {
        size_t index = (rb->length - rb->length + i) % rb->length; // Proper circular indexing
        double value = rb->buffer[index];
        if (!isnan(value)) {
            validValues[validCount++] = value;
        }
    }

    // If no valid values, return NaN
    if (validCount == 0) {
        return NAN;
    }

    // Sort the valid values
    qsort(validValues, validCount, sizeof(double), qsortCompareDouble);

    // Calculate the median
    double median;
    if (validCount % 2 == 0) {
        // Even number of elements: average the two middle values
        median = (validValues[validCount / 2] + validValues[validCount / 2 - 1]) / 2.0;
    } else {
        // Odd number of elements: take the middle value
        median = validValues[validCount / 2];
    }

    return median;
}

void destroyRingBuffer(RingBuffer * rb)
{
	free(rb->buffer);
	free(rb);
	rb = NULL;
}

// ============================================================================================= //
// Fuzzy Logic Functions
// ============================================================================================= //

int fuzzyMatchDouble(double A, double B, double tolerance)
{
	return (A >= B - tolerance) && (A <= B + tolerance);
}


// ============================================================================================= //
// End of File
// ============================================================================================= //