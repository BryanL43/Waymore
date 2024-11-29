/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: WaveShare Team / Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: DEV_Config.h
*
* Description:: Modified version of WaveShare's motor demo
*               libaries, adapted to the needs of our project.
*
**************************************************************/

#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include <bcm2835.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

// ============================================================================================= //
// Variables and states
// ============================================================================================= //


// ============================================================================================= //
// Library Functions
// ============================================================================================= //

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);

void DEV_Delay_ms(UDOUBLE xms);

void DEV_I2C_Init(uint8_t Add);

void I2C_Write_Byte(uint8_t Cmd, uint8_t value);

int I2C_Read_Byte(uint8_t Cmd);

int I2C_Read_Word(uint8_t Cmd);

UBYTE DEV_ModuleInit(void);

void DEV_ModuleExit(void);

// ============================================================================================= //
// End of File
// ============================================================================================= //
#endif
