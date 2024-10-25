/*
  Portfolio_Editor_Page.cpp - Portfolio editor interface for ST7735
  Copyright (c) 2021 Luke Mills.  All right reserved.
*/

#include "Portfolio_Editor_Page.h"

Portfolio_Editor_Page::Portfolio_Editor_Page(const char *label, COIN **coins, int coin_name_count)
  : coins(coins), coin_name_count(coin_name_count) {
  setLabel(label);
  selector = new Price_Selector();
}

void Portfolio_Editor_Page::pageMoveUp() {
  if (!editing_value) {
    displayOnIndex(selectedIndex, false);
    if (selectedIndex < row_count) {
      while (selectedIndex + row_count < coin_name_count) {
        selectedIndex += row_count;
      }
    } else {
      selectedIndex -= row_count;
    }
    displayOnIndex(selectedIndex, true);
  }
}

void Portfolio_Editor_Page::pageMoveDown() {
  if (!editing_value) {
    displayOnIndex(selectedIndex, false);
    selectedIndex += row_count;
    if (selectedIndex >= coin_name_count) {
      selectedIndex = selectedIndex % row_count;
    }
    displayOnIndex(selectedIndex, true);
  }
}

void Portfolio_Editor_Page::pageMoveLeft() {
  if (!editing_value) {
    displayOnIndex(selectedIndex, false);
    if (selectedIndex == 0) {
      selectedIndex = coin_name_count - 1;
    } else {
      selectedIndex--;
    }
    displayOnIndex(selectedIndex, true);
  }
}

void Portfolio_Editor_Page::pageMoveRight() {
  if (!editing_value) {
    displayOnIndex(selectedIndex, false);
    selectedIndex++;
    if (selectedIndex >= coin_name_count) {
      selectedIndex = 0;
    }
    displayOnIndex(selectedIndex, true);
  }
}


void Portfolio_Editor_Page::pagePress() {
  if (!editing_value) {
    editing_value = true;
    selector->setOldValue(coins[selectedIndex]->amount);
    selector->display(getScreen());
  } else {
    if (!selector->isDecimal()) {
      selector->addDecimalPoint(getScreen());
    } else {
      selector->clear(getScreen());
      coins[selectedIndex]->setAmount(selector->getValue());
      selector->reset();
      editing_value = false;
    }
  }
}

void Portfolio_Editor_Page::pageBack() {
  if (editing_value) {
    selector->clear(getScreen());
    selector->reset();
    editing_value = false;
  } else {
    setEntered(false);
  }
}

bool Portfolio_Editor_Page::pageSerialize(uint8_t *buffer, int *index, uint16_t length_limit) {
  for (int i = 0; i < COIN_COUNT; i++){
    std::memcpy(buffer + *index, &coins[i]->amount, 8);
    *index+=8;
  }
  return true;
}

bool Portfolio_Editor_Page::pageDeserialize(uint8_t *buffer, int *index, uint16_t length_limit) {
  for (int i = 0; i < COIN_COUNT; i++){
    std::memcpy(&coins[i]->amount, buffer + *index, 8);
    *index+=8;
  }
  return true;
}

void Portfolio_Editor_Page::displayOnIndex(int index, bool selected) {
  int option_sep, option_height, background_height, background_x_padding, label_height, button_rounding = 0;

  if (getScreen()->width() == 128){
    option_sep = OPTION_SEP_128;
    option_height = OPTION_HEIGHT_128;
    background_height = BACKGROUND_HEIGHT_128;
    background_x_padding = BACKGROUND_X_PADDING_128;
    label_height = LABEL_HEIGHT_128;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 160){
    option_sep = OPTION_SEP_160;
    option_height = OPTION_HEIGHT_160;
    background_height = BACKGROUND_HEIGHT_160;
    background_x_padding = BACKGROUND_X_PADDING_160;
    label_height = LABEL_HEIGHT_160;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 240){
    option_sep = OPTION_SEP_320;
    option_height = OPTION_HEIGHT_320;
    background_height = BACKGROUND_HEIGHT_320;
    background_x_padding = BACKGROUND_X_PADDING_320;
    label_height = LABEL_HEIGHT_320;
    getScreen()->setTextSize(2);
  } else if (getScreen()->width() == 320){
    option_sep = OPTION_SEP_320;
    option_height = OPTION_HEIGHT_320;
    background_height = BACKGROUND_HEIGHT_320;
    background_x_padding = BACKGROUND_X_PADDING_320;
    label_height = LABEL_HEIGHT_320;
    getScreen()->setTextSize(2);
  }

  uint16_t x_co = index % row_count;
  uint16_t y_co = index / row_count;
  uint16_t current_x;
  if (x_co == 0) {
    current_x = padding;
  } else {
    current_x = padding + (x_co * (max_width + option_sep));
  }

  uint16_t current_y = label_height + option_height * y_co;

  int16_t x, y;
  uint16_t width, height;

  getScreen()->getTextBounds(coins[index]->coin_code, 0, 0, &x, &y, &width, &height);
  getScreen()->setTextColor(WHITE);

  if (selected) {
    getScreen()->fillRect(current_x - background_x_padding, current_y - (background_height - height) / 2, max_width + 2 * background_x_padding, background_height, DARK_GREY);
  } else {
    getScreen()->fillRect(current_x - background_x_padding, current_y - (background_height - height) / 2, max_width + 2 * background_x_padding, background_height, BLACK);
  }
  getScreen()->drawRect(current_x - background_x_padding - 2, current_y - (background_height - height) / 2, 2, background_height, coins[index]->portfolio_colour);

  getScreen()->setCursor(current_x + (max_width - width) / 2, current_y);
  getScreen()->print(coins[index]->coin_code);
}

