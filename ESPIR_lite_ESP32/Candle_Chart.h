/*
  ST7735_Candle_Chart.h - Menu for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>

#include "TFT_abstraction_layer.h"
#include "Colours.h"

#define CANDLE_WIDTH_128 3
#define CANDLE_WIDTH_160 3
#define CANDLE_WIDTH_320 5

#define CANDLE_COUNT_128 20
#define CANDLE_COUNT_160 26
#define CANDLE_COUNT_240 20
#define CANDLE_COUNT_320 26

#ifndef Candle_Chart_h
#define Candle_Chart_h

// For candle graph element
typedef struct graph_candle {
  float opening;
  float closing;
  float high;
  float low;
} G_CANDLE;

class Candle_Chart {
public:
  Candle_Chart(Adafruit_GFX *, int, int, int);
  void display(int currency = 0);
  void displaySmall(int, int, int, int);
  G_CANDLE *candles;
  uint16_t count;
  uint16_t current_candles;
  int top;
  int bottom;
  int labels;
  void initialiseCandles();
  void nextTimePeriod(float);
  void addPrice(float);
  void freeCandles();
  bool candles_init;
  void reset();

private:
  Adafruit_GFX *tft;
};

#endif
