#include "Coin.h"
#include <cstring>

// Constructor for Coin with bitmap
COIN::COIN(char* code, char* id, const unsigned char* bm, uint16_t circle_col,
           uint16_t bm_col, uint16_t portfolio_col, double coin_amount,
           Value_Drawer* drawer, Adafruit_GFX* tft)
  : bitmap(bm), circle_colour(circle_col), bm_colour(bm_col), portfolio_colour(portfolio_col), amount(coin_amount), value_drawer(drawer), tft(tft) {

  clearIdCode();  // Refresh identifiers

  // copy code and id
  strncpy(coin_code, code, 7);
  strncpy(coin_id, id, 30);

  // set properties
  candles_init = false;
  current_change = 0;
  current_price = 0;

  initCandles();
}

// Constructor for coin without bitmap
COIN::COIN(char* code, char* id, uint16_t portfolio_col,
           double coin_amount, Value_Drawer* drawer, Adafruit_GFX* tft)
  : portfolio_colour(portfolio_col), amount(coin_amount), value_drawer(drawer), tft(tft) {

  clearIdCode();  // Refresh identifiers

  // copy code and id
  strncpy(coin_code, code, 7);
  strncpy(coin_id, id, 30);

  portfolio_colour = portfolio_col;
  amount = coin_amount;

  value_drawer = drawer;

  current_change = 0;
  current_price = 0;

  initCandles();
}

void COIN::clearIdCode() {
  for (int i = 0; i < 8; i++) {
    coin_code[i] = '\0';
  }

  for (int i = 0; i < 31; i++) {
    coin_id[i] = '\0';
  }
}

// Free the candles
void COIN::freeCandles() {
  if (candles_init) {
    candles_init = false;
    candles->freeCandles();
    free(candles);
  }
}

// Initialise the candle graph
void COIN::initCandles() {
  if (!candles_init) {
    candles_init = true;
    // special case for 160x80 display
    if (tft->width() == 160 && tft->height() == 80){
      candles = new Candle_Chart(tft, tft->height() - 27,
                              tft->height() - 3, 0);
    } else {
      candles = new Candle_Chart(tft, tft->height() / 2,
                              tft->height() - 6, 0);
    }
  }
}

// Display properties of coin on display
void COIN::draw(int currency, bool bitmap_enabled) {
  tft->setTextColor(WHITE);
  tft->fillRect(0, 0, tft->width(), tft->height(), BLACK);

  if (bitmap_enabled) {
    if (tft->width() == 128){
      tft->fillRoundRect(0, 0, 42, 42, 5, circle_colour);
      drawBitmap(1, 1, bitmap, 40, 40, bm_colour);
    } else if (tft->width() == 160) {
      if (tft->height() == 80){
        tft->fillCircle(10, 10, 40, circle_colour);
        drawBitmap(2, 2, bitmap, 40, 40, bm_colour);
      } else {
        tft->fillCircle(12, 12, 44, circle_colour);
        drawBitmap(4, 4, bitmap, 40, 40, bm_colour);
      }
    } else if (tft->width() == 240) {
      tft->fillCircle(10, 10, 44, circle_colour);
      drawBitmap(4, 4, bitmap, 40, 40, bm_colour);
    } else if (tft->width() == 320) {
      tft->fillCircle(20, 20, 88, circle_colour);
      drawBitmapDouble(4, 4, bitmap, 40, 40, bm_colour);
    }
  }

  drawName(bitmap_enabled);
  drawPrice(currency, bitmap_enabled);
  drawPercentageChange(bitmap_enabled);
  candles->display();
}

// Draws the name of the coin on the screen.
void COIN::drawName(bool bitmap_enabled) {
  tft->setTextSize(2);

  int len = 0;
  for (len = 0; len < 8; len++) {
    if (coin_code[len] == 0)
      break;
  }

  if (tft->width() == 128){
    if (bitmap_enabled) {
      tft->setCursor(
        BM_NAME_START_X_128 + (((tft->width() - BM_NAME_START_X_128) / 8) * (8 - len)) / 2,
        6);
    } else {
      tft->setCursor(
        NO_BM_NAME_START_X_128 + (((tft->width() - 2 * NO_BM_NAME_START_X_128) / 8) * (8 - len)) / 2,
        6);
    }
  } else if (tft->width() == 160) {
    if (bitmap_enabled) {
      tft->setCursor(
        BM_NAME_START_X_160 + (((tft->width() - BM_NAME_START_X_160) / 8) * (8 - len)) / 2,
        6);
    } else {
      tft->setCursor(
        NO_BM_NAME_START_X_160 + (((tft->width() - 2 * NO_BM_NAME_START_X_160) / 8) * (8 - len)) / 2,
        6);
    }
  } else if (tft->width() == 240){
    tft->setTextSize(5);
    if (bitmap_enabled) {
      tft->setCursor(
        BM_NAME_START_X_240 + (((tft->width() - BM_NAME_START_X_240) / 8) * (8 - len)) / 2,
        6);
    } else {
      tft->setCursor(
        NO_BM_NAME_START_X_240 + (((tft->width() - 2 * NO_BM_NAME_START_X_240) / 8) * (8 - len)) / 2,
        6);
    }
  } else if (tft->width() == 320){
    tft->setTextSize(5);
    if (bitmap_enabled) {
      tft->setCursor(
        BM_NAME_START_X_320 + (((tft->width() - BM_NAME_START_X_320) / 8) * (8 - len)) / 2,
        6);
    } else {
      tft->setCursor(
        NO_BM_NAME_START_X_320 + (((tft->width() - 2 * NO_BM_NAME_START_X_320) / 8) * (8 - len)) / 2,
        6);
    }
  }

  tft->print(coin_code);
}

