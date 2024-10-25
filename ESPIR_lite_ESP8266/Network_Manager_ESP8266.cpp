#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "Network_Manager_ESP8266.h"

int dBmtoPercentage(int dBm) {
  int quality;
  if (dBm <= RSSI_MIN) {
    quality = 0;
  } else if (dBm >= RSSI_MAX) {
    quality = 100;
  } else {
    quality = 2 * (dBm + 100);
  }
  return quality;
}

uint16_t interpolateColor(uint16_t color1, uint16_t color2, float ratio) {
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5) & 0x3F;
    uint8_t b1 = color1 & 0x1F;
    
    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5) & 0x3F;
    uint8_t b2 = color2 & 0x1F;
    
    uint8_t r = r1 + (r2 - r1) * ratio;
    uint8_t g = g1 + (g2 - g1) * ratio;
    uint8_t b = b1 + (b2 - b1) * ratio;
    
    return (r << 11) | (g << 5) | b;
}

Network_Manager_ESP8266::Network_Manager_ESP8266(Adafruit_GFX* tft, COIN** coins)
  : tft(tft), coins(coins) {
}

void Network_Manager_ESP8266::scanForNetworks() {
  tft->fillScreen(BLACK);
  state = Network_State::SELECTING_AP;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  int all_networks_count = WiFi.scanNetworks();
  if (all_networks_count == 0) {
    // no networks found
  } else {
    network_count = 0;

    if (all_networks_count > MAX_NETWORKS) {
      all_networks_count = MAX_NETWORKS;
    }

    networks = (Network*) malloc(sizeof(Network) * all_networks_count);

    for (int i = 0; i < all_networks_count; i++) {
      int ssid_len = strlen(WiFi.SSID(i).c_str());
      if (ssid_len >= MAX_SSID_LENGTH - 1) {
          continue;
      }

      strncpy(networks[network_count].SSID, WiFi.SSID(i).c_str(), ssid_len);
      networks[network_count].SSID[ssid_len] = 0;
      networks[network_count].strength = dBmtoPercentage(WiFi.RSSI(i));
      networks[network_count].hidden = WiFi.isHidden(i);
      networks[network_count].password_needed = (WiFi.encryptionType(i) != ENC_TYPE_NONE);

      network_count++;
    }

    sortNetworks(networks, network_count);
    display();
  }
}

void Network_Manager_ESP8266::enterState(Network_State new_state) {
  state = new_state;
}

void Network_Manager_ESP8266::display() {
  if (state == Network_State::SELECTING_AP) {
    tft->fillScreen(BLACK);
    tft->setCursor(2, 2);
    tft->setTextColor(GRAY);
    tft->print("Select your network:");
    for (int i = ap_display_list_base; i < ap_display_list_base + (int)((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT); i++) {
      if (i >= network_count) {
        break;
      }

      if (i == selected_id) {
        displayAPSelected(i);
      } else {
        displayAP(i);
      }
    }
  } else if (state == Network_State::ENTERING_PASSWORD) {
    tft->fillScreen(BLACK);

    // init keyboard for password entry
    initialiseKeyboard();
    
    keyboard->displayPrompt("Enter password:");
    keyboard->display();
  }
}

void Network_Manager_ESP8266::initialiseKeyboard(){
    keyboard = new Keyboard(tft);
    keyboard->setLengthLimit(INPUT_LENGTH_LIMIT);
}

void Network_Manager_ESP8266::freeKeyboard(){
    if(keyboard != nullptr) {
        delete keyboard;
        keyboard = nullptr;  // Prevent dangling pointer
    }
}

void Network_Manager_ESP8266::displayAP(int index) {
  if (networks[index].strength > 66) {
    tft->drawLine(2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 
                  2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 2, GREEN);
  }
  if (networks[index].strength > 33) {
    tft->drawLine(6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base),
                  6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 5, ORANGE);
  }
  if (networks[index].strength > 0) {
    tft->drawLine(10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base),
                  10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 8, RED);
  }

  tft->setCursor(20, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 4);
  tft->setTextColor(DARK_GREY);
  tft->print(networks[index].SSID);
}

void Network_Manager_ESP8266::displayAPSelected(int index) {
  if (networks[index].strength > 66) {
    tft->drawLine(2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base),
                  2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 2, GREEN);
  }
  if (networks[index].strength > 33) {
    tft->drawLine(6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base),
                  6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 5, ORANGE);
  }
  if (networks[index].strength > 0) {
    tft->drawLine(10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base),
                  10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 8, RED);
  }

  tft->setCursor(20, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 4);
  tft->setTextColor(WHITE);
  tft->print(networks[index].SSID);
}

void Network_Manager_ESP8266::sortNetworks(Network* networks, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; ++j) {
      if (networks[j].strength < networks[j + 1].strength) {
        Network temp = networks[j];
        networks[j] = networks[j + 1];
        networks[j + 1] = temp;
      }
    }
  }
}

