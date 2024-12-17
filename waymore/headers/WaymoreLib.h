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
#include <time.h>
#include <bits/pthreadtypes.h>
#include <signal.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <bcm2835.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>

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

typedef struct RingBuffer
{
    double * buffer;
    int length;
    int head;
    int tail;
}RingBuffer;

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

// Dictionary of definitions for flexibility and clarity elsewhere in code
#define ON		1
#define OFF		0

#define IN		0
#define OUT		1

#define READ	0
#define WRITE	1

#define TRUE	1
#define FALSE	0

// ============================================================================================= //
// GPIO Functions
// ============================================================================================= //

int initializeGPIO();
int uninitializeGPIO();

#ifdef __cplusplus
extern "C" {
#endif

void setPinDirection(int pin, int direction);
void setPinLevel(int pin, int level);

#ifdef __cplusplus
}
#endif

int getPinLevel(int pin);

// ============================================================================================= //
// I2C Functions
// ============================================================================================= //

int initializeI2C();
int uninitializeI2C();
void registerDeviceI2C(uint8_t ADDR);
int readByteI2C(uint8_t ADDR, uint8_t reg);
int readBytesI2C(uint8_t ADDR, char * destbuf, uint32_t len);
int writeByteI2C(uint8_t ADDR, uint8_t reg, uint8_t value);
int writeBytesI2C(uint8_t ADDR, const char * buf, uint32_t len);

// ============================================================================================= //
// Threading Functions
// ============================================================================================= //

Thread * startThread(const char * name, void*(*function)(void *));
void stopThread(Thread * thread);

// ============================================================================================= //
// Time Functions
// ============================================================================================= //

// Interruptable waiting
void nanoWait(uint64_t nanoseconds);
void microWait(uint64_t microseconds);
void milliWait(uint64_t milliseconds);

// Times and Durations
struct timespec currentTime();
unsigned long microSecondsSince(struct timespec * previous);
void printTimeBetween(struct timespec * previous, struct timespec * current);

// ============================================================================================= //
// Ring Buffer Functions
// ============================================================================================= //

RingBuffer * newRingBuffer(int length);
void destroyRingBuffer(RingBuffer * rb);
void pushRingBuffer(RingBuffer * rb, double value);
double getMeanRingBuffer(RingBuffer * rb);
double getMedianRingBuffer(RingBuffer * rb);

// ============================================================================================= //
// Fuzzy Logic Functions
// ============================================================================================= //

int fuzzyMatchDouble(double A, double B, double tolerance);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif