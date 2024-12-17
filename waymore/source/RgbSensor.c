/**************************************************************
* Class:: CSC-615-01 Spring 2024
* Name:: Waymore Team
* Student ID:: ...
* Github-Name:: ...
* Project:: Final Project
*
* File:: RgbSensor.c
*
* Description:: Implementations of functions and wrappers
*		        for TCS34725 RGB sensor functionality.
*
**************************************************************/

// ============================================================================================= //
// Library Linking
// ============================================================================================= //

#include "WaymoreLib.h"
#include "RgbSensor.h"

// ============================================================================================= //
// Internal Variables & States
// ============================================================================================= //

// Device config
int readRegister = 0x94;
enum TCS34725_Atime integrationTime;

// Storage
HSVData * hsvData;

// ============================================================================================= //
// Private functions
// ============================================================================================= //

void waitForIntegration()
{
    switch (integrationTime)
    {
        case INTEGRATE2_4MS:
            usleep(2400);
            break;
        case INTEGRATE24MS:
            usleep(24000);
            break;
        case INTEGRATE50MS:
            usleep(50000);
            break;
        case INTEGRATE101MS:
            usleep(101000);
            break;
        case INTEGRATE154MS:
            usleep(154000);
            break;
        case INTEGRATE700MS:
            usleep(700000);
            break;
    }
}

void configureRGB(enum TCS34725_Atime atime, enum TCS34725_Gain gain)
{
    // Initialize a temp variable to store hex configuration settings
    char config[2] = {0};

    // Put Enable register (0x80) into config[0]
    config[0] = 0x80;
    config[1] = 0x03;
    writeBytesI2C(RGBADDR, config, 2);

    integrationTime = atime;
    // Put ALS time register (0x81) into config[0]
    config[0] = 0x81;
    // Set the integration time
    config[1] = atime;
    // Write 2 bytes of data to the RGBADDR
    writeBytesI2C(RGBADDR, config, 2);

    // Put Wait Time register (0x83) into config[0]
    config[0] = 0x83;
    // Set the wait time to 2.4 ms
    config[1] = 0xFF;
    // Write 2 bytes of data to the RGBADDR
    writeBytesI2C(RGBADDR, config, 2);

    // Put Control register(0x8F) into config[0]
    config[0] = 0x8F;
    // Set the gain to 4x
    config[1] = gain;
    // Write 2 bytes of data to the RGBADDR
    writeBytesI2C(RGBADDR, config, 2);
}

// ============================================================================================= //
// Public Functions
// ============================================================================================= //

void initializeRGB(enum TCS34725_Atime integrationTime, enum TCS34725_Gain gain)
{
    hsvData = malloc(sizeof(HSVData));
    if (hsvData == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for RGB sensor data (HSV). Exiting.\n");
        exit(1);
    }

    registerDeviceI2C(RGBADDR);

    // Configure the RGB sensor
    configureRGB(integrationTime, gain);

    // Wait for sensor initialization
    usleep(500000);
}

RGB collectRawReading()
{
    if (RGBADDR < 0)
    {
        fprintf(stderr, "RGB RGBADDR has not been initialized!\n");
        exit(1);
    }

    // Init an array to store raw data
    char data[8] = {0};

    // Trigger a read by writing to the read register
    writeByteI2C(RGBADDR, readRegister, 1);

    // Wait for the integration time to pass
    waitForIntegration();

    // Read the results
    readBytesI2C(RGBADDR, data, 8);

    // Put the data into the correct bins
    uint16_t clear = (data[1] << 8) + data[0];
    uint16_t red   = (data[3] << 8) + data[2];
    uint16_t green = (data[5] << 8) + data[4];
    uint16_t blue  = (data[7] << 8) + data[6];

    // Calculate the RGB values
    int r = ((float)red / clear) * 255.0;
    int g = ((float)green / clear) * 255.0;
    int b = ((float)blue / clear) * 255.0;
    
    // Bound all readings from 0 to 255
    r = r > 255 ? 255 : ((r < 0) ? 0 : r);
    g = g > 255 ? 255 : ((g < 0) ? 0 : g);
    b = b > 255 ? 255 : ((b < 0) ? 0 : b);

    // Contruct the RGB struct and return
    RGB rawReading;

    rawReading.red = (uint8_t) r;
    rawReading.green = (uint8_t) g;
    rawReading.blue = (uint8_t) b;

    return rawReading;
}

void setHSVData(RGB reading)
{
    // convert to HSV
    // acceptable degrees for red: greater than 330 (pink) or less than 30 (orange)
    // we don't really need S or V, we just need H (hue)
    // we're keeping it because it's good for checking

    // divide r,g,b by 255 to change the range from 0..255 to 0.0...1.0 
    double r = reading.red / 255.0;
    double g = reading.green / 255.0;
    double b = reading.blue / 255.0;

    double cmax = fmax(r, fmax(g, b)); // maximum of r, g, b 
    double cmin = fmin(r, fmin(g, b)); // minimum of r, g, b 
    double diff = cmax - cmin; // diff of cmax and cmin. 
    double h = -1, s = -1; 

    // if cmax and cmax are equal then h = 0 
    if (cmax == cmin) 
        h = 0; 
  
    // if cmax equal r then compute h 
    else if (cmax == r) 
        h = 60 * ((g - b) / diff) + 360 % 360; 
  
    // if cmax equal g then compute h 
    else if (cmax == g) 
        h = fmod(60 * ((b - r) / diff) + 120, 360); 
  
    // if cmax equal b then compute h 
    else if (cmax == b) 
        h = fmod(60 * ((r - g) / diff) + 240, 360); 
  
    // if cmax equal zero 
    if (cmax == 0) 
        s = 0; 
    else
        s = (diff / cmax) * 100; 
  
    // compute v 
    double v = cmax * 100; 

    hsvData->hue = h;
    hsvData->saturation = s;
    hsvData->value = v;
}

void colorMatch()
{
    double thresh = 5.0;

    if (fuzzyMatchDouble(hsvData->hue, GREEN, thresh))
    {
        strcpy(hsvData->colorName, "Green");
    }
    else if (fuzzyMatchDouble(hsvData->hue, BLUE, thresh))
    {
        strcpy(hsvData->colorName, "Blue");
    }
    else if (fuzzyMatchDouble(hsvData->hue, YELLOW, thresh))
    {
        strcpy(hsvData->colorName, "Yellow");
    }
    else if (fuzzyMatchDouble(hsvData->hue, REDLOW, thresh)
       || fuzzyMatchDouble(hsvData->hue, REDHIGH, thresh))
    {
        strcpy(hsvData->colorName, "Red");
    }
    else // Naaaaahhh
    {
        strcpy(hsvData->colorName, "I dunno");
    }
}

HSVData readColor()
{
    RGB rgb = collectRawReading();
    setHSVData(rgb);
    colorMatch();
    return *hsvData;
}

void uninitializeRGB()
{
    free(hsvData);
    hsvData = NULL;
}

// ============================================================================================= //
// End of File
// ============================================================================================= //