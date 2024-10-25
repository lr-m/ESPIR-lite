#include "Storage.h"

Storage::Storage(){EEPROM.begin(STORAGE_SIZE);};

// save the passed buffer into memory
void Storage::save(uint8_t* buffer, uint16_t length){
    write(0, buffer, length);
}

// read eeprom into passed buffer
void Storage::load(uint8_t* buffer, uint16_t length){
    read(0, buffer, length);
}

// indicate cleared by removing magic bytes
void Storage::reset(){
    uint8_t magic_killer[4] = {0};
    write(0, magic_killer, 4);
}

// write data of length at specified index
void Storage::write(int startAddress, const uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    EEPROM.write(startAddress + i, data[i]);
  }
  EEPROM.commit();  // Commit the changes to flash
}

// read data into buffer of length at specified index
void Storage::read(int startAddress, uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    data[i] = EEPROM.read(startAddress + i);
  }
}