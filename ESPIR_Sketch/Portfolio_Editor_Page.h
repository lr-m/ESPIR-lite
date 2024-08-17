/*
  Portfolio_Editor.h - Portfolio editor interface for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <ST7735_Menu.h>
#include <cstring>

#include "Coin.h"
#include "Colours.h"
#include "IR_Codes.h"

#ifndef Portfolio_Editor_h
#define Portfolio_Editor_h

#define OPTION_SEP 8
#define OPTION_HEIGHT 12
#define BACKGROUND_HEIGHT 10
#define BACKGROUND_X_PADDING 2
#define LABEL_HEIGHT 2

#define SELECTED_LIMIT 10

class Price_Selector {
public:
  Price_Selector();
  void takeIntInput(uint32_t value);
  void setOldValue(double);
  double getValue();
  void clear(Adafruit_ST7735* display);
  void reset();
  void display(Adafruit_ST7735* display);
  void refresh(Adafruit_ST7735* display);
  void addDecimalPoint(Adafruit_ST7735* display);
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
