#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include "Colours.h"

// ensure this library description is only included once
#ifndef Keyboard_h
#define Keyboard_h

#define KEY_HEIGHT 11
#define NORMAL_KEY_WIDTH 11
#define SPECIAL_KEY_WIDTH 10
#define EDGE_BORDER 4

#define MAX_INPUT_LENGTH 31

#define SCHEME_BG 0x0000
#define SCHEME_MAIN 0xCD63  // 0xD800 // (Red) //0x2CB2 (Blue)
#define SCHEME_SECONDARY 0xF6CD
#define SCHEME_TEXT_LIGHT 0xFFFF
#define SCHEME_TEXT_DARK 0x0000

enum class Mode {
  LOWER,
  UPPER,
  NUMBERS,
  SPECIALS,
  BOTTOM
};

class Key {
public:
  Key(Adafruit_ST7735*, int, int, int, int, char*, int);
  void display(Mode);
  void displaySelected(Mode);

  int x;
  int y;
  int w;
  int h;
  char* action;

private:
  Adafruit_ST7735* tft;
  int bottom_key;
};

// library interface description
class Keyboard {
  // user-accessible "public" interface
public:
  Keyboard(Adafruit_ST7735*);
  void press();
  void displayCurrentString();
  void display();
  void displayPrompt(char* prompt);
  void moveRight();
  void moveLeft();
  void moveUp();
  void moveDown();
  void setMode(Mode);
  void setMode(Mode, int);
  void reset();
  void goToTabs();
  void exitTabs();
  bool enterPressed();
  char* getCurrentInput();
  void end();
  void setModeClear(Mode, int);
  void displayInstructions();
  int getCurrentInputLength();
  void setLengthLimit(int);

  // library-accessible "private" interface
private:
  Adafruit_ST7735* tft;
  Key* letters;
  Key* numbers;
  Key* specials;
  Key* bottom_keys;

  Key* selected;
  char* current_string;      // Entered string
  Mode mode;                  // Keyboard mode
  uint16_t selected_index;        // Index of selected key in list
  uint16_t current_input_length;  // Length of current input
  uint16_t enter_pressed;         // Indicate if enter pressed
  Mode last_mode;             // Mode to return to when exiting tabs
  uint16_t last_key;              // Key last selected when entering tabs
  uint16_t length_limit;
};

#endif