void Network_Manager_ESP8266::press() {
  if (getState() == Network_State::SELECTING_AP) {
    saveMagic();
    saveSSID(networks[selected_id].SSID);
    state = Network_State::ENTERING_PASSWORD;
    display();
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->press();
    if (keyboard->enterPressed()) {
      savePassword(keyboard->getCurrentInput());
      state = Network_State::DISCONNECTED;

      // we have fetched keyboard input now, so we can free it
      freeKeyboard();

      if (!loadAndConnect()) {
        scanForNetworks();
      }
    }
  }
}

void Network_Manager_ESP8266::refreshNetworks() {
  if (getState() == Network_State::SELECTING_AP) {
    scanForNetworks();
  }
}

bool Network_Manager_ESP8266::loadMagic(char* magic) {
  for (size_t i = 0; i < MAGIC_LENGTH; i++) {
    magic[i] = EEPROM.read(CREDENTIALS_ADDRESS + i);
    if (magic[i] == 0) {
      break;
    }
  }
  return true;
}

bool Network_Manager_ESP8266::saveMagic() {
  for (size_t i = 0; i < MAGIC_LENGTH; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + i, MAGIC[i]);
  }
  EEPROM.commit();
  return true;
}

bool Network_Manager_ESP8266::loadSSID(char* ssid) {
  for (size_t i = 0; i < SSID_LENGTH; i++) {
    ssid[i] = EEPROM.read(CREDENTIALS_ADDRESS + MAGIC_LENGTH + i);
    if (ssid[i] == 0) {
      break;
    }
  }
  if (ssid[0] == 0) {
    return false;
  }
  return true;
}

bool Network_Manager_ESP8266::loadPassword(char* password) {
  for (size_t i = 0; i < SSID_LENGTH; i++) {
    password[i] = EEPROM.read(CREDENTIALS_ADDRESS + MAGIC_LENGTH + SSID_LENGTH + i);
    if (password[i] == 0) {
      break;
    }
  }
  return true;
}

bool Network_Manager_ESP8266::saveSSID(char* ssid) {
  int len = strlen(ssid);
  if (len >= SSID_LENGTH) {
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + i, ssid[i]);
  }
  EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + len, 0);
  EEPROM.commit();
  return true;
}

bool Network_Manager_ESP8266::savePassword(char* password) {
  int len = strlen(password);
  if (len >= SSID_LENGTH) {
    return false;
  }

  for (size_t i = 0; i < len; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + SSID_LENGTH + i, password[i]);
  }
  EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + SSID_LENGTH + len, 0);
  EEPROM.commit();
  return true;
}

void Network_Manager_ESP8266::back() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    state = Network_State::SELECTING_AP;
    display();

    freeKeyboard(); // no longer need keyboard so we can just free it up
  }
}

void Network_Manager_ESP8266::moveUp() {
  if (getState() == Network_State::SELECTING_AP) {
    displayAP(selected_id);
    selected_id = (selected_id == 0) ? min((uint16_t)((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT), network_count) - 1 : selected_id - 1;
    displayAPSelected(selected_id);
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveUp();
  }
}

void Network_Manager_ESP8266::moveDown() {
  if (getState() == Network_State::SELECTING_AP) {
    displayAP(selected_id);
    selected_id = (selected_id == min((uint16_t)((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT), network_count) - 1) ? 0 : selected_id + 1;
    displayAPSelected(selected_id);
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveDown();
  }
}

void Network_Manager_ESP8266::moveLeft() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveLeft();
  }
}

void Network_Manager_ESP8266::moveRight() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveRight();
  }
}

void Network_Manager_ESP8266::intInput(int input) {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    if (input == 0) {
      keyboard->setMode(Mode::LOWER);
    } else if (input == 1) {
      keyboard->setMode(Mode::UPPER);
    } else if (input == 2) {
      keyboard->setMode(Mode::NUMBERS);
    } else if (input == 3) {
      keyboard->setMode(Mode::SPECIALS);
    } else if (input == 4) {
      keyboard->setMode(Mode::BOTTOM);
    }
  }
}

bool Network_Manager_ESP8266::reconnect() {
  char ssid[SSID_LENGTH];
  char password[SSID_LENGTH];

  if (loadSSID(ssid) && loadPassword(password)) {
    WiFi.begin(ssid, password);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      i++;

      if (i == 30) {
        return false;
      }
    }

    state = Network_State::CONNECTED;
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    return true;
  }
  return false;
}

void Network_Manager_ESP8266::clearCredentials() {
  for (size_t i = 0; i < SSID_LENGTH * 2; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + i, 0);
  }
  EEPROM.commit();
}

void Network_Manager_ESP8266::constructURL(int* refresh_ids, int count, const char* currency, char* url_buffer) {
  int curr_url_pos = 0;

  strcpy(url_buffer + curr_url_pos, URL_START);
  curr_url_pos += strlen(URL_START);

  for (int j = 0; j < count - 1; j++) {
    strcpy(url_buffer + curr_url_pos, coins[refresh_ids[j]]->coin_id);
    curr_url_pos += strlen(coins[refresh_ids[j]]->coin_id);

    url_buffer[curr_url_pos++] = '%';
    url_buffer[curr_url_pos++] = '2';
    url_buffer[curr_url_pos++] = 'C';
  }

  strcpy(url_buffer + curr_url_pos, coins[refresh_ids[count - 1]]->coin_id);
  curr_url_pos += strlen(coins[refresh_ids[count - 1]]->coin_id);

  strcpy(url_buffer + curr_url_pos, URL_END);
  curr_url_pos += strlen(URL_END);

  strncpy(url_buffer + curr_url_pos, currency, 3);
  curr_url_pos += 3;

  url_buffer[curr_url_pos] = 0;
}

