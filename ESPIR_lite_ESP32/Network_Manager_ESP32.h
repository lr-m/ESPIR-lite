#include <EEPROM.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Coin.h"
#include "Keyboard.h"
#include "EEPROM.h"

#ifndef Network_Manager_ESP32_h
#define Network_Manager_ESP32_h

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

#define REQUEST_URL_BUFFER_SIZE 256
#define JSONDOC_PROCESSOR_SIZE 1024
#define CURRENT_STRING_BUFFER_SIZE 16

#define MAGIC_LENGTH 5
#define MAGIC "ESPIR"

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

class Network_Manager_ESP32 {
public:
  Network_Manager_ESP32(Adafruit_GFX* display, COIN** coins);
  void scanForNetworks();
  void press();
  void back();
  void moveUp();
  void moveDown();
  void moveLeft();
  void moveRight();
  void intInput(int input);
  void enterState(Network_State new_state);
  void display();
  bool loadAndConnect();
  bool reconnect();
  void clearCredentials();
  void refreshNetworks();
  bool refreshData(int* coin_indexes, int count, const char* currency);
  void sortNetworks(Network* networks, int size);
  void displayAP(int index);
  void displayAPSelected(int index);
  Network_State getState() {
    return state;
  }
  bool saveSSID(char* ssid);
  bool savePassword(char* password);
  bool loadSSID(char* ssid);
  bool loadPassword(char* password);
  bool saveMagic();
  bool loadMagic(char* magic);
  bool isConnected() {
    return (state == Network_State::CONNECTED);
  }
  void constructURL(int* coin_indexes, int count, const char* currency, char* url_buffer);
  void enterCredentials();

  // Keyboard management
    void initialiseKeyboard();
    void freeKeyboard();

private:
  WiFiClientSecure client;
  HTTPClient http;
  Network_State state;
  Network* networks = NULL;
  Adafruit_GFX* tft;
  uint16_t network_count = 0;
  uint16_t ap_display_list_base = 0;
  COIN** coins;
  uint16_t selected_id = 0;
  Keyboard* keyboard;
};

#endif