/*
  Portfolio_Editor.h - Portfolio editor interface for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>
#include <Adafruit_GFX.h>
#include <ST7735_Menu.h>
#include <cstring>

#include "Coin.h"
#include "Colours.h"
#include "IR_Codes.h"

#ifndef Portfolio_Editor_h
#define Portfolio_Editor_h

// #define OPTION_SEP 8
// #define OPTION_HEIGHT 12
// #define BACKGROUND_HEIGHT 10
// #define BACKGROUND_X_PADDING 2
// #define LABEL_HEIGHT 2

// for 128 width
#define OPTION_SEP_128 8
#define OPTION_HEIGHT_128 12
#define BACKGROUND_HEIGHT_128 10
#define BACKGROUND_X_PADDING_128 2
#define LABEL_HEIGHT_128 2

// for 160 width
#define OPTION_SEP_160 8
#define OPTION_HEIGHT_160 12
#define BACKGROUND_HEIGHT_160 10
#define BACKGROUND_X_PADDING_160 2
#define LABEL_HEIGHT_160 2

// for 320 width
#define OPTION_SEP_320 16
#define OPTION_HEIGHT_320 24
#define BACKGROUND_HEIGHT_320 20
#define BACKGROUND_X_PADDING_320 4
#define LABEL_HEIGHT_320 4

class Price_Selector {
public:
  Price_Selector();
  void takeIntInput(uint32_t value);
  void setOldValue(double);
  double getValue();
  void clear(Adafruit_GFX* display);
  void reset();
  void display(Adafruit_GFX* display);
  void refresh(Adafruit_GFX* display);
  void addDecimalPoint(Adafruit_GFX* display);
  bool isDecimal() {
    return decimal_component_active;
  }
  char getInputIndex() {
    return input_index;
  }
  char getDecimalPointIndex() {
    return decimal_point_index;
  }

private:
  double old_value = 0;
  char new_double[14] = { 0 };
  char decimal_component_active = 0;
  char input_index = 0;
  char decimal_point_index = 0;
};

// library interface description
class Portfolio_Editor_Page : public Page {
public:
  Portfolio_Editor_Page(const char* label, COIN** coins, int coin_name_count);
  void pageMoveUp() override;
  void pageMoveDown() override;
  void pageMoveLeft() override;
  void pageMoveRight() override;
  void pagePress() override;
  void pageBack() override;
  void pageDisplay() override;
  bool pageSerialize(uint8_t* buffer, int* index, uint16_t length_limit) override;
  bool pageDeserialize(uint8_t* buffer, int* index, uint16_t length_limit) override;
  void pageDefault() override{};
  void pageIntInput(uint32_t input) override;

  void displayOnIndex(int, bool);
private:
  COIN** coins;
  char* coin_names[COIN_COUNT];
  uint16_t coin_name_count;
  uint16_t max_width = 0;
  uint16_t padding = 0;
  uint16_t selectedIndex = 0;
  uint16_t row_count = 0;
  bool editing_value = false;
  Price_Selector* selector;
};

#endif