void Portfolio_Editor_Page::pageDisplay() {
  int option_sep, option_height, background_height, background_x_padding, label_height, button_rounding = 0;

  if (getScreen()->width() == 128){
    option_sep = OPTION_SEP_128;
    option_height = OPTION_HEIGHT_128;
    background_height = BACKGROUND_HEIGHT_128;
    background_x_padding = BACKGROUND_X_PADDING_128;
    label_height = LABEL_HEIGHT_128;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 160){
    option_sep = OPTION_SEP_160;
    option_height = OPTION_HEIGHT_160;
    background_height = BACKGROUND_HEIGHT_160;
    background_x_padding = BACKGROUND_X_PADDING_160;
    label_height = LABEL_HEIGHT_160;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 240){
    option_sep = OPTION_SEP_320;
    option_height = OPTION_HEIGHT_320;
    background_height = BACKGROUND_HEIGHT_320;
    background_x_padding = BACKGROUND_X_PADDING_320;
    label_height = LABEL_HEIGHT_320;
    getScreen()->setTextSize(2);
  } else if (getScreen()->width() == 320){
    option_sep = OPTION_SEP_320;
    option_height = OPTION_HEIGHT_320;
    background_height = BACKGROUND_HEIGHT_320;
    background_x_padding = BACKGROUND_X_PADDING_320;
    label_height = LABEL_HEIGHT_320;
    getScreen()->setTextSize(2);
  }

  // clear the screen
  getScreen()->fillScreen(BLACK);
  getScreen()->setTextColor(WHITE);

  row_count = 0;
  max_width = 0;
  padding = 0;

  // get the max text width of one of the coins (all boxes have same width)
  for (int i = 0; i < coin_name_count; ++i) {
    int16_t x, y;
    uint16_t width, height;

    getScreen()->getTextBounds(coins[i]->coin_code, 0, 0, &x, &y, &width, &height);
    if (width > max_width) {
      max_width = width;
    }
  }

  // Work out padding to center options
  uint16_t current = 0;
  while (current + max_width < getScreen()->width()) {
    current += max_width + option_sep;
    row_count++;
  }
  uint16_t row_width = (row_count * max_width) + ((row_count - 1) * option_sep);
  padding = (getScreen()->width() - row_width) / 2;

  // Display each option label horizontally with a red box
  uint16_t current_x = padding;
  uint16_t current_y = label_height;
  for (int i = 0; i < coin_name_count; ++i) {
    int16_t x, y;
    uint16_t width, height;

    getScreen()->getTextBounds(coins[i]->coin_code, 0, 0, &x, &y, &width, &height);
    if (current_x + max_width > getScreen()->width()) {
      current_y += option_height;
      current_x = padding;
    }

    getScreen()->drawRect(current_x - background_x_padding - 2, current_y - (background_height - height) / 2, 2, background_height, coins[i]->portfolio_colour);
    getScreen()->setCursor(current_x + (max_width - width) / 2, current_y);
    getScreen()->print(coins[i]->coin_code);

    current_x += max_width + option_sep;
  }

  displayOnIndex(selectedIndex, true);
}

void Portfolio_Editor_Page::pageIntInput(uint32_t input) {
  if (editing_value) {
    if (!selector->isDecimal()) {
      if (selector->getInputIndex() < 10) {
        selector->takeIntInput(input);
      }
    } else {
      if ((selector->getInputIndex() < 13) && ((selector->getInputIndex() - selector->getDecimalPointIndex()) < 7)) {
        selector->takeIntInput(input);
      }
    }

    selector->refresh(getScreen());
  }
}



// Constructor for Amount Selector
Price_Selector::Price_Selector() {}

