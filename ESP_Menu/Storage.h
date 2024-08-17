#include <EEPROM.h>

#define STORAGE_SIZE 512

#ifndef Storage_h
#define Storage_h

class Storage {
    public:
        Storage();
        void save(uint8_t*, uint16_t);
        void load(uint8_t*, uint16_t);
        void reset();
        void write(int startAddress, const uint8_t* data, size_t length);
        void read(int startAddress, uint8_t* data, size_t length);
};

#endif