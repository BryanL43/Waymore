#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>


// uint16_t Adafruit_TCS34725::read16(uint8_t reg) {
//   uint8_t buffer[2] = {TCS34725_COMMAND_BIT | reg, 0}; // 0x80
//   i2c_dev->write_then_read(buffer, 1, buffer, 2);
//   return (uint16_t(buffer[1]) << 8) | (uint16_t(buffer[0]) & 0xFF);
// }

// void Adafruit_TCS34725::getRawData(uint16_t *r, uint16_t *g, uint16_t *b,
//                                    uint16_t *c) {
//   if (!_tcs34725Initialised)
//     begin();

//   *c = read16(TCS34725_CDATAL); // 0x14
//   *r = read16(TCS34725_RDATAL); // 0x16
//   *g = read16(TCS34725_GDATAL); // 0x18
//   *b = read16(TCS34725_BDATAL); // 0x1A

//   /* Set a delay for the integration time */
//   /* 12/5 = 2.4, add 1 to account for integer truncation */
//   delay((256 - _tcs34725IntegrationTime) * 12 / 5 + 1);
// }

// void Adafruit_TCS34725::getRGB(float *r, float *g, float *b) {
//   uint16_t red, green, blue, clear;
//   getRawData(&red, &green, &blue, &clear);
//   uint32_t sum = clear;

//   // Avoid divide by zero errors ... if clear = 0 return black
//   if (clear == 0) {
//     *r = *g = *b = 0;
//     return;
//   }

//   *r = (float)red / sum * 255.0;
//   *g = (float)green / sum * 255.0;
//   *b = (float)blue / sum * 255.0;
// }


// config and device as private variables in .h

// open file at start of thread loop

// init 
// get raw data (uses read16)
    // refresh struct of private variables the brain calls
// convert raw data to rgb

// convert rgb to a color with a confidence interval

int main()
{
    // Create I2C bus
    int device;
    char *bus = "/dev/i2c-1";
    if ((device = open(bus, O_RDWR)) < 0)
    {
        printf("Failed to open the bus. \n");
        exit(1);
    }
    // Get I2C device, TCS34725 I2C address is 0x29
    ioctl(device, I2C_SLAVE, 0x29);

    // Select enable register(0x80)
    // Power ON, RGBC enable, wait time disable(0x03)
    char config[2] = {0};
    config[0] = 0x80;
    config[1] = 0x03;
    write(device, config, 2);

    // Select ALS time register(0x81)
    // Atime = 700 ms(0x00)
    // Atime = time spent reading
    config[0] = 0x81;
    config[1] = 0xF6;
    write(device, config, 2);

    // Select Wait Time register(0x83)
    // WTIME : 2.4ms(0xFF) 
    config[0] = 0x83;
    config[1] = 0xFF;
    write(device, config, 2);

    // Select control register(0x8F)
    // A GAIN = 1x(0x00)
    // Again = sensitivity multiplier
    config[0] = 0x8F;
    config[1] = 0x10;
    write(device, config, 2);
    sleep(1);

    // Read 8 bytes of data from register(0x94)
    // lsb/msb = least/most significant digit
    // cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
    char reg[1] = {0x94};
    write(device, reg, 1);
    char data[8] = {0};

    if(read(device, data, 8) != 8)
    {
        printf("Error : Input/output Error \n");

        close(device);

        return 1;
    }
    // printf("data: %d,%d,%d,%d,%d,%d,%d,%d\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);


    // Convert the data
    uint16_t clear = (data[1] * 256 + data[0]);
    uint16_t red = (data[3] * 256 + data[2]);
    uint16_t green = (data[5] * 256 + data[4]);
    uint16_t blue = (data[7] * 256 + data[6]);

    uint32_t sum = clear;
    float r, g, b;

    // Avoid divide by zero errors ... if clear = 0 return black
    if (clear == 0) {
        r = g = b = 0;
        close(device);
        return -1;
    }

    // Check for possible saturation
    if (red == 0xFFFF || green == 0xFFFF || blue == 0xFFFF) {
        printf("Warning: One or more channels are saturated.\n");
    }

    // printf("Red color luminance : %d lux \n", red);
    // printf("Green color luminance : %d lux \n", green);
    // printf("Blue color luminance : %d lux \n", blue);
    // printf("(clear) IR luminance : %d lux \n", clear);

    // sum desired / sum current
    // float max_sum = 65535;  
    // float calibration_factor = max_sum / sum;
    float calibration_factor = 1;

    printf("\ncalibration factor: %f\n\n", calibration_factor);

    r = ((float)red / sum) * 255.0 * calibration_factor;
    g = ((float)green / sum) * 255.0 * calibration_factor;
    b = ((float)blue / sum) * 255.0 * calibration_factor;

    // Cap the values to 0-255
    r = (r > 255.0) ? 255.0 : ((r < 0) ? 0 : r);
    g = (g > 255.0) ? 255.0 : ((g < 0) ? 0 : g);
    b = (b > 255.0) ? 255.0 : ((b < 0) ? 0 : b);

    printf("Hex color: #%02x%02x%02x\n", (uint8_t)r, (uint8_t)g, (uint8_t)b);
    printf("%d, %d, %d\n", (uint8_t)r, (uint8_t)g, (uint8_t)b);

    /*

    printf("MOVE SENSOR NOW\n\n\n");
    sleep(4);

    if(read(device, data, 8) != 8)
    {
        printf("Error : Input/output Error \n");

        close(device);

        return 1;
    }


    // Convert the data
    clear = (data[1] * 256 + data[0]);
    red = (data[3] * 256 + data[2]);
    green = (data[5] * 256 + data[4]);
    blue = (data[7] * 256 + data[6]);

    sum = clear;

    // Avoid divide by zero errors ... if clear = 0 return black
    if (clear == 0) {
        r = g = b = 0;
        close(device);
        return -1;
    }

    r = (((float)red / sum) * 255.0) * calibration_factor;
    g = (((float)green / sum) * 255.0) * calibration_factor;
    b = (((float)blue / sum) * 255.0) * calibration_factor;


    // Cap the values to 0-255
    r = (r > 255.0) ? 255.0 : ((r < 0) ? 0 : r);
    g = (g > 255.0) ? 255.0 : ((g < 0) ? 0 : g);
    b = (b > 255.0) ? 255.0 : ((b < 0) ? 0 : b);

    printf("Hex color: #%02x%02x%02x\n", (uint8_t)r, (uint8_t)g, (uint8_t)b);
    printf("%d, %d, %d\n", (uint8_t)r, (uint8_t)g, (uint8_t)b);


    */


    close(device);
    return 0;
}