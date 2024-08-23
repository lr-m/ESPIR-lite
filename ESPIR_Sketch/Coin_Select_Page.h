#include <ST7735_Menu.h>
#include "Coin.h"

#ifndef Coin_Select_Page_h
#define Coin_Select_Page_h

// for 128 width
#define OPTION_SEP_128 8
#define OPTION_HEIGHT_128 16
#define BACKGROUND_HEIGHT_128 12
#define BACKGROUND_X_PADDING_128 2
#define LABEL_HEIGHT_128 18
#define BUTTON_ROUNDING_128 1

// for 160 width
#define OPTION_SEP_160 8
#define OPTION_HEIGHT_160 16
#define BACKGROUND_HEIGHT_160 12
#define BACKGROUND_X_PADDING_160 2
#define LABEL_HEIGHT_160 18
#define BUTTON_ROUNDING_160 1

// for 320 width
#define OPTION_SEP_320 16
#define OPTION_HEIGHT_320 28
#define BACKGROUND_HEIGHT_320 20
#define BACKGROUND_X_PADDING_320 4
#define LABEL_HEIGHT_320 32
#define BUTTON_ROUNDING_320 4

#define SELECTED_LIMIT 20

class Coin_Select_Page : public Page {
public:
  Coin_Select_Page(const char* label, COIN** coins, bool* selected_array, int coin_name_count);
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

  void clearCoins();
  void displayOnIndex(int, bool);
  bool* getSelected() {
    return is_selected;
  }
private:
  COIN** coins;
  char* coin_names[COIN_COUNT];
  bool* is_selected;
  uint16_t coin_name_count;
  uint16_t max_width = 0;
  uint16_t padding = 0;
  uint16_t selectedIndex = 0;
  uint16_t row_count = 0;
  uint16_t selected_count = 1; // first coin is selected by default
};

#endif