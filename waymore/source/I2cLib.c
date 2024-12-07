#define BUS "/dev/i2c-1"

int device;

int initializeI2C(uint8_t ADDR)
{
    // Open the I2C bus
    if ((device = open(BUS, O_RDWR)) < 0) {
        fprintf(stderr, "initializeRGB: Failed to open the I2C bus.\n");
        exit(1);
    }

    // Initialize the device at the given address
    if (ioctl(device, I2C_SLAVE, ADDR) < 0) {
        fprintf(stderr, 
                "initializeRGB: Failed to initialize I2C device at address %02x\n", 
                ADDR);
        int res = close(device);
        if(res < 0)
        {
            fprintf(stderr, "Failed to close I2C device at address %02x\n", ADDR);
            exit(1);
        }
        exit(1);
    }
}

int uninitializeI2C(uint8_t ADDR)
{
    if(device >= 0)
    {
        int res = close(device);
        if(res < 0)
        {
            fprintf(stderr, "Failed to close I2C device at address %02x\n", ADDR);
            exit(1);
        }
    }
    else
    {
        printf("There is no device open at %02x\n", ADDR);
    }
}