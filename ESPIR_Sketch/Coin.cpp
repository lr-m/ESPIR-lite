#include "Coin.h"
#include <cstring>

// Constructor for Coin with bitmap
COIN::COIN(char* code, char* id, const unsigned char* bm, uint16_t circle_col,
           uint16_t bm_col, uint16_t portfolio_col, double coin_amount,
           Value_Drawer* drawer, Adafruit_ST7735* display)
  : bitmap(bm), circle_colour(circle_col), bm_colour(bm_col), portfolio_colour(portfolio_col), amount(coin_amount), value_drawer(drawer), display(display) {

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
           double coin_amount, Value_Drawer* drawer, Adafruit_ST7735* display)
  : portfolio_colour(portfolio_col), amount(coin_amount), value_drawer(drawer), display(display) {

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
    candles = (Candle_Chart*)malloc(sizeof(Candle_Chart));
    *(candles) = Candle_Chart(display, 26, display->height() / 2,
                              display->height() - 6, 0);
  }
}

// Display properties of coin on display
void COIN::draw(int currency, bool bitmap_enabled) {
  display->setTextColor(WHITE);
  display->fillRect(0, 0, display->width(), display->height(), BLACK);

  if (bitmap_enabled) {
    display->fillCircle(12, 12, 44, circle_colour);
    drawBitmap(4, 4, bitmap, 40, 40, bm_colour);
  }

  drawName(bitmap_enabled);

  if (bitmap_enabled) {
    display->setCursor(BM_PRICE_START_X, PRICE_START_Y);
  } else {
    display->setCursor(NO_BM_PRICE_START_X, PRICE_START_Y);
  }

  value_drawer->drawPrice(7, current_price, 7, 2, currency);

  drawPercentageChange(bitmap_enabled);
  candles->display();
}

// Draws the name of the coin on the screen.
void COIN::drawName(bool bitmap_enabled) {
  display->setTextSize(2);

  int len = 0;
  for (len = 0; len < 8; len++) {
    if (coin_code[len] == 0)
      break;
  }

  if (bitmap_enabled) {
    display->setCursor(
      BM_PRICE_START_X + (((display->width() - BM_PRICE_START_X) / 8) * (8 - len)) / 2,
      6);
  } else {
    display->setCursor(
      NO_BM_PRICE_START_X + (((display->width() - 2 * NO_BM_PRICE_START_X) / 8) * (8 - len)) / 2,
      6);
  }

  display->print(coin_code);
}

// Draws the percentage change on the screen.
void COIN::drawPercentageChange(bool bitmap_enabled) {
  if (bitmap_enabled) {
    display->setCursor(BM_CHANGE_START_X, CHANGE_START_Y);
  } else {
    display->setCursor(NO_BM_CHANGE_START_X, CHANGE_START_Y);
  }

  display->setTextSize(1);
  display->print("24 Hour: ");

  value_drawer->drawPercentageChange(4, current_change, 2, 1);
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
        display->drawPixel(x + i, y + j, color);
    }
  }
}