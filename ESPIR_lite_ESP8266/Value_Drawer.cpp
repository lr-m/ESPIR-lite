#include "Value_Drawer.h"

#define max(a, b) \
  ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })

#define min(a, b) \
  ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })

Value_Drawer::Value_Drawer(Adafruit_GFX* tft) : tft(tft) {
}

void Value_Drawer::drawPrice(double available_space, double price, double max_precision, int size, int currency) {
  tft->setTextSize(size);
  tft->setTextColor(WHITE);

  if (currency == 0) {  // GBP
    tft->print(char(156));
  } else if (currency == 1) {  // USD
    tft->print(char(36));
  } else if (currency == 2) {  // EUR
    tft->print(char(237));
  }

  // Calculate the minimum and maximum values that can fit in the space
  double max = pow(10, available_space) - 1;
  double min = (pow(0.1, available_space - 2) - 5 * pow(0.1, available_space)) * 10;  // * 10 for precision of at least 2

  // Represent number normally, but limit occupied space
  if (price == 0) {
    tft->print("0.");

    for (int i = 0; i < available_space - 2; i++) {
      tft->print('0');
    }
  } else if (price < min) {
    double mul = 10;
    int e_val = -1;

    while (price * mul < pow(10, available_space - 4)) {
      mul *= 10;
      e_val--;
    }

    if ((int)round(price * mul) == (int)pow(10, available_space - 3)) {
      mul /= 10;
      e_val++;
    }

    tft->print((int)round(price * mul));
    tft->print('e');
    tft->print(e_val);
  } else if (price >= max) {
    double div = 10;
    int e_val = 1;

    while (price / div >= pow(10, available_space - 2)) {
      div *= 10;
      e_val++;
    }

    if ((int)round(price / div) == (int)pow(10, available_space - 2)) {
      div *= 10;
      e_val++;
    }

    tft->print((int)round(price / div));
    tft->print('e');
    tft->print(e_val);
  } else {
    int precision;

    if (floor(price) > 0) {
      int integer_digits = (int)log10(floor(price)) + 1;
      precision = min(max_precision, available_space - integer_digits - 1);
    } else {
      precision = min(max_precision, available_space - 2);
    }

    precision = max(0, precision);  // Ensure no negative precision

    // Account for rounding increasing the size of the value
    if (floor(price) > 0) {
      double test = round(price * pow(10, precision)) / pow(10, precision);

      // If more digits after rounding by desired precision, call again on rounded
      if ((int)log10(floor(test)) + 1 > (int)log10(floor(price)) + 1) {
        drawPrice(available_space, test, max_precision, size, currency);
        return;
      }
    }

    tft->print(price, precision);
  }

  tft->setTextColor(WHITE);
}

void Value_Drawer::drawSign(double value) {
  if (value < 0) {
    tft->setTextColor(LIGHT_RED);
    tft->print('-');
  } else {
    tft->setTextColor(LIGHT_GREEN);
    tft->print('+');
  }
}

// Draws the percentage change on the screen.
void Value_Drawer::drawPercentageChange(double available_space,
                                        double value, double max_precision, int size) {
  tft->setTextSize(size);

  int sign;

  double change_val;
  if (value < 0) {
    sign = -1;
    change_val = -value;
  } else {
    sign = 1;
    change_val = value;
  }

  // Calculate the minimum and maximum values that can fit in the space
  double max = pow(10, available_space) - 1;
  double min = pow(0.1, available_space - 2) - 5 * pow(0.1, available_space);

  // Represent number normally, but limit occupied space
  if (change_val == 0) {
    drawSign(value);

    tft->print("0.");

    for (int i = 0; i < available_space - 2; i++) {
      tft->print('0');
    }

    tft->print('%');
  } else if (change_val < min) {
    double mul = 10;
    int e_val = -1;

    while (change_val * mul < pow(10, available_space - 4)) {
      mul *= 10;
      e_val--;
    }

    if ((int)round(change_val * mul) == (int)pow(10, available_space - 3)) {
      mul /= 10;
      e_val++;
    }

    drawSign(value);
    tft->print((int)round(change_val * mul));
    tft->print('e');
    tft->print(e_val);
    tft->print('%');
  } else if (change_val >= max) {
    double div = 10;
    int e_val = 1;

    while (change_val / div >= pow(10, available_space - 2)) {
      div *= 10;
      e_val++;
    }

    if ((int)round(change_val / div) == (int)pow(10, available_space - 2)) {
      div *= 10;
      e_val++;
    }

    drawSign(value);
    tft->print((int)round(change_val / div));
    tft->print('e');
    tft->print(e_val);
    tft->print('%');
  } else {
    int precision;

    if (floor(change_val) > 0) {
      int integer_digits = (int)log10(floor(change_val)) + 1;

      precision = min(max_precision, available_space - integer_digits - 1);
    } else {
      precision = min(max_precision, available_space - 2);  // For the 0.
    }

    precision = max(0, precision);

    // Account for rounding
    if (floor(change_val) > 0) {
      double test = round(change_val * pow(10, precision)) / pow(10, precision);

      // If more digits after rounding by desired precision, call again on rounded
      if ((int)log10(floor(test)) + 1 > (int)log10(floor(change_val)) + 1) {
        drawPercentageChange(available_space, sign * test, max_precision, size);
        return;
      }
    }

    drawSign(value);

    tft->print(change_val, precision);

    tft->print('%');
  }

  tft->setTextColor(WHITE);
}