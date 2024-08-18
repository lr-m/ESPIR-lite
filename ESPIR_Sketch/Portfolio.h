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

// for pie chart represenation
#define TEXT_SIZE 1
#define TEXT_X_OFFSET 7
#define TEXT_Y_OFFSET 28
#define TEXT_Y_SPACING 10
#define PERCENTAGE_TEXT_X_OFFSET 40
#define COLOR_RECT_X 2
#define COLOR_RECT_WIDTH 2

// for bar representation
#define VALUE_TEXT_X_OFFSET_128 40
#define PERFORMANCE_TEXT_X_OFFSET_128 90

#define VALUE_TEXT_X_OFFSET_160 52
#define PERFORMANCE_TEXT_X_OFFSET_160 123

// for pie chart size/positioning on other display widths
#define PIE_CHART_CENTER_X_OFFSET_128 35
#define PIE_CHART_CENTER_Y_OFFSET_128 5
#define PIE_CHART_RADIUS_OUTER_128 24
#define PIE_CHART_RADIUS_INNER_128 12

#define PIE_CHART_CENTER_X_OFFSET_160 41
#define PIE_CHART_CENTER_Y_OFFSET_160 5
#define PIE_CHART_RADIUS_OUTER_160 32
#define PIE_CHART_RADIUS_INNER_160 16

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
