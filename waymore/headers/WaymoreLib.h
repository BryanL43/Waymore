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

#ifndef _WAYMORE_LIB_H_
#define _WAYMORE_LIB_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <pthread.h>

// ============================================================================================= //
// Definitions of Structures
// ============================================================================================= //

typedef struct Thread
{
	char * name;
	pthread_t id;
	int running;
}Thread;

typedef enum RegisterOffset{
    SEL = 0x00,
    SET = 0x1C,
    CLR = 0x28,
    RD = 0x34,
}RegisterOffset;

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

// Dictionary of definitions for flexibility and clarity elsewhere in code
#define ON		1
#define OFF		0

#define HIGH	1
#define LOW		0

#define IN		0
#define OUT		1

#define READ	0
#define WRITE	1

#define TRUE	1
#define FALSE	0

// ============================================================================================= //
// Validation functions
// ============================================================================================= //

void validatePin(int pin);
void validateLevel(int level);
void validateDirection(int direction);

// ============================================================================================= //
// Initialization and Uninitialization Functions
// ============================================================================================= //

int initializeGPIO();
int uninitializeGPIO();


int initializeI2C();
int registerDeviceI2C(uint8_t ADDR);
int uninitializeI2C(uint8_t ADDR);

// ============================================================================================= //
// Primary Functions
// ============================================================================================= //

void setPinDirection(int pin, int direction);
void setPinLevel(int pin, int level);
int getPinLevel(int pin);

int readBytesI2C(uint8_t ADDR, char * destBuffer, uint32_t count)
int writeBytesI2C(uint8_t ADDR, char * sourceBuffer, uint32_t count);

// ============================================================================================= //
// Threading Initialization and Uninitialization Functions
// ============================================================================================= //

Thread * startThread(const char * name, void*(*function)(void *));
void stopThread(Thread * thread);

// =============================================================================== //
// Time Related Functions
// =============================================================================== //

// Interruptable waiting
void nanoWait(uint64_t nanoseconds);
void microWait(uint64_t microseconds);
void milliWait(uint64_t milliseconds);

// Times and Durations
struct timespec currentTime();
unsigned long microSecondsSince(struct timespec * previous);
void printTimeBetween(struct timespec * previous, struct timespec * current);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif