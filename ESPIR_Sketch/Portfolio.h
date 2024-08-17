/*
  Portfolio.h - Portfolio Interface for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <cmath>

#include "Candle_Chart.h"
#include "Coin.h"
#include "Portfolio_Editor_Page.h"
#include "Colours.h"
#include "Value_Drawer.h"

#define COIN_COUNT 20
#define COIN_PAGE_LIMIT 10

#define DEG2RAD 0.0174532925
#define MODE_COUNT 3

// ensure this library description is only included once
#ifndef Portfolio_h
#define Portfolio_h

enum class DisplayMode {
  BAR,
  PIE,
  CANDLE,
};

class Portfolio {
public:
  Portfolio(Adafruit_ST7735 *, COIN **, Value_Drawer*);
  void display(int);
  void getTotalValue(double *);
  float getFloatValue();
  COIN **coins;
  COIN* sorted_coins[COIN_COUNT];
  DisplayMode display_mode = DisplayMode::BAR;
  void refreshSelectedCoins();
  Candle_Chart *candle_chart;
  void addPriceToCandles();
  void nextTimePeriod();
  void clearCandles();
  void previousMode();
  void nextMode();
  void drawPropBar(double *);
  Value_Drawer* value_drawer;
  void press(int);
  int getElementCount();
  void clear();

private:
  Adafruit_ST7735 *tft;
  void drawValue(double *, int);
  void drawBarSummary(double *, int);
  void drawPieSummary(double *);
  void drawCandleChart(double *, int);
  uint16_t page_base = 0;
  void fillSegment(int x, int y, double startAngle, double endAngle, int r, int innerR, unsigned int colour);
  bool isInsideSegment(int px, int py, int x, int y, double startAngle, double endAngle, int r, int innerR);
};

#endif
