#include <cstring>
#include "Network_Manager_ESP32.h"

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
    // Extract RGB components from color1 (green)
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5) & 0x3F;
    uint8_t b1 = color1 & 0x1F;
    
    // Extract RGB components from color2 (red)
    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5) & 0x3F;
    uint8_t b2 = color2 & 0x1F;
    
    // Interpolate between components
    uint8_t r = r1 + (r2 - r1) * ratio;
    uint8_t g = g1 + (g2 - g1) * ratio;
    uint8_t b = b1 + (b2 - b1) * ratio;
    
    // Combine back into 565 format
    return (r << 11) | (g << 5) | b;
}

Network_Manager_ESP32::Network_Manager_ESP32(Adafruit_GFX* tft, COIN** coins)
  : tft(tft), coins(coins), keyboard(keyboard) {
}

void Network_Manager_ESP32::scanForNetworks() {
  // reset keyboard and screen
  tft->fillScreen(BLACK);

  // Indicate we are now selecting an access point
  state = Network_State::SELECTING_AP;

  // Set network to listen for access points
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  int all_networks_count = WiFi.scanNetworks();
  if (all_networks_count == 0) {
    // no networks found
  } else {
    network_count = 0;

    // more than 32 ap's nearby?!?!?!
    if (all_networks_count > MAX_NETWORKS) {
      all_networks_count = MAX_NETWORKS;
    }

    // Dynamically allocate network buffer
    networks = (Network*)malloc(sizeof(Network) * all_networks_count);

    // populate the buffers
    for (int i = 0; i < all_networks_count; i++) {
      int ssid_len = std::strlen(WiFi.SSID(i).c_str());
      if (ssid_len >= MAX_SSID_LENGTH - 1) {
          continue;
      }

      // Copy properties into network object at the correct position
      strncpy(networks[network_count].SSID, WiFi.SSID(i).c_str(), ssid_len);
      networks[network_count].SSID[ssid_len] = 0; // Ensure null terminator
      networks[network_count].strength = dBmtoPercentage(WiFi.RSSI(i));
      networks[network_count].hidden = false; // ESP32 doesn't have an isHidden() method
      networks[network_count].password_needed = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);

      // Only increment network_count after a valid network is added
      network_count++;
    }

    // now sort the networks by the signal strength
    sortNetworks(networks, network_count);

    // now display the networks that have been found
    display();
  }
}

void Network_Manager_ESP32::enterState(Network_State new_state) {
  state = new_state;
}

void Network_Manager_ESP32::display(){
  if (state == Network_State::SELECTING_AP) {
    tft->fillScreen(BLACK);
    tft->setCursor(2, 2);
    tft->setTextColor(GRAY);
    tft->print("Select your network:");
    for (int i = ap_display_list_base; i < ap_display_list_base + (int) ((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT); i++) {
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

void Network_Manager_ESP32::initialiseKeyboard(){
    keyboard = new Keyboard(tft);
    keyboard->setLengthLimit(INPUT_LENGTH_LIMIT);
}

void Network_Manager_ESP32::freeKeyboard(){
    if(keyboard != nullptr) {
        delete keyboard;
        keyboard = nullptr;  // Prevent dangling pointer
    }
}


void Network_Manager_ESP32::displayAP(int index) {
  // display strength indicator
  if (networks[index].strength > 66) {
    tft->drawLine(2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 2, GREEN);
  }
  if (networks[index].strength > 33) {
    tft->drawLine(6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 5, ORANGE);
  }
  if (networks[index].strength > 0) {
    tft->drawLine(10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 8, RED);
  }

  // display SSID
  tft->setCursor(20, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 4);
  tft->setTextColor(DARK_GREY);
  tft->print(networks[index].SSID);
}

void Network_Manager_ESP32::displayAPSelected(int index) {
  // display strength indicator
  if (networks[index].strength > 66) {
    tft->drawLine(2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 2, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 2, GREEN);
  }
  if (networks[index].strength > 33) {
    tft->drawLine(6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 6, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 5, ORANGE);
  }
  if (networks[index].strength > 0) {
    tft->drawLine(10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * ((index + 1) - ap_display_list_base), 10, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 8, RED);
  }

  // display SSID
  tft->setCursor(20, AP_LABEL_HEIGHT + AP_ITEM_HEIGHT * (index - ap_display_list_base) + 4);
  tft->setTextColor(WHITE);
  tft->print(networks[index].SSID);
}

void Network_Manager_ESP32::sortNetworks(Network* networks, int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; ++j) {
      // Use the comparison function to determine the order
      if (networks[j].strength < networks[j + 1].strength) {
        // Swap arr[j] and arr[j + 1]
        Network temp = networks[j];
        networks[j] = networks[j + 1];
        networks[j + 1] = temp;
      }
    }
  }
}

void Network_Manager_ESP32::press() {
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

      if (!loadAndConnect()){
        scanForNetworks();
      }
    }
  }
}

