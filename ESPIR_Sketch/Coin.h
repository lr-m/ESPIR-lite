/*
  ESPIR_Menu.h - Menu for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>

#include "TFT_abstraction_layer.h"
#include "Candle_Chart.h"
#include "Value_Drawer.h"
#include "Colours.h"

#ifndef Coin_h
#define Coin_h

#define COIN_COUNT 20

// Positional constants for 128 width
#define NO_BM_NAME_START_X_128 24
#define NO_BM_PRICE_START_X_128 24
#define NO_BM_CHANGE_START_X_128 22

#define BM_NAME_START_X_128 44
#define BM_PRICE_START_X_128 44
#define BM_CHANGE_START_X_128 20

#define CHANGE_START_Y_128 48
#define PRICE_START_Y_128 28

// Positional constants for 160 width
#define NO_BM_NAME_START_X_160 32
#define NO_BM_PRICE_START_X_160 32
#define NO_BM_CHANGE_START_X_160 35

#define BM_NAME_START_X_160 60
#define BM_PRICE_START_X_160 60
#define BM_CHANGE_START_X_160 60

#define CHANGE_START_Y_160 50
#define PRICE_START_Y_160 28

// Positional constants for 320 width
#define NO_BM_NAME_START_X_320 54
#define NO_BM_PRICE_START_X_320 64
#define NO_BM_CHANGE_START_X_320 70

#define BM_NAME_START_X_320 110
#define BM_PRICE_START_X_320 120
#define BM_CHANGE_START_X_320 120

#define CHANGE_START_Y_320 94
#define PRICE_START_Y_320 56

// candle counts
#define CANDLE_COUNT_128 20
#define CANDLE_COUNT_160 26
#define CANDLE_COUNT_320 26

// For storing coins
class COIN {
public:
  COIN(char *, char *, const unsigned char *, uint16_t, uint16_t, uint16_t,
       double, Value_Drawer *, Adafruit_GFX *);
  COIN(char *, char *, uint16_t, double, Value_Drawer *, Adafruit_GFX *);

  char coin_code[8] = { 0 };
  char coin_id[31] = { 0 };

  Adafruit_GFX *tft;

  // the values that we display on the coin display
  double current_price;
  double current_change;
  Candle_Chart *candles;
  const unsigned char *bitmap;

  // amount of this coin we own (for portfolio)
  double amount;

  // coin colours
  uint16_t circle_colour;
  uint16_t bm_colour;
  uint16_t portfolio_colour;

  // some status stuff
  bool candles_init;

  Value_Drawer *value_drawer;

  bool amount_changed = false;

  void freeCandles();
  void initCandles();

  void draw(int currency, bool bitmap_enabled);
  void drawPercentageChange(bool bitmap_enabled);
  void drawName(bool bitmap_enabled);
  void drawPrice(int currency, bool bitmap_enabled);
  void drawBitmap(int16_t, int16_t, const uint8_t *, int16_t,
                  int16_t, uint16_t);
  void drawBitmapDouble(int16_t, int16_t, const uint8_t *, int16_t,
                  int16_t, uint16_t);
  void toggleBitmap();
  void clearIdCode();

  void setAmount(double new_amount) {amount = new_amount; amount_changed = true; }
  bool getAmountChanged() { return amount_changed; amount_changed = false; }
};

#endif