void Price_Selector::takeIntInput(uint32_t value) {
  if (value == 0) {
    new_double[input_index] = '0';
    input_index++;
  } else if (value == 1) {
    new_double[input_index] = '1';
    input_index++;
  } else if (value == 2) {
    new_double[input_index] = '2';
    input_index++;
  } else if (value == 3) {
    new_double[input_index] = '3';
    input_index++;
  } else if (value == 4) {
    new_double[input_index] = '4';
    input_index++;
  } else if (value == 5) {
    new_double[input_index] = '5';
    input_index++;
  } else if (value == 6) {
    new_double[input_index] = '6';
    input_index++;
  } else if (value == 7) {
    new_double[input_index] = '7';
    input_index++;
  } else if (value == 8) {
    new_double[input_index] = '8';
    input_index++;
  } else if (value == 9) {
    new_double[input_index] = '9';
    input_index++;
  }
}

// Sets the value to the passed value
void Price_Selector::setOldValue(double val) {
  old_value = val;
}

// Returns the current value
double Price_Selector::getValue() {
  return strtod(new_double, NULL);
}

// Clears the area occupied by the selector
void Price_Selector::clear(Adafruit_GFX *display) {
  if (display->width() == 128){
    display->fillRect(0, display->height() - 40, display->width(), 40, BLACK);
  } else if (display->width() == 160) {
    if (display->height() == 80){
      display->fillRect(0, display->height() - 32, display->width(), 32, BLACK);
    } else {
      display->fillRect(0, display->height() - 40, display->width(), 40, BLACK);
    }
  } else if (display->width() == 240) {
    display->fillRect(0, display->height() - 60, display->width(), 60, BLACK);
  } else if (display->width() == 320) {
    display->fillRect(0, display->height() - 60, display->width(), 60, BLACK);
  }
}

void Price_Selector::reset() {
  for (int i = 0; i < 13; i++) {
    new_double[i] = '0';
  }
  new_double[13] = 0;

  decimal_component_active = 0;
  input_index = 0;
  decimal_point_index = 0;
}

// Redraws the value selected by the user
void Price_Selector::display(Adafruit_GFX *display) {
  if (display->width() == 128){
    display->setTextSize(1);
    display->setCursor(2, display->height() - 40);
  } else if (display->width() == 160) {
    display->setTextSize(1);
    if (display->height() == 80){
      display->setCursor(2, display->height() - 30);
    } else {
      display->setCursor(2, display->height() - 40);
    }
  } else if (display->width() == 240) {
    display->setTextSize(2);
    display->setCursor(2, display->height() - 60);
  } else if (display->width() == 320) {
    display->setTextSize(2);
    display->setCursor(2, display->height() - 60);
  }

  // Draw old value
  display->setTextColor(GRAY);
  display->print("Old: ");
  display->print(old_value, 6);

  if (display->width() == 128){
    display->setTextSize(2);
    display->setCursor(2, display->height() - 20);
  } else if (display->width() == 160) {
    display->setTextSize(2);
    if (display->height() == 80){
      display->setCursor(2, display->height() - 17);
    } else {
      display->setCursor(2, display->height() - 20);
    }
  } else if (display->width() == 240) {
    display->setTextSize(4);
    display->setCursor(2, display->height() - 35);
  } else if (display->width() == 320) {
    display->setTextSize(4);
    display->setCursor(2, display->height() - 35);
  }

  // Indicate entry
  display->setTextColor(WHITE);
  if (getInputIndex() < 13) {
    display->print('_');
  }
}

// refreshes the value being drawn
void Price_Selector::refresh(Adafruit_GFX *display) {
  // Clear old value
  if (display->width() == 128){
    display->fillRect(0, display->height() - 24, display->width(), 24, BLACK);
  } else if (display->width() == 160) {
    if (display->height() == 80){
      display->fillRect(0, display->height() - 18, display->width(), 18, BLACK);
    } else {
      display->fillRect(0, display->height() - 28, display->width(), 28, BLACK);
    }
  } else if (display->width() == 240) {
    display->fillRect(0, display->height() - 35, display->width(), 35, BLACK);
  } else if (display->width() == 320) {
    display->fillRect(0, display->height() - 35, display->width(), 35, BLACK);
  }

  // Draw new value
  display->setTextColor(WHITE);

  if (display->width() == 128){
    display->setTextSize(2);
    display->setCursor(2, display->height() - 20);
  } else if (display->width() == 160) {
    display->setTextSize(2);
    display->setCursor(2, display->height() - 20);
  } else if (display->width() == 240) {
    display->setTextSize(4);
    display->setCursor(2, display->height() - 35);
  } else if (display->width() == 320) {
    display->setTextSize(4);
    display->setCursor(2, display->height() - 35);
  }

  for (int i = 0; i < input_index; i++)
    display->print(new_double[i]);

  if (input_index < 13 && (!decimal_component_active) || (decimal_component_active && input_index - decimal_point_index < 7))
    display->print('_');
}

void Price_Selector::addDecimalPoint(Adafruit_GFX *display) {
  // Put a 0 before the decimal point if no inputs
  if (input_index == 0) {
    new_double[0] = '0';
    input_index++;
  }

  decimal_component_active = 1;
  decimal_point_index = input_index;
  new_double[input_index] = '.';
  input_index++;

  refresh(display);
}
