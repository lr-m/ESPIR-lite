#include "Crypto_Ticker_Menu.h"

void placeholder() {
  // Serial.println("test");
}

Crypto_Ticker_Menu::Crypto_Ticker_Menu(Adafruit_GFX* tft, COIN** coins, Portfolio* portfolio, Network_Manager_ESP8266* network_manager)
  : coins(coins), portfolio(portfolio), network_manager(network_manager), tft(tft) {
  selected_coins[0] = true;

  // initialise the menu
  menu = new Menu(tft, STORAGE_SIZE);

  // initialise the customize submenu and add elements
  customize_submenu = new Submenu("Customize", tft);
  coin_select_page = new Coin_Select_Page("Select Coins", coins, selected_coins, 20);
  portfolio_editor_page = new Portfolio_Editor_Page("Edit portfolio", coins, 20);
  currency_selector = new Selector("Currency:", currency_options, currency_options_index, 3, "customize:currency", 0);
  
  customize_submenu->addElement(coin_select_page);
  customize_submenu->addElement(portfolio_editor_page);
  customize_submenu->addElement(currency_selector);

  // initialise the crypto settings submenu and add elements
  crypto_settings_submenu = new Submenu("Crypto Settings", tft);

  // init elements
  crypto_settings_reset_coins_button = new Button("Clear Coins", [=] {
    coin_select_page->clearCoins();
  });
  crypto_settings_cycle_duration_slider = new Slider("Cycle Duration", 0, 60, 10, 10, "crypto_settings:cycle_duration");
  crypto_settings_candle_duration_slider = new Slider("Candle Duration", 5, 60, 5, 15, "crypto_settings:candle_duration");
  crypto_settings_toggle_bitmaps_checkbox = new Checkbox("Enable bitmaps:", "crypto_settings:enable_bitmaps", true);

  // add elements
  crypto_settings_submenu->addElement(crypto_settings_reset_coins_button);
  crypto_settings_submenu->addElement(crypto_settings_cycle_duration_slider);
  crypto_settings_submenu->addElement(crypto_settings_candle_duration_slider);
  crypto_settings_submenu->addElement(crypto_settings_toggle_bitmaps_checkbox);

  // initialise the portfolio settings submenu
  portfolio_settings_clear_portfolio_button = new Button("Clear Portfolio", [=] {
    portfolio->clear();
  });
  portfolio_settings_submenu = new Submenu("Portfolio Settings", tft);
  portfolio_settings_cycle_duration_slider = new Slider("Cycle Duration", 0, 60, 10, 10, "portfolio_settings:cycle_duration");
  portfolio_settings_candle_duration_slider = new Slider("Candle Duration", 5, 60, 5, 15, "portfolio_settings:candle_duration");
  
  // add elements
  portfolio_settings_submenu->addElement(portfolio_settings_clear_portfolio_button);
  portfolio_settings_submenu->addElement(portfolio_settings_cycle_duration_slider);
  portfolio_settings_submenu->addElement(portfolio_settings_candle_duration_slider);
  
  // initialise the storage settings submenu
  storage_submenu = new Submenu("Storage Settings", tft);
  clear_settings = new Button("Reset Settings", [=] {
    menu->reset();
  });
  clear_wifi_credentials = new Button("Clear WiFi Info", [=] {
    network_manager->clearCredentials();
  });
  memory_info_button = new Button("Memory Info", [=] {
    displayMemoryInfo();
  });

  // add elements
  storage_submenu->addElement(clear_settings);
  storage_submenu->addElement(clear_wifi_credentials);
  storage_submenu->addElement(memory_info_button);

  // StatusPage status_page;

  // add submenus to menu
  menu->addElement(customize_submenu);
  menu->addElement(crypto_settings_submenu);
  menu->addElement(portfolio_settings_submenu);
  menu->addElement(storage_submenu);

  // load menu from storage if present
  menu->load();
}

bool Crypto_Ticker_Menu::back() {
  if (!menu->isEntered()) {
    return true;
  } else {
    menu->back();
    return false;
  }
}

void Crypto_Ticker_Menu::getNextCoinIndex(uint16_t* current_index) {
  bool* selected_indexes = coin_select_page->getSelected();
  uint16_t start_index = (*current_index + 1) % COIN_COUNT;  // Start searching from the next index
  for (uint16_t i = start_index; i != *current_index; i = (i + 1) % COIN_COUNT) {
    if (selected_indexes[i]) {
      *current_index = i;
      return;
    }
  }
}

void Crypto_Ticker_Menu::getPreviousCoinIndex(uint16_t* current_index) {
  bool* selected_indexes = coin_select_page->getSelected();
  uint16_t start_index = (*current_index + COIN_COUNT - 1) % COIN_COUNT;  // Start searching from the previous index
  for (uint16_t i = start_index; i != *current_index; i = (i + COIN_COUNT - 1) % COIN_COUNT) {
    if (selected_indexes[i]) {
      *current_index = i;
      return;
    }
  }
}

bool Crypto_Ticker_Menu::isCoinSelected(int index){
  return coin_select_page->getSelected()[index];
}

void Crypto_Ticker_Menu::displayMemoryInfo(){
  uint8_t start = tft->height() - 20;
  tft->fillRect(0, start, tft->width(), tft->height() - start, BLACK);

  // draw free heap data
  tft->setCursor(2, start);
  tft->setTextColor(DARK_GREY);
  tft->print("Free heap:      ");
  tft->setTextColor(WHITE);
  tft->print(ESP.getFreeHeap());

  // draw max free block size
  tft->setCursor(2, start + 10);
  tft->setTextColor(DARK_GREY);
  tft->print("Max free block: ");
  tft->setTextColor(WHITE);
  tft->print(ESP.getMaxFreeBlockSize());

  // draw fragmentation stats
  tft->setCursor(2, start + 20);
  tft->setTextColor(DARK_GREY);
  tft->print("Fragmentation:  ");
  if (ESP.getHeapFragmentation() < 33)
    tft->setTextColor(LIGHT_GREEN);
  else if (ESP.getHeapFragmentation() < 66)
    tft->setTextColor(ORANGE);
  else
    tft->setTextColor(LIGHT_RED);

  tft->print(ESP.getHeapFragmentation());
  tft->print("%");
}