#include <Adafruit_GFX.h>     // Core graphics library
#include "TFT_abstraction_layer.h"
#include <cmath>

#include "Colours.h"

#ifndef Value_Drawer_h
#define Value_Drawer_h

class Value_Drawer {
public:
  Value_Drawer(Adafruit_GFX*);
  void drawPercentageChange(double, double, double, int);
  void drawPrice(double, double, double, int, int);
  void drawSign(double);

private:
  Adafruit_GFX* tft;
};

#endif