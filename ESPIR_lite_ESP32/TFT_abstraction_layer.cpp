#include "TFT_abstraction_layer.h"

TFT::TFT(TFT_Type displayType, int8_t cs, int8_t dc, int8_t res) {
    type = displayType;
    switch (displayType) {
        case ST7735_SPI_128_128:
            display = new Adafruit_ST7735(cs, dc, res);
            break;
        case ST7735_SPI_160_80:
            display = new Adafruit_ST7735(cs, dc, res);
            break;
        case ST7735_SPI_160_128:
            display = new Adafruit_ST7735(cs, dc, res);
            break;
        case ST7789_SPI_240_240:
            display = new Adafruit_ST7789(cs, dc, res);
            break;
        case ST7789_SPI_320_240:
            display = new Adafruit_ST7789(cs, dc, res);
            break;
        case ST7789_SPI_320_240_INVERT:
            display = new Adafruit_ST7789(cs, dc, res);
            break;
        default:
            display = nullptr;
            break;
    }
}

TFT::~TFT() {
    if (display) {
        delete display;
    }
}

void TFT::init(){
    switch (type) {
        case ST7735_SPI_128_128:
            static_cast<Adafruit_ST7735*>(display)->initR(INITR_144GREENTAB);
            display->setRotation(1);
            display->fillScreen(BLACK);
            break;
        case ST7735_SPI_160_80:
            static_cast<Adafruit_ST7735*>(display)->initR(INITR_MINI160x80_GREENTAB);
            display->setRotation(1);
            display->invertDisplay(1);
            display->fillScreen(BLACK);
            break;
        case ST7735_SPI_160_128:
            static_cast<Adafruit_ST7735*>(display)->initR(INITR_BLACKTAB);
            display->setRotation(1);
            display->fillScreen(BLACK);
            break;
        case ST7789_SPI_240_240:
            static_cast<Adafruit_ST7789*>(display)->init(240, 240);  // Or use initR if applicable
            display->setRotation(1);
            display->invertDisplay(1);
            display->fillScreen(BLACK);
            break;
        case ST7789_SPI_320_240:
            static_cast<Adafruit_ST7789*>(display)->init(240, 320);  // Or use initR if applicable
            display->setRotation(1);
            display->invertDisplay(0);
            display->fillScreen(BLACK);
            break;
        case ST7789_SPI_320_240_INVERT:
            static_cast<Adafruit_ST7789*>(display)->init(240, 320);  // Or use initR if applicable
            display->setRotation(1);
            display->invertDisplay(1);
            display->fillScreen(BLACK);
            break;
        default:
            break;
    }
}

Adafruit_GFX* TFT::getDisplay() {
    return display;
}