void Network_Manager_ESP32::refreshNetworks(){
  if (getState() == Network_State::SELECTING_AP) {
    scanForNetworks();
  }
}

bool Network_Manager_ESP32::loadMagic(char* magic) {
  // load from EEPROM
  for (size_t i = 0; i < MAGIC_LENGTH; i++) {
    magic[i] = EEPROM.read(CREDENTIALS_ADDRESS + i);
    if (magic[i] == 0) {
      break;
    }
  }
  return true;
}

bool Network_Manager_ESP32::saveMagic() {
  // write to EEPROM
  for (size_t i = 0; i < MAGIC_LENGTH; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + i, MAGIC[i]);
  }
  EEPROM.commit();  // Commit the changes to flash
  return true;
}

bool Network_Manager_ESP32::loadSSID(char* ssid) {
  // load from EEPROM
  for (size_t i = 0; i < SSID_LENGTH; i++) {
    ssid[i] = EEPROM.read(CREDENTIALS_ADDRESS + MAGIC_LENGTH + i);
    if (ssid[i] == 0) {
      break;
    }
  }

  // if empty, return false
  if (ssid[0] == 0) {
    return false;
  }
  return true;
}

bool Network_Manager_ESP32::loadPassword(char* password) {
  // load from EEPROM
  for (size_t i = 0; i < SSID_LENGTH; i++) {
    password[i] = EEPROM.read(CREDENTIALS_ADDRESS + MAGIC_LENGTH + SSID_LENGTH + i);
    if (password[i] == 0) {
      break;
    }
  }

  // always return true as can have unprotected networks (but not the best idea!)
  return true;
}

bool Network_Manager_ESP32::saveSSID(char* ssid) {
  int len = strlen(ssid);
  if (len >= SSID_LENGTH) {
    return false;
  }

  // write to EEPROM
  for (size_t i = 0; i < len; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + i, ssid[i]);
  }
  EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + len, 0);
  EEPROM.commit();  // Commit the changes to flash
  return true;
}

bool Network_Manager_ESP32::savePassword(char* password) {
  int len = strlen(password);
  if (len >= SSID_LENGTH) {
    return false;
  }

  // write to EEPROM
  for (size_t i = 0; i < len; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + SSID_LENGTH + i, password[i]);
  }
  EEPROM.write(CREDENTIALS_ADDRESS + MAGIC_LENGTH + SSID_LENGTH + len, 0);
  EEPROM.commit();  // Commit the changes to flash
  return true;
}

void Network_Manager_ESP32::back() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    state = Network_State::SELECTING_AP;
    display();

    freeKeyboard(); // no longer need keyboard so we can just free it up
  }
}

void Network_Manager_ESP32::moveUp() {
  if (getState() == Network_State::SELECTING_AP) {
    displayAP(selected_id);
    selected_id = (selected_id == 0) ? min((uint16_t) ((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT), network_count) - 1 : selected_id - 1;
    displayAPSelected(selected_id);
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveUp();
  }
}

void Network_Manager_ESP32::moveDown() {
  if (getState() == Network_State::SELECTING_AP) {
    displayAP(selected_id);
    selected_id = (selected_id == min((uint16_t) ((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT), network_count) - 1) ? 0 : selected_id + 1;
    displayAPSelected(selected_id);
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveDown();
  }
}

void Network_Manager_ESP32::moveLeft() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveLeft();
  }
}

void Network_Manager_ESP32::moveRight() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveRight();
  }
}

