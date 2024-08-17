#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <cmath>

#include "Colours.h"

#ifndef Value_Drawer_h
#define Value_Drawer_h

class Value_Drawer {
public:
  Value_Drawer(Adafruit_ST7735*);
  void drawPercentageChange(double, double, double, int);
  void drawPrice(double, double, double, int, int);
  void drawSign(double);

private:
  Adafruit_ST7735* display;
};

#endif