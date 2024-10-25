#include <ST7735_Menu.h>
#include <Adafruit_GFX.h>

#include "TFT_abstraction_layer.h"
#include "Coin_Select_Page.h"
#include "Portfolio_Editor_Page.h"
#include "Coin.h"
#include "Portfolio.h"
#include "Network_Manager_ESP32.h"

#define STORAGE_SIZE 440

#ifndef Crypto_Ticker_Menu_h
#define Crypto_Ticker_Menu_h

class Crypto_Ticker_Menu {
public:
  Crypto_Ticker_Menu(Adafruit_GFX *tft, COIN **coins, Portfolio *portfolio, Network_Manager_ESP32 *network_manager);
  void display() {
    menu->display();
  }
  void moveUp() {
    menu->moveUp();
  }
  void moveDown() {
    menu->moveDown();
  }
  void moveLeft() {
    menu->moveLeft();
  }
  void moveRight() {
    menu->moveRight();
  }
  void press() {
    menu->press();
  }
  bool back();
  void save() {
    menu->save();
  }
  void load() {
    menu->load();
  }
  void reset() {
    menu->reset();
  }
  void intInput(uint32_t input) {
    menu->intInput(input);
  }
  bool getSelectorValue(const char *id, int *dest) {
    return menu->getSelectorValue(id, dest);
  }
  bool getSliderValue(const char *id, int *dest){
    return menu->getSliderValue(id, dest);
  }
  bool getCheckboxValue(const char *id, bool *dest){
    return menu->getCheckboxValue(id, dest);
  }
  void getNextCoinIndex(uint16_t *current_index);
  void getPreviousCoinIndex(uint16_t *current_index);
  bool isCoinSelected(int index);
  bool *getSelectedCoins() {
    return selected_coins;
  }
  const char *getCurrency() {
    return currency_options_lower[currency_selector->getSelectedIndex()];
  }
  int getCoinCandlePeriod() {
    return crypto_settings_candle_duration_slider->getValue();
  }
  int getPortfolioCandlePeriod() {
    return portfolio_settings_candle_duration_slider->getValue();
  }
  int getCoinCycleDuration() {
    return crypto_settings_cycle_duration_slider->getValue();
  }
  int getPortfolioCycleDuration() {
    return portfolio_settings_cycle_duration_slider->getValue();
  }
  void displayMemoryInfo();

private:
  COIN **coins;
  Portfolio *portfolio;
  bool selected_coins[20] = { 0 };
  Menu *menu;
  Network_Manager_ESP32 *network_manager;
  Adafruit_GFX *tft;

  Submenu *customize_submenu;
  Coin_Select_Page *coin_select_page;
  Portfolio_Editor_Page *portfolio_editor_page;
  Selector *currency_selector;
  Button *memory_info_button;

  Submenu *crypto_settings_submenu;
  Button *crypto_settings_reset_coins_button;
  Slider *crypto_settings_cycle_duration_slider;
  Slider *crypto_settings_candle_duration_slider;
  const char *currency_options_lower[3] = { "gbp", "usd", "eur" };
  const char *currency_options[3] = { "GBP", "USD", "EUR" };
  int currency_options_index[3] = { 0, 1, 2 };
  Checkbox *crypto_settings_toggle_bitmaps_checkbox;

  Submenu *portfolio_settings_submenu;
  Button *portfolio_settings_clear_portfolio_button;
  Slider *portfolio_settings_cycle_duration_slider;
  Slider *portfolio_settings_candle_duration_slider;

  Submenu *storage_submenu;
  Button *clear_settings;
  Button *clear_wifi_credentials;

  Button *wifi_settings_change_network;
};

#endif