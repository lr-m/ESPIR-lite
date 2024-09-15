#include <cstring>
#include "Network_Manager.h"

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

Network_Manager::Network_Manager(Adafruit_GFX* tft, COIN** coins, Keyboard* keyboard)
  : tft(tft), coins(coins), keyboard(keyboard) {
  keyboard->setLengthLimit(INPUT_LENGTH_LIMIT);
}


void Network_Manager::scanForNetworks() {
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
    for (int i = 0; i < all_networks_count; ++i) {
      int ssid_len = std::strlen(WiFi.SSID(i).c_str());
      if (ssid_len >= MAX_SSID_LENGTH - 1) {
          continue;
      }

      // Copy properties into network object at the correct position
      strncpy(networks[network_count].SSID, WiFi.SSID(i).c_str(), ssid_len);
      networks[network_count].SSID[ssid_len] = 0; // Ensure null terminator
      networks[network_count].strength = dBmtoPercentage(WiFi.RSSI(i));
      networks[network_count].hidden = WiFi.isHidden(i);
      networks[network_count].password_needed = (i == 7) ? false : true;

      // Only increment network_count after a valid network is added
      network_count++;
  }

    // now sort the networks by the signal strength
    sortNetworks(networks, network_count);

    // now display the networks that have been found
    display();
  }
}

void Network_Manager::display() {
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
    keyboard->displayPrompt("Enter password:");
    keyboard->display();
  }
}

