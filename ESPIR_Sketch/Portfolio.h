/*
  Portfolio.h - Portfolio Interface for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <cmath>

#include "Candle_Chart.h"
#include "Coin.h"
#include "Portfolio_Editor_Page.h"
#include "Colours.h"
#include "Value_Drawer.h"

#include "TFT_abstraction_layer.h"

#define COIN_COUNT 20
#define COIN_PAGE_LIMIT 10

#define DEG2RAD 0.0174532925
#define MODE_COUNT 3

// ensure this library description is only included once
#ifndef Portfolio_h
#define Portfolio_h

// define portfolio text sizes
#define TEXT_SIZE_128 1
#define TEXT_SIZE_160 1
#define TEXT_SIZE_320 2

#define BAR_THICKNESS_128 3
#define BAR_THICKNESS_160 3
#define BAR_THICKNESS_320 4

#define BAR_Y_128 21
#define BAR_Y_160 21
#define BAR_Y_320 34

// define percentage represenation offsets
#define TEXT_X_OFFSET_128 7
#define TEXT_Y_OFFSET_128 28
#define TEXT_Y_SPACING_128 10
#define PERCENTAGE_TEXT_X_OFFSET_128 40
#define COLOR_RECT_X_128 2
#define COLOR_RECT_WIDTH_128 2
#define PAGE_LIMIT_128 10

#define TEXT_X_OFFSET_160 7
#define TEXT_Y_OFFSET_160 28
#define TEXT_Y_SPACING_160 10
#define PERCENTAGE_TEXT_X_OFFSET_160 40
#define COLOR_RECT_X_160 2
#define COLOR_RECT_WIDTH_160 2
#define PAGE_LIMIT_160 10

// for 160x80
#define PAGE_LIMIT_160_80 5

#define TEXT_X_OFFSET_320 7
#define TEXT_Y_OFFSET_320 42
#define TEXT_Y_SPACING_320 20
#define PERCENTAGE_TEXT_X_OFFSET_320 90
#define COLOR_RECT_X_320 1
#define COLOR_RECT_WIDTH_320 3
#define PAGE_LIMIT_320 10

#define PERCENTAGE_TEXT_X_OFFSET_240 70

// for bar representation
#define VALUE_TEXT_X_OFFSET_128 40
#define PERFORMANCE_TEXT_X_OFFSET_128 90

#define VALUE_TEXT_X_OFFSET_160 52
#define PERFORMANCE_TEXT_X_OFFSET_160 123

#define VALUE_TEXT_X_OFFSET_240 64
#define PERFORMANCE_TEXT_X_OFFSET_240 165

#define VALUE_TEXT_X_OFFSET_320 104
#define PERFORMANCE_TEXT_X_OFFSET_320 246

// for pie chart size/positioning on other display widths
#define PIE_CHART_CENTER_X_OFFSET_128 35
#define PIE_CHART_CENTER_Y_OFFSET_128 5
#define PIE_CHART_RADIUS_OUTER_128 24
#define PIE_CHART_RADIUS_INNER_128 12

#define PIE_CHART_CENTER_X_OFFSET_160 41
#define PIE_CHART_CENTER_Y_OFFSET_160 5
#define PIE_CHART_RADIUS_OUTER_160 32
#define PIE_CHART_RADIUS_INNER_160 16

#define PIE_CHART_CENTER_X_OFFSET_160_80 41
#define PIE_CHART_CENTER_Y_OFFSET_160_80 9
#define PIE_CHART_RADIUS_OUTER_160_80 26
#define PIE_CHART_RADIUS_INNER_160_80 13

#define PIE_CHART_CENTER_X_OFFSET_240 64
#define PIE_CHART_CENTER_Y_OFFSET_240 10
#define PIE_CHART_RADIUS_OUTER_240 48
#define PIE_CHART_RADIUS_INNER_240 24

#define PIE_CHART_CENTER_X_OFFSET_320 76
#define PIE_CHART_CENTER_Y_OFFSET_320 10
#define PIE_CHART_RADIUS_OUTER_320 64
#define PIE_CHART_RADIUS_INNER_320 36

// for candle chart
#define CANDLE_OFFSET_Y_128 36
#define CANDLE_OFFSET_Y_160 36
#define CANDLE_OFFSET_Y_320 48

enum class DisplayMode {
  BAR,
  PIE,
  CANDLE,
};

class Portfolio {
public:
  Portfolio(Adafruit_GFX *, COIN **, Value_Drawer*);
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
  Adafruit_GFX *tft;
  void drawValue(double *, int);
  void drawBarSummary(double *, int);
  void drawPieSummary(double *);
  void drawCandleChart(double *, int);
  uint16_t page_base = 0;
  uint16_t coin_page_limit = PAGE_LIMIT_160;
};

#endif
