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
// Definitions of Constants (private to waymoreLib)
// ============================================================================================= //

// Memory Offset Configuration
typedef enum Offset{
    SEL = 0x00,
    SET = 0x1C,
    CLR = 0x28,
    RD = 0x34,
}Offset;

// MMAP Configuration
#define GPIO_BASE_ADDRESS 0xfe200000
#define MEMORY_BLOCK_SIZE 4096

// ============================================================================================= //
// Internal variables and states
// ============================================================================================= //

// Main gpio mapping
volatile uint32_t * gpio;

// Convert byte offsets to index (integer) offsets
int selectIdx	= SEL >> 2;
int setIdx	    = SET >> 2;
int clearIdx	= CLR >> 2;
int readIdx	    = RD  >> 2;


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

void initializeGPIO()
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
		perror("initializeGPIO: Failed to open /dev/mem");
		exit(1);
	}

	// map the GPIO pins into memory, catching errors
	gpio = (volatile uint32_t *) mmap(NULL,
					  MEMORY_BLOCK_SIZE,
					  PROT_READ | PROT_WRITE,
					  MAP_SHARED,
					  mem,
					  GPIO_BASE_ADDRESS);
	if (gpio == MAP_FAILED)
	{
		perror("initializeGPIO: Memory mapping failed");
		close(mem);
		exit(1);
	}

	printf("done.\n");
	// Close file and return
	close(mem);
}

void uninitializeGPIO()
{
	if(gpio != NULL)
	{
		munmap((void *)gpio, MEMORY_BLOCK_SIZE);
		gpio = NULL;
	}
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
		exit(1);
	}

	// Validate the name
	if(name == NULL) name = "unnamed";
	printf("Starting '%s'\n", name);

	// Allocate memory for the pointer, checking for errors
	Thread * thread = (Thread *) malloc(sizeof(Thread));
	if (thread == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for '%s'\n", name);
		exit(1);
	}

	thread->name = strdup(name);
	if (thread->name == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for name of thread\n");
		free(thread);
		exit(1);
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
		exit(1);
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