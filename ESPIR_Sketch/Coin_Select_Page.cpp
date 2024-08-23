#include "Coin_Select_Page.h"

Coin_Select_Page::Coin_Select_Page(const char *label, COIN **coins, bool *selected_array, int coin_name_count)
  : coins(coins), coin_name_count(coin_name_count), is_selected(selected_array) {
  setLabel(label);
}

void Coin_Select_Page::pageMoveUp() {
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

void Coin_Select_Page::pageMoveDown() {
  displayOnIndex(selectedIndex, false);
  selectedIndex += row_count;
  if (selectedIndex >= coin_name_count) {
    selectedIndex = selectedIndex % row_count;
  }
  displayOnIndex(selectedIndex, true);
}

void Coin_Select_Page::pageMoveLeft() {
  displayOnIndex(selectedIndex, false);
  if (selectedIndex == 0) {
    selectedIndex = coin_name_count - 1;
  } else {
    selectedIndex--;
  }
  displayOnIndex(selectedIndex, true);
}

void Coin_Select_Page::pageMoveRight() {
  displayOnIndex(selectedIndex, false);
  selectedIndex++;
  if (selectedIndex >= coin_name_count) {
    selectedIndex = 0;
  }
  displayOnIndex(selectedIndex, true);
}


void Coin_Select_Page::pagePress() {
  if (is_selected[selectedIndex] && selected_count > 1) {
    is_selected[selectedIndex] = false;
    selected_count--;
  } else if (!is_selected[selectedIndex]) {
    is_selected[selectedIndex] = true;
    selected_count++;
  }
  displayOnIndex(selectedIndex, true);
}

void Coin_Select_Page::pageBack() {
  setEntered(false);
}

bool Coin_Select_Page::pageSerialize(uint8_t *buffer, int *index, uint16_t length_limit) {
  uint8_t value = 0;
  for (int i = 0; i < COIN_COUNT; i++) {
    // increment value if current coin selected
    if (is_selected[i]) {
      value += 1;
    }

    // increment byte value if end of byte reached
    if ((i > 0) && ((i + 1) % 8 == 0)) {
      buffer[*index] = value;
      value = 0;
      (*index)++;
    }

    // pad the last bits in incomplete byte
    if (i == COIN_COUNT - 1) {
      for (int i = 0; i < 8 - (COIN_COUNT % 8); i++) {
        value = value << 1;
      }
      buffer[*index] = value;
      (*index)++;
      return true;
    }

    // shift bit up
    value = value << 1;
  }
  return true;
}

bool Coin_Select_Page::pageDeserialize(uint8_t *buffer, int *index, uint16_t length_limit) {
  selected_count = 0;
  for (int i = 0; i < COIN_COUNT; i++) {
    is_selected[i] = (buffer[*index] >> (7 - (i % 8))) & 0x01;

    if (is_selected[i]) {
      selected_count++;
    }

    if (i == COIN_COUNT-1){
      (*index)++;
      return true;
    }

    if ((i > 0) && ((i+1) % 8 == 0)) {
      (*index)++;
    }
  }
  *index++;
  return true;
}

void Coin_Select_Page::displayOnIndex(int index, bool selected) {
  int option_sep, option_height, background_height, background_x_padding, label_height, button_rounding = 0;

  if (getScreen()->width() == 128){
    option_sep = OPTION_SEP_128;
    option_height = OPTION_HEIGHT_128;
    background_height = BACKGROUND_HEIGHT_128;
    background_x_padding = BACKGROUND_X_PADDING_128;
    label_height = LABEL_HEIGHT_128;
    button_rounding = BUTTON_ROUNDING_128;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 160){
    option_sep = OPTION_SEP_160;
    option_height = OPTION_HEIGHT_160;
    background_height = BACKGROUND_HEIGHT_160;
    background_x_padding = BACKGROUND_X_PADDING_160;
    label_height = LABEL_HEIGHT_160;
    button_rounding = BUTTON_ROUNDING_160;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 320){
    option_sep = OPTION_SEP_320;
    option_height = OPTION_HEIGHT_320;
    background_height = BACKGROUND_HEIGHT_320;
    background_x_padding = BACKGROUND_X_PADDING_320;
    label_height = LABEL_HEIGHT_320;
    button_rounding = BUTTON_ROUNDING_320;
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

  if (selected) {
    getScreen()->setTextColor(BLACK);
  } else {
    getScreen()->setTextColor(WHITE);
  }

  if (is_selected[index]) {
    getScreen()->fillRoundRect(current_x - background_x_padding, current_y - (background_height - height) / 2, max_width + 2 * background_x_padding, background_height, button_rounding, GREEN);
  } else {
    getScreen()->fillRoundRect(current_x - background_x_padding, current_y - (background_height - height) / 2, max_width + 2 * background_x_padding, background_height, button_rounding, RED);
  }

  getScreen()->setCursor(current_x + (max_width - width) / 2, current_y);
  getScreen()->print(coins[index]->coin_code);
}

void Coin_Select_Page::pageDisplay() {
  int option_sep, option_height, background_height, background_x_padding, label_height, button_rounding = 0;

  if (getScreen()->width() == 128){
    option_sep = OPTION_SEP_128;
    option_height = OPTION_HEIGHT_128;
    background_height = BACKGROUND_HEIGHT_128;
    background_x_padding = BACKGROUND_X_PADDING_128;
    label_height = LABEL_HEIGHT_128;
    button_rounding = BUTTON_ROUNDING_128;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 160){
    option_sep = OPTION_SEP_160;
    option_height = OPTION_HEIGHT_160;
    background_height = BACKGROUND_HEIGHT_160;
    background_x_padding = BACKGROUND_X_PADDING_160;
    label_height = LABEL_HEIGHT_160;
    button_rounding = BUTTON_ROUNDING_160;
    getScreen()->setTextSize(1);
  } else if (getScreen()->width() == 320){
    option_sep = OPTION_SEP_320;
    option_height = OPTION_HEIGHT_320;
    background_height = BACKGROUND_HEIGHT_320;
    background_x_padding = BACKGROUND_X_PADDING_320;
    label_height = LABEL_HEIGHT_320;
    button_rounding = BUTTON_ROUNDING_320;
    getScreen()->setTextSize(2);
  }

  // clear the screen
  getScreen()->fillScreen(BLACK);

  // draw the title
  getScreen()->setCursor(5, 5);
  getScreen()->setTextColor(WHITE);
  getScreen()->print("Select coins:");

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

    if (is_selected[i]) {
      getScreen()->fillRoundRect(current_x - background_x_padding, current_y - (background_height - height) / 2, max_width + 2 * background_x_padding, background_height, button_rounding, GREEN);
    } else {
      getScreen()->fillRoundRect(current_x - background_x_padding, current_y - (background_height - height) / 2, max_width + 2 * background_x_padding, background_height, button_rounding, RED);
    }

    getScreen()->setCursor(current_x + (max_width - width) / 2, current_y);
    getScreen()->print(coins[i]->coin_code);

    current_x += max_width + option_sep;
  }

  displayOnIndex(selectedIndex, true);
}

void Coin_Select_Page::pageIntInput(uint32_t input) {
  if (input * row_count < coin_name_count) {
    displayOnIndex(selectedIndex, false);

    selectedIndex = (input * row_count) + (selectedIndex % row_count);
    displayOnIndex(selectedIndex, true);
  }
}

void Coin_Select_Page::clearCoins(){
  // might have to also clear the candle charts for each coin that is deselected
  for (int i = 0; i < COIN_COUNT; i++){
    is_selected[i] = false;
  }
  is_selected[0] = true;
}