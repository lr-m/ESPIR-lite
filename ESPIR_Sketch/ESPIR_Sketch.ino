#include <Adafruit_GFX.h>     // Core graphics library
#include <IRremote.h>

#include "Colours.h"
#include "IR_Codes.h"
#include "Crypto_Ticker_Menu.h"
#include "Coin.h"
#include "Portfolio.h"
#include "Network_Manager.h"
#include "TFT_abstraction_layer.h"

// Define PIN config
#define TFT_RES D0
#define TFT_DC D1
#define TFT_SCL D5
#define TFT_SDA D7
#define TFT_CS D2
#define RECV_PIN D4

#define UPDATE_PERIOD 60

extern unsigned char BTC_logo[];
extern unsigned char ETH_logo[];
extern unsigned char USDT_logo[];
extern unsigned char BNB_logo[];
extern unsigned char SOL_logo[];
extern unsigned char USDC_logo[];
extern unsigned char XRP_logo[];
extern unsigned char TON_logo[];
extern unsigned char DOGE_logo[];
extern unsigned char TRX_logo[];
extern unsigned char ADA_logo[];
extern unsigned char AVAX_logo[];
extern unsigned char SHIB_logo[];
extern unsigned char LINK_logo[];
extern unsigned char BCH_logo[];
extern unsigned char DOT_logo[];
extern unsigned char LEO_logo[];
extern unsigned char UNI_logo[];
extern unsigned char NEAR_logo[];
extern unsigned char DAI_logo[];

extern unsigned char epd_bitmap_logo_red[];
extern unsigned char epd_bitmap_logo_green[];

// init display here (modify to match your display)
TFT tft = TFT(ST7735_SPI_160_80, TFT_CS, TFT_DC, TFT_SDA, TFT_SCL, TFT_RES);

// IR remote
decode_results results;

// Instantiate menu
Crypto_Ticker_Menu* menu;
Value_Drawer* value_drawer;
Network_Manager* network_manager;
Keyboard* keyboard;

Portfolio* portfolio;
COIN* coins[20];
uint16_t current_coin = 0;
bool update_flip_flop = false;
bool add_to_candles_flip_flop = false;
bool portfolio_candle_flip_flop = false;
bool coin_candle_flip_flop = false;
bool coin_cycle_flip_flop = false;
bool portfolio_cycle_flip_flop = false;

bool update_group_flip_flop = false;

int old_portfolio_candle_period = 0;
int old_coin_candle_period = 0;
int old_currency = 0;

// for keeping track of state
enum class State {
  NETWORK,
  MENU,
  COIN,
  PORTFOLIO,
  MULTICOIN
};

State state;
State last_state;

