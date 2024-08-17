/*
  ESPIR_Menu.h - Menu for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "Candle_Chart.h"
#include "Value_Drawer.h"
#include "Colours.h"

#ifndef Coin_h
#define Coin_h

#define COIN_COUNT 20

// Positional constants
#define BM_PRICE_START_X 60
#define NO_BM_PRICE_START_X 32
#define PRICE_START_Y 28

#define BM_CHANGE_START_X 60
#define NO_BM_CHANGE_START_X 35
#define CHANGE_START_Y 50

// For storing coins
class COIN {
public:
  COIN(char *, char *, const unsigned char *, uint16_t, uint16_t, uint16_t,
       double, Value_Drawer *, Adafruit_ST7735 *);
  COIN(char *, char *, uint16_t, double, Value_Drawer *, Adafruit_ST7735 *);

  char coin_code[8] = { 0 };
  char coin_id[31] = { 0 };

  Adafruit_ST7735 *display;

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
  void drawBitmap(int16_t, int16_t, const uint8_t *, int16_t,
                  int16_t, uint16_t);
  void toggleBitmap();
  void clearIdCode();

  void setAmount(double new_amount) {amount = new_amount; amount_changed = true; }
  bool getAmountChanged() { return amount_changed; amount_changed = false; }
};

#endif
