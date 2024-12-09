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
uint8_t currentI2cAddr = -1;
pthread_spinlock_t lock;

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
	if (i2cBus >= 0)
	{
		fprintf(stderr, "The I2C bus has already been initialized!\n");
		return -1;
	}

    // Open the I2C bus
    if ((i2cBus = open(I2CBUS, O_RDWR)) < 0) 
	{
        fprintf(stderr, "Failed to open the I2C bus!\n");
        return -1;
    }

    // Initialize a spinlock to only allow one
    // thread to communicate over the bus at a time
    if (pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE) != 0)
    {
        fprintf(stderr, "Failed to initialize the I2C spinlock!\n", ADDR);
        if(close(i2cBus) < 0)
        {
            fprintf(stderr, "Failed to close I2C bus!\n", ADDR);
            return -1;
        }
        return -1;
    }

	return 0;
}

int registerDeviceI2C(uint8_t ADDR)
{
    // Initialize a device at the given address
    if (ioctl(i2cBus, I2C_SLAVE, ADDR) < 0) {
        fprintf(stderr, "Failed to initialize the I2C device at address %02x!\n", ADDR);
        if(close(i2cBus) < 0)
        {
            fprintf(stderr, "Failed to close the I2C bus!\n", ADDR);
            return -1;
        }
        return -1;
    }

	return 0;
}

int uninitializeI2C(uint8_t ADDR)
{
    if (i2cBus < 0)
    {
        printf("There is no i2cBus open at %02x\n", ADDR);
        return -1;
    }

    if(close(i2cBus) < 0)
    {
        fprintf(stderr, "Failed to close I2C i2cBus at address %02x\n", ADDR);
        if(pthread_spin_destroy(&lock) < 0)
        {
            fprintf(stderr, "Failed to destroy the I2C spinlock!\n", ADDR);
            return -1;
        }
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

int readBytesI2C(uint8_t ADDR, char * destBuffer, uint32_t count)
{
    if (pthread_spin_lock(&lock) != 0)
    {
        // should only occurs if the calling thread already has the lock
        // or if the lock argument is invalid.
        printf("I2C Spinlock error!\n");
        return -1;
    }
    
    if (currentI2cAddr != ADDR)
    {
        // Set the address to the desired device if necessary (void function)
        currentI2cAddr = ADDR;
        bcm2835_i2c_setSlaveAddress(ADDR);
    }
    
	int bytesRead = bcm2835_i2c_read(destBuffer, count);
    if ( bytesRead < count)
    {
        fprintf(stderr, 
            "Failed to read all %d bytes from I2C address %02x!\n",
            count, ADDR);
        return -1;
    }

    if (pthread_spin_unlock(&lock) != 0)
    {
        // should only occurs if the calling thread already has the lock
        // or if the lock argument is invalid.
        fprintf(stderr, "I2C spinlock error: failed to unlock!\n");
        return -1;
    }

	return bytesRead;
}

int writeBytesI2C(uint8_t ADDR, char * sourceBuffer, uint32_t count)
{
    if (pthread_spin_lock(&lock) < != 0)
    {
        // should only occurs if the calling thread already has the lock
        // or if the lock argument is invalid.
        fprintf(stderr, "I2C spinlock failed!\n");
        return -1;
    }

    if(currentI2cAddr != ADDR)
    {
        currentI2cAddr = ADDR;
        bcm2835_i2c_setSlaveAddress(ADDR);
    }

	int bytesWritten = bcm2835_i2c_write(sourceBuffer, count);
    if (bytesWritten < count)
    {
        fprintf(stderr, 
            "Failed to write all %d bytes to I2C address %02x!\n",
            count, ADDR);
        return -1;
    }

    if (pthread_spin_unlock(&lock) != 0)
    {
        // should only occurs if the calling thread already has the lock
        // or if the lock argument is invalid.
        fprintf(stderr, "I2C spinlock error: failed to unlock!\n");
        return -1;
    }

	return bytesWritten;
}


// ============================================================================================= //
// Threading Initialization and Uninitialization Functions
// ============================================================================================= //

Thread * startThread(const char * name, void* (*function) (void *))
{
	// Validate the function
	if(function == NULL)
	{
		fprintf(stderr, "Failed to start %s: "
				"function cannot be null.\n", name);
		return NULL;
	}

	// Validate the name
	if(name == NULL) name = "unnamed";
	printf("Starting '%s'\n", name);

	// Allocate memory for the pointer, checking for errors
	Thread * thread = (Thread *) malloc(sizeof(Thread));
	if (thread == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for '%s'\n", name);
		return NULL;
	}

	thread->name = strdup(name);
	if (thread->name == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for name of thread\n");
		free(thread);
		return NULL;
	}

	// Spin up the thread
	thread->running = 1;
	int res = pthread_create(&thread->id, NULL, function, NULL);
	if (res != 0)
	{
		fprintf(stderr, "Failed to start '%s'\n", thread->name);
		thread->running = 0;
		free(thread->name);
		free(thread);
		return NULL;
	}

	return thread;
}

void stopThread(Thread * thread)
{
	// Perform null check
	if (thread == NULL) return;

	// Set flag
	thread->running = 0;

	// Wait for the thread to complete its task or be stopped
	pthread_join(thread->id, NULL);

	printf("'%s' has finished - freeing its resources\n", thread->name);
	
	// Free the resources
	if (thread->name != NULL) free(thread->name);

	// Free the struct's memory
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
// End of File
// ============================================================================================= //