bool Network_Manager_ESP8266::loadAndConnect() {
  char magic[MAGIC_LENGTH];
  char ssid[SSID_LENGTH];
  char password[SSID_LENGTH];

  loadMagic(magic);

  if (memcmp(magic, MAGIC, MAGIC_LENGTH) != 0) {
    return false;
  } else if (loadSSID(ssid) && loadPassword(password)) {
    tft->fillScreen(BLACK);
    tft->setTextSize(1);
    tft->setTextColor(WHITE);
    tft->setCursor(0, 10);
    tft->print(" SSID: ");
    tft->setTextColor(LIGHT_RED);
    tft->println(ssid);
    tft->setTextColor(WHITE);
    tft->print(" PWD:  ");
    tft->setTextColor(LIGHT_RED);
    tft->println(password);
    tft->setTextColor(WHITE);
    WiFi.begin(ssid, password);
    tft->print("\n ");

    const int TOTAL_DURATION_MS = 10000;
    const int MIN_SEGMENT_WIDTH = 3;
    const int progressWidth = (4 * tft->width() / 5);
    const int start = tft->width() / 10;

    int maxIterations = progressWidth / MIN_SEGMENT_WIDTH;
    int delayMs = TOTAL_DURATION_MS / maxIterations;
    int segmentWidth = progressWidth / maxIterations;

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(delayMs);
        i++;
        
        int xPos = start + (i - 1) * segmentWidth;
        float ratio = (float)(i - 1) / (maxIterations - 1);
        uint16_t color = interpolateColor(LIGHT_GREEN, LIGHT_RED, ratio);
        
        tft->fillRect(xPos, 34, segmentWidth, 8, color);
        
        if (i == maxIterations) {
          tft->print("\n ");
          tft->setTextColor(LIGHT_RED);
          tft->println("\n Failed!");
          tft->setTextColor(WHITE);
          delay(1000);
          return false;
        }
    }

    state = Network_State::CONNECTED;
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    tft->print("\n ");
    tft->setTextColor(LIGHT_GREEN);
    tft->println("\n Connected!");

    if (tft->height() > 80) {
      tft->setTextColor(WHITE);
      tft->println("\n IP address: ");
      tft->print(" ");
      tft->println(WiFi.localIP());
    }

    // do cleanup if wifi discovery used
    if (networks != NULL) {
        WiFi.scanDelete();
        free(networks);
        networks = NULL;  // Good practice to prevent use-after-free
        network_count = 0;
    }

    delay(1000);
    return true;
  }
  return false;
}

bool Network_Manager_ESP8266::refreshData(int* coin_indexes, int count, const char* currency) {
    // Instanciate Secure HTTP communication
    char* request_url = (char*) malloc(REQUEST_URL_BUFFER_SIZE);
    if (!request_url) return false;
    
    http.useHTTP10(true);
    constructURL(coin_indexes, count, currency, request_url);  // Get the URL
    
    // Init secure client
    secureClient.setInsecure();
    
    // HTTP request
    http.begin(secureClient, request_url);
    int httpCode = http.GET();
    
    // free the request url ASAP to ease heap pressure
    free(request_url);
    request_url = NULL;

    // Buffers for data
    StaticJsonDocument<JSONDOC_PROCESSOR_SIZE> doc;
    char id_buffer[COIN_ID_BUFFER_SIZE];
    
    // OK http response
    if (httpCode == 200) {
        int read_bytes;
        // Parse incoming JSON from stream
        http.getStream().find("\"");
        do {
            read_bytes = http.getStream().readBytesUntil('"', id_buffer, COIN_ID_BUFFER_SIZE - 1);
            id_buffer[read_bytes] = 0;
            http.getStream().find(':');
            deserializeJson(doc, http.getStream());
            // Add to coin or portfolio
            for (int i = 0; i < count; i++) {
                if (strcmp(id_buffer, coins[coin_indexes[i]]->coin_id) == 0
                    && doc[currency] > 0) {

                    snprintf(id_buffer, sizeof(id_buffer), "%.*s_24h_change", 3, currency);

                    coins[coin_indexes[i]]->current_price = doc[currency];
                    coins[coin_indexes[i]]->current_change = doc[id_buffer];
                    coins[coin_indexes[i]]->candles->addPrice(doc[currency]);
                    break;
                }
            }
        } while (http.getStream().findUntil(",\"", "}"));  // Continue until last '}' reached
        http.end();
        return true;
    } else if (httpCode == -1) {
        http.end();
        state = Network_State::DISCONNECTED;
        bool reconnect_result = reconnect(); // try and reconnect
        return false;
    } else {
        http.end();
        return false;
    }
}