void Network_Manager_ESP32::intInput(int input) {
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

bool Network_Manager_ESP32::loadAndConnect() {
  char magic[MAGIC_LENGTH];
  char ssid[SSID_LENGTH];
  char password[SSID_LENGTH];

  // first check the magic at the start of the EEPROm
  loadMagic(magic);

  // first check magic
  if (memcmp(magic, MAGIC, MAGIC_LENGTH) != 0){
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
        
        // Calculate the x position of the rectangle
        int xPos = start + (i - 1) * segmentWidth;
        
        // Calculate color interpolation ratio (0.0 to 1.0)
        float ratio = (float)(i - 1) / (maxIterations - 1);
        
        // Interpolate between green (0x4a0) and red (0xf800)
        uint16_t color = interpolateColor(LIGHT_GREEN, LIGHT_RED, ratio);
        
        // Draw the rectangle on the screen
        tft->fillRect(xPos, 34, segmentWidth, 8, color);
        
        // If the maximum number of iterations is reached, reset and scan for networks
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

    // only print if display is high enough
    if (tft->height() > 80){
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
  } else {
    return false;
  }
}

bool Network_Manager_ESP32::reconnect() {
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

void Network_Manager_ESP32::clearCredentials() {
  for (size_t i = 0; i < SSID_LENGTH * 2; i++) {
    EEPROM.write(CREDENTIALS_ADDRESS + i, 0);
  }
  EEPROM.commit();  // Commit the changes to flash
}

void printHeapUsage() {
  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");

  // ESP32 doesn't have a direct equivalent for getHeapFragmentation()
  // You can use esp_get_free_heap_size() and esp_get_minimum_free_heap_size() if needed
  Serial.print("Minimum Free Heap: ");
  Serial.print(ESP.getMinFreeHeap());
  Serial.println(" bytes");
}

bool Network_Manager_ESP32::refreshData(int* coin_indexes, int count, const char* currency) {
  // Instantiate Secure HTTP communication
  char request_url[REQUEST_URL_BUFFER_SIZE] = { 0 };
  constructURL(coin_indexes, count, currency, request_url);  // Get the URL

  // Init client
  client.setInsecure();

  // HTTP request
  http.begin(client, request_url);
  int httpCode = http.GET();

  // Buffers for data
  char currency_change_string[CURRENT_STRING_BUFFER_SIZE];
  snprintf(currency_change_string, sizeof(currency_change_string), "%.*s_24h_change", 3, currency);

  bool success = false;

  // Handle HTTP response
  if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      if (payload.length() > 0) {
          DynamicJsonDocument jsonDoc(JSONDOC_PROCESSOR_SIZE);
          DeserializationError error = deserializeJson(jsonDoc, payload);
          
          if (!error) {
              for (int i = 0; i < count; i++) {
                  const char* coin_id = coins[coin_indexes[i]]->coin_id;
                  if (!coin_id) continue;  // Skip if coin_id is null

                  if (jsonDoc.containsKey(coin_id)) {
                      JsonObject coinObj = jsonDoc[coin_id];
                      if (coinObj.containsKey(currency) && 
                          coinObj[currency].is<float>() && 
                          coinObj[currency] > 0) {
                          
                          float price = coinObj[currency];
                          float change = coinObj[currency_change_string] | 0.0f;  // Default to 0 if missing

                          coins[coin_indexes[i]]->current_price = price;
                          coins[coin_indexes[i]]->current_change = change;
                          
                          if (coins[coin_indexes[i]]->candles) {
                              coins[coin_indexes[i]]->candles->addPrice(price);
                          }
                          success = true;
                      }
                  }
              }
          }
      }
  } else if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED) {
      // try and reconnect
      state = Network_State::DISCONNECTED;
      http.end();
      return reconnect();
  }

  http.end();
  return success;
}

void Network_Manager_ESP32::constructURL(int* refresh_ids, int count, const char* currency, char* url_buffer) {
  int curr_url_pos = 0;

  // Add start of url
  std::strcpy(url_buffer + curr_url_pos, URL_START);
  curr_url_pos += std::strlen(URL_START);

  // Add coins selected in the specific coin part
  // Add all coins to url except last
  for (int j = 0; j < count - 1; j++) {
    std::strcpy(url_buffer + curr_url_pos, coins[refresh_ids[j]]->coin_id);
    curr_url_pos += std::strlen(coins[refresh_ids[j]]->coin_id);

    url_buffer[curr_url_pos++] = '%';
    url_buffer[curr_url_pos++] = '2';
    url_buffer[curr_url_pos++] = 'C';
  }

  // Add final coin to url
  std::strcpy(url_buffer + curr_url_pos, coins[refresh_ids[count - 1]]->coin_id);
  curr_url_pos += std::strlen(coins[refresh_ids[count - 1]]->coin_id);

  // Add ending to URL
  std::strcpy(url_buffer + curr_url_pos, URL_END);
  curr_url_pos += std::strlen(URL_END);

  // Append the currency
  std::strncpy(url_buffer + curr_url_pos, currency, 3);
  curr_url_pos += 3;

  url_buffer[curr_url_pos] = 0;
}