// draw the price on the screen
void COIN::drawPrice(int currency, bool bitmap_enabled){
  if (tft->width() == 128){
    if (bitmap_enabled) {
      // if 5 digits when rounded to int, will end in '.' and look weird, so offset slightly
      if (current_price < 100000 && current_price >= 10000){
        tft->setCursor(BM_PRICE_START_X_128 + 5, PRICE_START_Y_128);
      } else {
        tft->setCursor(BM_PRICE_START_X_128, PRICE_START_Y_128);
      }
    } else {
      tft->setCursor(NO_BM_PRICE_START_X_128, PRICE_START_Y_128);
    }

    value_drawer->drawPrice(6, current_price, 6, 2, currency);
  } else if (tft->width() == 160) {
    if (tft->height() == 80){
      if (bitmap_enabled) {
        tft->setCursor(BM_PRICE_START_X_160, PRICE_START_Y_160_80);
      } else {
        tft->setCursor(NO_BM_PRICE_START_X_160, PRICE_START_Y_160_80);
      }
    } else {
      if (bitmap_enabled) {
        tft->setCursor(BM_PRICE_START_X_160, PRICE_START_Y_160);
      } else {
        tft->setCursor(NO_BM_PRICE_START_X_160, PRICE_START_Y_160);
      }
    }

    value_drawer->drawPrice(7, current_price, 7, 2, currency);
  } else if (tft->width() == 240){
    if (bitmap_enabled) {
      tft->setCursor(BM_PRICE_START_X_240, PRICE_START_Y_240);
    } else {
      tft->setCursor(NO_BM_PRICE_START_X_240, PRICE_START_Y_240);
    }

    value_drawer->drawPrice(7, current_price, 7, 4, currency);
  } else if (tft->width() == 320){
    if (bitmap_enabled) {
      tft->setCursor(BM_PRICE_START_X_320, PRICE_START_Y_320);
    } else {
      tft->setCursor(NO_BM_PRICE_START_X_320, PRICE_START_Y_320);
    }

    value_drawer->drawPrice(7, current_price, 7, 4, currency);
  }
}

// Draws the percentage change on the screen.
void COIN::drawPercentageChange(bool bitmap_enabled) {

  if (tft->width() == 128){
    if (bitmap_enabled) {
      tft->setCursor(BM_CHANGE_START_X_128, CHANGE_START_Y_128);
    } else {
      tft->setCursor(NO_BM_CHANGE_START_X_128, CHANGE_START_Y_128);
    }

    tft->setTextSize(1);
    tft->print("24 Hour: ");

    value_drawer->drawPercentageChange(4, current_change, 2, 1);
  } else if (tft->width() == 160) {
    if (tft->height() == 80){
      if (bitmap_enabled) {
        tft->setCursor(BM_CHANGE_START_X_160, CHANGE_START_Y_160_80);
      } else {
        tft->setCursor(NO_BM_CHANGE_START_X_160, CHANGE_START_Y_160_80);
      }
    } else {
      if (bitmap_enabled) {
        tft->setCursor(BM_CHANGE_START_X_160, CHANGE_START_Y_160);
      } else {
        tft->setCursor(NO_BM_CHANGE_START_X_160, CHANGE_START_Y_160);
      }
    }

    tft->setTextSize(1);
    tft->print("24 Hour: ");

    value_drawer->drawPercentageChange(4, current_change, 2, 1);
  } else if (tft->width() == 240){
    if (bitmap_enabled) {
      tft->setCursor(BM_CHANGE_START_X_240, CHANGE_START_Y_240);
    } else {
      tft->setCursor(NO_BM_CHANGE_START_X_240, CHANGE_START_Y_240);
    }

    tft->setTextSize(2);
    tft->print("24 Hour: ");

    value_drawer->drawPercentageChange(4, current_change, 2, 2);
  } else if (tft->width() == 320){
    if (bitmap_enabled) {
      tft->setCursor(BM_CHANGE_START_X_320, CHANGE_START_Y_320);
    } else {
      tft->setCursor(NO_BM_CHANGE_START_X_320, CHANGE_START_Y_320);
    }

    tft->setTextSize(2);
    tft->print("24 Hour: ");

    value_drawer->drawPercentageChange(4, current_change, 2, 2);
  }
}

// Draws a passed bitmap on the screen at the given position with the given
// colour.
void COIN::drawBitmap(int16_t x, int16_t y,
                      const uint8_t* bitmap, int16_t w, int16_t h,
                      uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if (byte & 0x80)
        tft->drawPixel(x + i, y + j, color);
    }
  }
}

// Draws a passed bitmap on the screen at double the scale, at the given position with the given colour.
void COIN::drawBitmapDouble(int16_t x, int16_t y,
                            const uint8_t* bitmap, int16_t w, int16_t h,
                            uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if (byte & 0x80) {
        // Draw a 2x2 block of pixels instead of a single pixel
        tft->drawPixel(x + 2 * i, y + 2 * j, color);
        tft->drawPixel(x + 2 * i + 1, y + 2 * j, color);
        tft->drawPixel(x + 2 * i, y + 2 * j + 1, color);
        tft->drawPixel(x + 2 * i + 1, y + 2 * j + 1, color);
      }
    }
  }
}