void setup(void) {
  state = State::NETWORK;
  last_state = State::COIN;

  IrReceiver.begin(RECV_PIN);

  tft.init();

  // initialise value drawer, this is needed for scientific notation when numbers get big
  value_drawer = new Value_Drawer(tft.getDisplay());

  portfolio = new Portfolio(tft.getDisplay(), coins, value_drawer);

  // Background col, bitmap col, portfolio col
  coins[0] =
    new COIN("BTC", "bitcoin", BTC_logo, BTC_BACKGROUND, BTC_FOREGROUND, BTC_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[1] =
    new COIN("ETH", "ethereum", ETH_logo, ETH_BACKGROUND, ETH_FOREGROUND, ETH_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[2] =
    new COIN("USDT", "tether", USDT_logo, USDT_BACKGROUND, USDT_FOREGROUND, USDT_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[3] =
    new COIN("BNB", "binancecoin", BNB_logo, BNB_BACKGROUND, BNB_FOREGROUND, BNB_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[4] =
    new COIN("SOL", "solana", SOL_logo, SOL_BACKGROUND, SOL_FOREGROUND, SOL_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[5] =
    new COIN("USDC", "usd-coin", USDC_logo, USDC_BACKGROUND, USDC_FOREGROUND, USDC_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[6] =
    new COIN("XRP", "ripple", XRP_logo, XRP_BACKGROUND, XRP_FOREGROUND, XRP_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[7] =
    new COIN("TON", "the-open-network", TON_logo, TON_BACKGROUND, TON_FOREGROUND, TON_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[8] =
    new COIN("DOGE", "dogecoin", DOGE_logo, DOGE_BACKGROUND, DOGE_FOREGROUND, DOGE_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[9] =
    new COIN("TRX", "tron", TRX_logo, TRX_BACKGROUND, TRX_FOREGROUND, TRX_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[10] =
    new COIN("ADA", "cardano", ADA_logo, ADA_BACKGROUND, ADA_FOREGROUND, ADA_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[11] =
    new COIN("AVAX", "avalanche-2", AVAX_logo, AVAX_BACKGROUND, AVAX_FOREGROUND, AVAX_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[12] =
    new COIN("SHIB", "shiba-inu", SHIB_logo, SHIB_BACKGROUND, SHIB_FOREGROUND, SHIB_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[13] =
    new COIN("LINK", "chainlink", LINK_logo, LINK_BACKGROUND, LINK_FOREGROUND, LINK_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[14] =
    new COIN("BCH", "bitcoin-cash", BCH_logo, BCH_BACKGROUND, BCH_FOREGROUND, BCH_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[15] =
    new COIN("DOT", "polkadot", DOT_logo, DOT_BACKGROUND, DOT_FOREGROUND, DOT_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[16] =
    new COIN("LEO", "leo-token", LEO_logo, LEO_BACKGROUND, LEO_FOREGROUND, LEO_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[17] =
    new COIN("UNI", "uniswap", UNI_logo, UNI_BACKGROUND, UNI_FOREGROUND, UNI_FOREGROUND, 0, value_drawer, tft.getDisplay());
  coins[18] =
    new COIN("NEAR", "near", NEAR_logo, NEAR_BACKGROUND, NEAR_FOREGROUND, NEAR_BACKGROUND, 0, value_drawer, tft.getDisplay());
  coins[19] =
    new COIN("DAI", "dai", DAI_logo, DAI_BACKGROUND, DAI_FOREGROUND, DAI_BACKGROUND, 0, value_drawer, tft.getDisplay());

  keyboard = new Keyboard(tft.getDisplay());
  menu = new Crypto_Ticker_Menu(tft.getDisplay(), coins, portfolio, network_manager);
  network_manager = new Network_Manager(tft.getDisplay(), coins, keyboard);

  // load settings from menu into old values
  menu->getSliderValue("crypto_settings:candle_duration", &old_coin_candle_period);
  menu->getSliderValue("portfolio_settings:candle_duration", &old_portfolio_candle_period);
  menu->getSelectorValue("customize:currency", &old_currency);
  
  // if first coin not selected, find the first selected coin
  if (!menu->isCoinSelected(0)){
    menu->getNextCoinIndex(&current_coin);  // get the index of the first selected coin to display on boot
  }

  drawIntroAnimation();

  // load wifi creds and connect to network (or enter setup)
  network_manager->loadAndConnect();

  if ((state == State::NETWORK) && network_manager->isConnected()) {
    tft.getDisplay()->setTextColor(LIGHT_GREEN);
    tft.getDisplay()->println("\n Requesting info...");

    state == State::COIN;
    refreshAllCoins();
    addCoinPricesToCandles();
    display();
  }
}

void drawIntroAnimation() {
  tft.getDisplay()->fillScreen(BLACK);

  if (tft.getDisplay()->width() == 128){
    drawBitmap(0, 20, epd_bitmap_logo_green, 128, 56, LIGHT_GREEN, 1);
    drawBitmap(0, 20, epd_bitmap_logo_red, 128, 56, RED, 1);
  } else if (tft.getDisplay()->width() == 160){
    if (tft.getDisplay()->height() == 80){
      drawBitmap(16, 2, epd_bitmap_logo_green, 128, 56, LIGHT_GREEN, 1);
      drawBitmap(16, 2, epd_bitmap_logo_red, 128, 56, RED, 1);
    } else {
      drawBitmap(16, 20, epd_bitmap_logo_green, 128, 56, LIGHT_GREEN, 1);
      drawBitmap(16, 20, epd_bitmap_logo_red, 128, 56, RED, 1);
    }
  } else if (tft.getDisplay()->width() == 240){
    drawBitmap(56, tft.getDisplay()->height()/2 - 100, epd_bitmap_logo_green, 128, 56, LIGHT_GREEN, 1);
    drawBitmap(56, tft.getDisplay()->height()/2 - 100, epd_bitmap_logo_red, 128, 56, RED, 1);
  } else if (tft.getDisplay()->width() == 320){
    drawBitmap(32, tft.getDisplay()->height()/2 - 100, epd_bitmap_logo_green, 128, 56, LIGHT_GREEN, 2);
    drawBitmap(32, tft.getDisplay()->height()/2 - 100, epd_bitmap_logo_red, 128, 56, RED, 2);
  }

  tft.getDisplay()->setTextColor(WHITE);

  int16_t x, y;
  uint16_t width, height;

  tft.getDisplay()->setTextSize(1);
  tft.getDisplay()->getTextBounds("Powered by CoinGecko", 0, 0, &x, &y, &width, &height);
  tft.getDisplay()->setCursor(tft.getDisplay()->width()/2 - width/2, tft.getDisplay()->height() - 18);
  tft.getDisplay()->write("Powered by CoinGecko");

  delay(2500);
}

// Draws a passed bitmap on the screen at the given position with the given
// colour and scale.
void drawBitmap(int16_t x, int16_t y,
                const uint8_t* bitmap, int16_t w, int16_t h,
                uint16_t color, int scale) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(bitmap + j * byteWidth + i / 8);

      if (byte & 0x80) {
        // Draw a block of size scale x scale
        for (int dx = 0; dx < scale; dx++) {
          for (int dy = 0; dy < scale; dy++) {
            tft.getDisplay()->drawPixel(x + i * scale + dx, y + j * scale + dy, color);
          }
        }
      }
    }
  }
}


void updateTime() {
  // Check minutes and update time
  long current_second = millis() / 1000;

  // Update coins if time reached
  if ((current_second % UPDATE_PERIOD == 0) && !update_flip_flop) {
    update_flip_flop = true;
    refreshCoins();
    // if updated and not on cycle, re-display, otherwise will display naturally
    if (state == State::COIN && (menu->getCoinCycleDuration() == 0)) {
      display();
    } else if (state == State::PORTFOLIO && (menu->getPortfolioCycleDuration() == 0)) {
      display();
    }
  } else if (current_second % UPDATE_PERIOD != 0) {
    update_flip_flop = false;
  }

  // Move portfolio candles if needed
  if (((current_second % (menu->getPortfolioCandlePeriod() * 60) == 0)) && !portfolio_candle_flip_flop) {
    portfolio_candle_flip_flop = true;
    portfolio->nextTimePeriod();
  } else if ((current_second % (menu->getPortfolioCandlePeriod() * 60)) != 0) {
    portfolio_candle_flip_flop = false;
  }

  // Move coin candles if needed
  if (((current_second % (menu->getCoinCandlePeriod() * 60)) == 0) && !coin_candle_flip_flop) {
    coin_candle_flip_flop = true;
    bool* selected_coins = menu->getSelectedCoins();
    for (int i = 0; i < COIN_COUNT; i++) {
      if (selected_coins[i]) {
        coins[i]->candles->nextTimePeriod(coins[i]->current_price);
      }
    }
  } else if ((current_second % (menu->getCoinCandlePeriod() * 60)) != 0) {
    coin_candle_flip_flop = false;
  }

  // add new prices to candles once position adjustments have been made
  if ((current_second % UPDATE_PERIOD == 0) && !add_to_candles_flip_flop) {
    add_to_candles_flip_flop = true;
    addCoinPricesToCandles();
  } else if (current_second % UPDATE_PERIOD != 0) {
    add_to_candles_flip_flop = false;
  }

  // if in coin mode, move to next coin if cycle period reached (and is non-zero)
  if ((state == State::COIN) && (menu->getCoinCycleDuration() > 0)) {
    if ((current_second % menu->getCoinCycleDuration() == 0) && !coin_cycle_flip_flop) {
      coin_cycle_flip_flop = true;
      menu->getNextCoinIndex(&current_coin);
      display();
    } else if (current_second % menu->getCoinCycleDuration() != 0) {
      coin_cycle_flip_flop = false;
    }
  }

  // if in portfolio mode, move to next coin if cycle period reached (and is non-zero)
  if (state == State::PORTFOLIO && (menu->getPortfolioCycleDuration() > 0)) {
    if ((current_second % menu->getPortfolioCycleDuration() == 0) && !portfolio_cycle_flip_flop) {
      portfolio_cycle_flip_flop = true;
      portfolio->nextMode();
      display();
    } else if (current_second % menu->getPortfolioCycleDuration() != 0) {
      portfolio_cycle_flip_flop = false;
    }
  }
}

void checkSettingsForChanges() {
  bool needs_refresh = false;

  // if candle period has changed in coins/portfolio, clear the candles
  int coin_candle_period = 0;
  menu->getSliderValue("crypto_settings:candle_duration", &coin_candle_period);
  if (coin_candle_period != old_coin_candle_period){
    clearCoinCandles();
    needs_refresh = true;
  }

  int portfolio_candle_period = 0;
  menu->getSliderValue("portfolio_settings:candle_duration", &portfolio_candle_period);
  if (portfolio_candle_period != old_portfolio_candle_period){
    portfolio->clearCandles();
    needs_refresh = true;
  }

  // if portfolio amount has been changed, reset portfolio candles
  for (int i = 0; i < COIN_COUNT; i++){
    if (coins[i]->getAmountChanged()){ // this call resets the changed state
      portfolio->clearCandles();
      needs_refresh = true;
      break;
    }
  }

  // if currency changed, clear the candles
  int currency = 0;
  menu->getSelectorValue("customize:currency", &currency);
  if (currency != old_currency){
    clearCoinCandles();
    portfolio->clearCandles();
    needs_refresh = true;
  }

  if (needs_refresh){
    refreshAllCoins();
    addCoinPricesToCandles();
  }

  // set the old checkers now
  menu->getSliderValue("crypto_settings:candle_duration", &old_coin_candle_period);
  menu->getSliderValue("portfolio_settings:candle_duration", &old_portfolio_candle_period);
  menu->getSelectorValue("customize:currency", &old_currency);
}

// This function is called when we are in menu state
void interactWithMenu() {
  if (IrReceiver.decode()) {
    int rawCode = IrReceiver.decodedIRData.decodedRawData;

    switch (rawCode) {
      case IR_ZERO:
        menu->intInput(0);
        break;
      case IR_ONE:
        menu->intInput(1);
        break;
      case IR_TWO:
        menu->intInput(2);
        break;
      case IR_THREE:
        menu->intInput(3);
        break;
      case IR_FOUR:
        menu->intInput(4);
        break;
      case IR_FIVE:
        menu->intInput(5);
        break;
      case IR_SIX:
        menu->intInput(6);
        break;
      case IR_SEVEN:
        menu->intInput(7);
        break;
      case IR_EIGHT:
        menu->intInput(8);
        break;
      case IR_NINE:
        menu->intInput(9);
        break;
      case IR_UP:
        menu->moveUp();
        break;
      case IR_DOWN:
        menu->moveDown();
        break;
      case IR_LEFT:
        menu->moveLeft();
        break;
      case IR_RIGHT:
        menu->moveRight();
        break;
      case IR_OK:
        menu->press();
        break;
      case IR_HASHTAG:
        if (menu->back()) {
          menu->save();
          checkSettingsForChanges();
          state = last_state;
          display();
        }
        break;
    }

    IrReceiver.resume();
  }
}

void interactWithNetwork() {
  if (IrReceiver.decode()) {
    int rawCode = IrReceiver.decodedIRData.decodedRawData;

    switch (rawCode) {
      case IR_ZERO:
        break;
      case IR_ONE:
        network_manager->intInput(0);
        break;
      case IR_TWO:
        network_manager->intInput(1);
        break;
      case IR_THREE:
        network_manager->intInput(2);
        break;
      case IR_FOUR:
        network_manager->intInput(3);
        break;
      case IR_FIVE:
        break;
      case IR_SIX:
        break;
      case IR_SEVEN:
        break;
      case IR_EIGHT:
        break;
      case IR_NINE:
        break;
      case IR_UP:
        network_manager->moveUp();
        break;
      case IR_DOWN:
        network_manager->moveDown();
        break;
      case IR_LEFT:
        network_manager->moveLeft();
        break;
      case IR_RIGHT:
        network_manager->moveRight();
        break;
      case IR_OK:
        network_manager->press();
        break;
      case IR_HASHTAG:
        network_manager->back();
        break;
    }

    IrReceiver.resume();
  }

  if ((state == State::NETWORK) && network_manager->isConnected()) {
    state = State::COIN;
    display();
  }
}

// this function is called when we are in coin/portfolio state
void interact() {
  if (IrReceiver.decode()) {
    int rawCode = IrReceiver.decodedIRData.decodedRawData;

    switch (rawCode) {
      case IR_ZERO:
        break;
      case IR_ONE:
        break;
      case IR_TWO:
        break;
      case IR_THREE:
        break;
      case IR_FOUR:
        break;
      case IR_FIVE:
        break;
      case IR_SIX:
        break;
      case IR_SEVEN:
        break;
      case IR_EIGHT:
        break;
      case IR_NINE:
        break;
      case IR_UP:
        if (state == State::COIN) {
          state = State::PORTFOLIO;
        } else {
          state = State::COIN;
        }
        display();
        break;
      case IR_DOWN:
        if (state == State::COIN) {
          state = State::PORTFOLIO;
        } else {
          state = State::COIN;
        }
        display();
        break;
      case IR_LEFT:
        if (state == State::COIN) {
          menu->getPreviousCoinIndex(&current_coin);
        } else if (state == State::PORTFOLIO) {
          portfolio->previousMode();
        }
        display();
        break;
      case IR_RIGHT:
        if (state == State::COIN) {
          menu->getNextCoinIndex(&current_coin);
        } else if (state == State::PORTFOLIO) {
          portfolio->nextMode();
        }
        display();
        break;
      case IR_OK:
        if (state == State::PORTFOLIO) {
          int currency = 0;
          menu->getSelectorValue("customize:currency", &currency);
          portfolio->press(currency);
        }
        break;
      case IR_HASHTAG:
        if ((state == State::COIN) || (state == State::PORTFOLIO)) {
          last_state = state;
          state = State::MENU;
          display();
        }
        break;
    }

    IrReceiver.resume();
  }
}

void refreshAllCoins() {
  tft.getDisplay()->fillRect(tft.getDisplay()->width() - 2, 0, 2, 2, WHITE);
  int indexes[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  // refresh first 10 coins
  network_manager->refreshData(indexes, 10, menu->getCurrency());
  delay(5000);
  // refresh second 10 coins
  for (int i = 0; i < 10; i++) {
    indexes[i] += 10;
  }
  network_manager->refreshData(indexes, 10, menu->getCurrency());
  tft.getDisplay()->fillRect(tft.getDisplay()->width() - 2, 0, 2, 2, BLACK);
}

void clearCoinCandles(){
  for (int i = 0; i < COIN_COUNT; i++){
    coins[i]->candles->reset();
  }
}

void refreshCoins() {
  tft.getDisplay()->fillRect(tft.getDisplay()->width() - 2, 0, 2, 2, WHITE);
  int indexes[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  if (update_group_flip_flop) {
    // refresh first 10 coins
    network_manager->refreshData(indexes, 10, menu->getCurrency());
  } else {
    // refresh second 10 coins
    for (int i = 0; i < 10; i++) {
      indexes[i] += 10;
    }
    network_manager->refreshData(indexes, 10, menu->getCurrency());
  }
  update_group_flip_flop = !update_group_flip_flop;
  tft.getDisplay()->fillRect(tft.getDisplay()->width() - 2, 0, 2, 2, BLACK);
  portfolio->addPriceToCandles();
}

void addCoinPricesToCandles() {
  bool* selected_coins = menu->getSelectedCoins();
  for (int i = 0; i < COIN_COUNT; i++) {
    if (selected_coins[i]) {
      coins[i]->candles->addPrice(coins[i]->current_price);
    }
  }
  portfolio->addPriceToCandles();
}

void display() {
  // display stuff based on current state
  if (state == State::NETWORK) {
  } else if (state == State::MENU) {
    menu->display();
  } else if (state == State::COIN) {
    int currency = 0;
    menu->getSelectorValue("customize:currency", &currency);
    bool bitmap_enabled = false;
    menu->getCheckboxValue("crypto_settings:enable_bitmaps", &bitmap_enabled);
    if (!menu->isCoinSelected(current_coin)) {
      menu->getNextCoinIndex(&current_coin);  // get the next selected coin just in case the current one was deselected
    }
    coins[current_coin]->draw(currency, bitmap_enabled);
  } else if (state == State::PORTFOLIO) {
    int currency = 0;
    menu->getSelectorValue("customize:currency", &currency);
    portfolio->display(currency);
  }
}

void loop() {
  // IR receive loop
  if (state == State::NETWORK) {
    interactWithNetwork();
  } else if (state == State::MENU) {
    interactWithMenu();
  } else {
    interact();
  }

  // If not in setup or menu, update the coin prices
  if ((state == State::COIN) || (state == State::PORTFOLIO)) {
    updateTime();
  }
}
