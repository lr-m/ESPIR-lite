#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>      // Core graphics library
#include <Adafruit_ST7735.h>   // ST7735 display library
#include <Adafruit_ST7789.h>   // ST7789 display library

#include "Colours.h"

// Enum for the display types
enum TFT_Type {
    ST7735_SPI_128_128,
    ST7735_SPI_160_80,
    ST7735_SPI_160_128,
    ST7789_SPI_240_240,
    ST7789_SPI_320_240,
    ST7789_SPI_320_240_INVERT
};

class TFT {
public:
    // Constructor
    TFT(TFT_Type displayType, int8_t cs, int8_t dc, int8_t sda, int8_t scl, int8_t re);

    // Destructor
    ~TFT();

    // Method to draw a pixel on the display
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    void init();

    // Add more method declarations as needed

    Adafruit_GFX* getDisplay();

private:
    Adafruit_GFX* display;  // Pointer to the base class
    TFT_Type type;
};

#endif // DISPLAY_H
