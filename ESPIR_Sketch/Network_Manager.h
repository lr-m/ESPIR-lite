#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "COIN.h"
#include "Keyboard.h"
#include "EEPROM.h"

#ifndef Network_Manager_h
#define Network_Manager_h

#define MAX_NETWORKS 32
#define MAX_SSID_LENGTH 21
#define AP_LIST_COUNT 9
#define AP_ITEM_HEIGHT 12
#define AP_LABEL_HEIGHT 12

#define URL_START "https://api.coingecko.com/api/v3/simple/price?ids="
#define URL_END "&include_24hr_change=true&vs_currencies="

#define CREDENTIALS_ADDRESS 448  // where we write the wifi ssid and password
#define SSID_LENGTH 32

#define INPUT_LENGTH_LIMIT 25

#define RSSI_MAX -50
#define RSSI_MIN -100

enum class Network_State {
  SELECTING_AP,
  ENTERING_PASSWORD,
  DISCONNECTED,
  CONNECTED
};

struct Network {
  char SSID[24] = { 0 };
  int strength = 0;
  bool hidden = false;
  bool password_needed = false;
};

class Network_Manager {
public:
  Network_Manager(Adafruit_ST7735* display, COIN** coins, Keyboard* keyboard);
  void scanForNetworks();
  void press();
  void back();
  void moveUp();
  void moveDown();
  void moveLeft();
  void moveRight();
  void intInput(int input);
  void display();
  void loadAndConnect();
  void reconnect();
  void clearCredentials();
  void setInitialised(bool new_initialised) {
    isInitialised = new_initialised;
  }
  void refreshData(int* coin_indexes, int count, const char* currency);
  void sortNetworks(Network* networks, int size);
  void displayAP(int index);
  void displayAPSelected(int index);
  Network_State getState() {
    return state;
  }
  Adafruit_ST7735* getScreen() {
    return screen;
  }
  bool saveSSID(char* ssid);
  bool savePassword(char* password);
  bool loadSSID(char* ssid);
  bool loadPassword(char* password);
  bool isConnected() {
    return (state == Network_State::CONNECTED);
  }
  void constructURL(int* coin_indexes, int count, const char* currency, char* url_buffer);

private:
  WiFiClientSecure client;
  HTTPClient http;
  Network_State state;
  Network* networks;
  bool isInitialised = false;
  Adafruit_ST7735* screen;
  uint16_t network_count = 0;
  uint16_t ap_display_list_base = 0;
  COIN** coins;
  uint16_t selected_id = 0;
  Keyboard* keyboard;
};

#endif