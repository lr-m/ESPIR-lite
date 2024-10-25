#ifndef Network_Manager_ESP8266_h
#define Network_Manager_ESP8266_h

#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "Coin.h"
#include "Keyboard.h"

// Network configuration constants
#define MAX_NETWORKS 32
#define MAX_SSID_LENGTH 21
#define AP_LIST_COUNT 9
#define AP_ITEM_HEIGHT 12
#define AP_LABEL_HEIGHT 12

// URL configuration
#define URL_START "https://api.coingecko.com/api/v3/simple/price?ids="
#define URL_END "&include_24hr_change=true&vs_currencies="

// Memory and buffer configuration
#define CREDENTIALS_ADDRESS 448 // where we write the wifi ssid and password
#define SSID_LENGTH 32
#define INPUT_LENGTH_LIMIT 25
#define REQUEST_URL_BUFFER_SIZE 256
#define JSONDOC_PROCESSOR_SIZE 100
#define COIN_ID_BUFFER_SIZE 40

// WiFi signal strength configuration
#define RSSI_MAX -50
#define RSSI_MIN -100

// Magic number for credential verification
#define MAGIC_LENGTH 5
#define MAGIC "ESPIR"

// Network states
enum class Network_State {
    SELECTING_AP,
    ENTERING_PASSWORD,
    DISCONNECTED,
    CONNECTED
};

// Network structure
struct Network {
    char SSID[24] = { 0 };
    int strength = 0;
    bool hidden = false;
    bool password_needed = false;
};

class Network_Manager_ESP8266 {
public:
    // Constructor
    Network_Manager_ESP8266(Adafruit_GFX* display, COIN** coins);

    // Network scanning and display
    void scanForNetworks();
    void display();
    void displayAP(int index);
    void displayAPSelected(int index);
    void refreshNetworks();
    void sortNetworks(Network* networks, int size);

    // Navigation and input
    void press();
    void back();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void intInput(int input);

    // State management
    void enterState(Network_State new_state);
    Network_State getState() { return state; }
    bool isConnected() { return (state == Network_State::CONNECTED); }

    // Connection management
    bool loadAndConnect();
    bool reconnect();
    void clearCredentials();
    bool refreshData(int* coin_indexes, int count, const char* currency);

    // Credential management
    bool saveSSID(char* ssid);
    bool savePassword(char* password);
    bool loadSSID(char* ssid);
    bool loadPassword(char* password);
    bool saveMagic();
    bool loadMagic(char* magic);

    // URL handling
    void constructURL(int* coin_indexes, int count, const char* currency, char* url_buffer);

    // Keyboard management
    void initialiseKeyboard();
    void freeKeyboard();

private:
    // Private member variables
    WiFiClientSecure secureClient; 
    HTTPClient http;
    Network_State state;
    Network* networks = NULL;
    Adafruit_GFX* tft;
    COIN** coins;
    Keyboard* keyboard;
    uint16_t network_count = 0;
    uint16_t ap_display_list_base = 0;
    uint16_t selected_id = 0;
};

#endif // Network_Manager_ESP8266_h