void Network_Manager::displayAP(int index) {
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

void Network_Manager::displayAPSelected(int index) {
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


void Network_Manager::sortNetworks(Network* networks, int size) {
  for (int i = 0; i < size - 1; ++i) {
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

void Network_Manager::press() {
  if (getState() == Network_State::SELECTING_AP) {
    saveSSID(networks[selected_id].SSID);
    state = Network_State::ENTERING_PASSWORD;
    display();
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->press();
    if (keyboard->enterPressed()) {
      savePassword(keyboard->getCurrentInput());
      state = Network_State::DISCONNECTED;
      loadAndConnect();
    }
  }
}

bool Network_Manager::loadSSID(char* ssid) {
  // load from EEPROM
  for (size_t i = 0; i < SSID_LENGTH; ++i) {
    ssid[i] = EEPROM.read(CREDENTIALS_ADDRESS + i);
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

bool Network_Manager::loadPassword(char* password) {
  // load from EEPROM
  for (size_t i = 0; i < SSID_LENGTH; ++i) {
    password[i] = EEPROM.read(CREDENTIALS_ADDRESS + SSID_LENGTH + i);
    if (password[i] == 0) {
      break;
    }
  }

  // if empty, return false
  if (password[0] == 0) {
    return false;
  }
  return true;
}

bool Network_Manager::saveSSID(char* ssid) {
  int len = strlen(ssid);
  if (len >= SSID_LENGTH) {
    return false;
  }

  // write to EEPROM
  for (size_t i = 0; i < len; ++i) {
    EEPROM.write(CREDENTIALS_ADDRESS + i, ssid[i]);
  }
  EEPROM.write(CREDENTIALS_ADDRESS + len, 0);
  EEPROM.commit();  // Commit the changes to flash
}

bool Network_Manager::savePassword(char* password) {
  int len = strlen(password);
  if (len >= SSID_LENGTH) {
    return false;
  }

  // write to EEPROM
  for (size_t i = 0; i < len; ++i) {
    EEPROM.write(CREDENTIALS_ADDRESS + SSID_LENGTH + i, password[i]);
  }
  EEPROM.write(CREDENTIALS_ADDRESS + SSID_LENGTH + len, 0);
  EEPROM.commit();  // Commit the changes to flash
}

void Network_Manager::back() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    state = Network_State::SELECTING_AP;
    keyboard->reset();
    display();
  }
}

void Network_Manager::moveUp() {
  if (getState() == Network_State::SELECTING_AP) {
    displayAP(selected_id);
    selected_id = (selected_id == 0) ? min((uint16_t) ((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT), network_count) - 1 : selected_id - 1;
    displayAPSelected(selected_id);
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveUp();
  }
}

void Network_Manager::moveDown() {
  if (getState() == Network_State::SELECTING_AP) {
    displayAP(selected_id);
    selected_id = (selected_id == min((uint16_t) ((tft->height() - AP_LABEL_HEIGHT) / AP_ITEM_HEIGHT), network_count) - 1) ? 0 : selected_id + 1;
    displayAPSelected(selected_id);
  } else if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveDown();
  }
}

void Network_Manager::moveLeft() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveLeft();
  }
}

void Network_Manager::moveRight() {
  if (getState() == Network_State::ENTERING_PASSWORD) {
    keyboard->moveRight();
  }
}

void Network_Manager::intInput(int input) {
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

void Network_Manager::loadAndConnect() {
  char ssid[SSID_LENGTH];
  char password[SSID_LENGTH];

  if (loadSSID(ssid) && loadPassword(password)) {
    tft->fillScreen(BLACK);
    tft->setTextSize(1);
    tft->setTextColor(WHITE);
    tft->setCursor(0, 10);
    tft->println(" SSID: ");
    tft->setTextColor(LIGHT_RED);
    tft->print(" ");
    tft->println(ssid);
    tft->setTextColor(WHITE);
    tft->println("\n Password: ");
    tft->setTextColor(LIGHT_RED);
    tft->print(" ");
    tft->println(password);
    tft->setTextColor(WHITE);
    WiFi.begin(ssid, password);
    tft->print("\n ");

    int i = 0;
    int maxIterations = 60;
    int segmentWidth = (tft->width() - 10) / maxIterations;

    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        i++;

        // Calculate the x position of the rectangle
        int xPos = 5 + (i - 1) * segmentWidth;

        // Determine the color: WHITE for the first 20 iterations, RED afterward
        int color = (i < 20) ? WHITE : RED;

        // Draw the rectangle on the screen
        tft->fillRect(xPos, 58, segmentWidth, 8, color);

        // If the maximum number of iterations is reached, reset and scan for networks
        if (i == maxIterations) {
            keyboard->reset();
            tft->fillScreen(BLACK);
            scanForNetworks();
            return;
        }
    }

    state = Network_State::CONNECTED;

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    tft->print("\n ");
    tft->setTextColor(LIGHT_GREEN);
    tft->println("\n Connected");
    tft->setTextColor(WHITE);
    tft->println("\n IP address: ");
    tft->print(" ");
    tft->println(WiFi.localIP());
    delay(5000);
    // tft->fillScreen(BLACK);
  } else {
    scanForNetworks();
  }
}

void Network_Manager::reconnect() {
  char ssid[SSID_LENGTH];
  char password[SSID_LENGTH];

  if (loadSSID(ssid) && loadPassword(password)) {
    WiFi.begin(ssid, password);

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      i++;

      if (i == 30) {
        return;
      }
    }

    state = Network_State::CONNECTED;

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  }
}

void Network_Manager::clearCredentials() {
  for (size_t i = 0; i < SSID_LENGTH * 2; ++i) {
    EEPROM.write(CREDENTIALS_ADDRESS + i, 0);
  }
  EEPROM.commit();
}

void printHeapUsage() {
  // Calculate and print the percentage of free heap
  // float percentageFree = (float)freeHeap / ESP.getHeapSize() * 100.0;

  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");

  Serial.print("Fragmentation: ");
  Serial.print(ESP.getHeapFragmentation());
  Serial.println("%");
}


/**
 * Constructs the full URL for data retrieval.
 */
void Network_Manager::constructURL(int* refresh_ids, int count, const char* currency, char* url_buffer) {
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

void Network_Manager::refreshData(int* coin_indexes, int count, const char* currency) {
  // Instanciate Secure HTTP communication
  char request_url[250] = { 0 };
  http.useHTTP10(true);
  constructURL(coin_indexes, count, currency, request_url);  // Get the URL

  // Init client
  client.setInsecure();

  // HTTP request
  http.begin(client, request_url);
  int httpCode = http.GET();

  // Buffers for data
  StaticJsonDocument<100> doc;
  char id_buffer[35];

  char currency_change_string[15];
  std::strncpy(currency_change_string, currency, 3);
  std::strcpy(currency_change_string + 3, "_24h_change");

  // OK http response
  if (httpCode == 200) {
    int read_bytes;

    // Parse incoming JSON from stream
    http.getStream().find("\"");
    do {
      read_bytes = http.getStream().readBytesUntil('"', id_buffer, 32);
      id_buffer[read_bytes] = 0;

      http.getStream().find(':');
      deserializeJson(doc, http.getStream());

      // Add to coin or portfolio
      for (int i = 0; i < count; i++) {
        if (strcmp(id_buffer, coins[coin_indexes[i]]->coin_id) == 0
            && doc[currency] > 0) {
          coins[coin_indexes[i]]->current_price = doc[currency];
          coins[coin_indexes[i]]->current_change = doc[currency_change_string];
          coins[coin_indexes[i]]->candles->addPrice(doc[currency]);
          break;
        }
      }
    } while (http.getStream().findUntil(",\"", "}"));  // Continue until last '}' reached

    http.end();
  } else if (httpCode == -1) {
    http.end();
    state = Network_State::DISCONNECTED;
    reconnect();
  } else {
    http.end();
  }
}