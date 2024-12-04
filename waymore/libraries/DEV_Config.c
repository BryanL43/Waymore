/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: WaveShare Team / Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: DEV_Config.c
*
* Description:: Implementation of modified version of
*               WaveShare's motor demo libaries, adapted
*               to the needs of our project.
*
**************************************************************/
// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include "DEV_Config.h"

// ============================================================================================= //
// Definitions of Constants
// ============================================================================================= //

#define DEV_SPI 0
#define DEV_I2C 1

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

// ============================================================================================= //
// Variables and states
// ============================================================================================= //

extern int INT_PIN; //4
uint32_t fd;
int INT_PIN;

// ============================================================================================= //
// Library Functions
// ============================================================================================= //

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    /*
        0:  INPT   
        1:  OUTP
    */
    if(Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT){
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
    }else {
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
    }
}

void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    bcm2835_gpio_write(Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    UBYTE Read_value = 0;
    Read_value = bcm2835_gpio_lev(Pin);
    return Read_value;
}


void DEV_Delay_ms(UDOUBLE xms)
{
    bcm2835_delay(xms);
}


void GPIO_Config(void)
{
    DEV_GPIO_Mode(4, 0);
}

void DEV_I2C_Init(uint8_t Add)
{
    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(Add);
}

void I2C_Write_Byte(uint8_t Cmd, uint8_t value)
{
    char wbuf[2]={Cmd, value};
    bcm2835_i2c_write(wbuf, 2);
}

int I2C_Read_Byte(uint8_t Cmd)
{
	int ref;
    char rbuf[2]={0};
    bcm2835_i2c_read_register_rs((char*)&Cmd, rbuf, 1);
    ref = rbuf[0];
    return ref;
}


int I2C_Read_Word(uint8_t Cmd)
{
	int ref;
    char rbuf[2] = {0};
    bcm2835_i2c_read_register_rs((char*)&Cmd, rbuf, 2);
    ref = rbuf[1]<<8 | rbuf[0];
    return ref;
}

UBYTE DEV_ModuleInit(void)
{
    if(!bcm2835_init()) {
        printf("DEV_ModuleInit: bcm2835 init failed!\r\n");
        return 1;
    }
    GPIO_Config();
    DEV_I2C_Init(0x29);
    
    return 0;
}

void DEV_ModuleExit(void)
{
    bcm2835_i2c_end();
    bcm2835_close();
}

// ============================================================================================= //
// End of File
// ============================================================================================= //