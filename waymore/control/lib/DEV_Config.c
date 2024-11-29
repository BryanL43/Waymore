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

static int DEV_Equipment_Testing(void)
{
	int i;
	int fd;
	char value_str[20];
	fd = open("/etc/issue", O_RDONLY);
    printf("Current environment: ");
	while(1) {
		if (fd < 0) {
			return -1;
		}
		for(i=0;; i++) {
			if (read(fd, &value_str[i], 1) < 0) {
				return -1;
			}
			if(value_str[i] ==32) {
				printf("\r\n");
				break;
			}
			printf("%c",value_str[i]);
		}
		break;
	}

	if(i<5) {
		printf("Unrecognizable\r\n");
        return -1;
	} else {
		char RPI_System[10]   = {"Raspbian"};
		for(i=0; i<6; i++) {
			if(RPI_System[i] != value_str[i]) {
                #if USE_DEV_LIB    
                    return 'J';
                #endif
                return -1;
			}
		}
        return 'R';
	}
	return -1;
}

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
    int Equipment = DEV_Equipment_Testing();
    if(Equipment=='R'){
        INT_PIN = 4;
    }else if(Equipment=='J'){
        #if USE_DEV_LIB
        INT_PIN = GPIO4;
        #endif
    }else{
        printf("Device read failed or unrecognized!!!\r\n");
        while(1);
    }
    
    DEV_GPIO_Mode(INT_PIN, 0);
}

void DEV_SPI_Init()
{
    //printf("BCM2835 SPI Device\r\n");  
    bcm2835_spi_begin();                                         //Start spi interface, set spi pin for the reuse function
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  //spi mode 0
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);  //Frequency
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                     //set CE0
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);     //enable cs0
}

void DEV_SPI_WriteByte(uint8_t Value)
{
    bcm2835_spi_transfer(Value);
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
    uint8_t rData[Len];
    bcm2835_spi_transfernb(pData,rData,Len);
}

void DEV_I2C_Init(uint8_t Add)
{
    //printf("BCM2835 I2C Device\r\n");  
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
    bcm2835_i2c_read_register_rs(&Cmd, rbuf, 1);
    ref = rbuf[0];
    return ref;
}


int I2C_Read_Word(uint8_t Cmd)
{
	int ref;
    char rbuf[2] = {0};
    bcm2835_i2c_read_register_rs(&Cmd, rbuf, 2);
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