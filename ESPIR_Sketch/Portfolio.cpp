/*
  Portfolio.cpp - Portfolio interface for ST7735
  Copyright (c) 2022 Luke Mills.  All right reserved.
*/

#include "Portfolio.h"

// Constructor for Portfolio Editor
Portfolio::Portfolio(Adafruit_ST7735 *display,
                     COIN **coin_arr, Value_Drawer *drawer)
  : value_drawer(drawer), tft(display), coins(coin_arr) {
  candle_chart = new Candle_Chart(tft, CANDLE_COUNT, 36, tft->height() - 12, 1);
}

// Draw the current value of the portfolio
void Portfolio::drawValue(double *total_value, int currency) {
  tft->setTextColor(WHITE);
  tft->setCursor(3, 4);

  if (tft->width() == 128){
    value_drawer->drawPrice(9, *total_value, 2, 2, currency);
  } else {
    value_drawer->drawPrice(12, *total_value, 2, 2, currency);
  }
}

// Moves to the next display mode
void Portfolio::nextMode() {
  int nextModeValue = static_cast<int>(display_mode) + 1;

  if (nextModeValue == static_cast<int>(MODE_COUNT)) {
    display_mode = static_cast<DisplayMode>(0);
  } else {
    display_mode = static_cast<DisplayMode>(nextModeValue);
  }
}

// Moves to the previous display mode
void Portfolio::previousMode() {
  int prevModeValue = static_cast<int>(display_mode) - 1;

  if (prevModeValue < 0) {
    display_mode = static_cast<DisplayMode>(MODE_COUNT - 1);
  } else {
    display_mode = static_cast<DisplayMode>(prevModeValue);
  }
}

// Draws the candle chart
void Portfolio::drawCandleChart(double *total_value, int currency) {
  candle_chart->display(currency);
}

void Portfolio::drawPropBar(double *total_value) {
  double coin_total = 0;
  double current_total = 0;

  for (int i = 0; i < COIN_COUNT; i++) {
    if (sorted_coins[i]->amount > 0) {
      coin_total =
        sorted_coins[i]->current_price * sorted_coins[i]->amount;

      tft->fillRect((current_total * (tft->width() / *total_value)), 21,
                    1 + (coin_total * (tft->width() / *total_value)), 3,
                    sorted_coins[i]->portfolio_colour);

      current_total += coin_total;
    }
  }
}

// gets the number of elements that can be displayed on the screen in summary + pie chart
int Portfolio::getElementCount() {
  int element_count = 0;
  for (int i = 0; i < COIN_COUNT; i++) {
    if (sorted_coins[i]->amount > 0) {
      element_count++;
    }
  }
  return element_count;
}

// Moves the candles to the next time period
void Portfolio::nextTimePeriod() {
  candle_chart->nextTimePeriod(getFloatValue());
}

// Adds the current price of portfolio to the candles
void Portfolio::addPriceToCandles() {
  float val = getFloatValue();

  candle_chart->addPrice(val);
}

// Clears the candles
void Portfolio::clearCandles() {
  candle_chart->reset();
}


// Display the coin breakdown screen with bar proportional bar chart
void Portfolio::drawBarSummary(double *total_value, int currency) {
  drawPropBar(total_value);

  // Draw coin code, cost of owned, and % of portfolio
  int drawing_count = 0;
  double coin_total = 0;

  for (int i = page_base; i < page_base + COIN_PAGE_LIMIT; i++) {
    if (i >= COIN_COUNT) {
      break;
    }
    if (sorted_coins[i]->amount > 0) {
      coin_total =
        sorted_coins[i]->current_price * sorted_coins[i]->amount;

      // Set text properties
      tft->setTextSize(TEXT_SIZE);
      tft->setCursor(TEXT_X_OFFSET, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
      
      // Draw color indicator rectangle
      tft->fillRect(COLOR_RECT_X, 
                    TEXT_Y_OFFSET - 1 + drawing_count * TEXT_Y_SPACING, 
                    COLOR_RECT_WIDTH, 
                    TEXT_Y_SPACING, 
                    sorted_coins[i]->portfolio_colour);
        
      // Print coin code and percentage
      tft->print(sorted_coins[i]->coin_code);
      tft->setTextColor(WHITE);

      // draw value
      if (tft->width() == 128){
        tft->setCursor(VALUE_TEXT_X_OFFSET_128, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
        value_drawer->drawPrice(7, coin_total, 2, 1, currency);
      } else {
        tft->setCursor(VALUE_TEXT_X_OFFSET_160, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
        value_drawer->drawPrice(10, coin_total, 2, 1, currency);
      }

      // draw percentage change
      if (tft->width() == 128){
        tft->setCursor(PERFORMANCE_TEXT_X_OFFSET_128, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
      } else {
        tft->setCursor(PERFORMANCE_TEXT_X_OFFSET_160, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
      }
      value_drawer->drawPercentageChange(4, sorted_coins[i]->current_change, 2, 1);

      drawing_count++;
    }
  }
}

void Portfolio::drawPieSummary(double *total_value) {
  double coin_total = 0;
  double current_total = 0;
  int drawing_count = 0;

  for (int i = page_base; i < page_base + COIN_PAGE_LIMIT; i++) {
    if (i >= COIN_COUNT) {
      break;
    }

    if (sorted_coins[i]->amount > 0) {
      coin_total = sorted_coins[i]->current_price * sorted_coins[i]->amount;
      
      // Set text properties
      tft->setTextSize(TEXT_SIZE);
      tft->setCursor(TEXT_X_OFFSET, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
      
      // Draw color indicator rectangle
      tft->fillRect(COLOR_RECT_X, 
                    TEXT_Y_OFFSET - 1 + drawing_count * TEXT_Y_SPACING, 
                    COLOR_RECT_WIDTH, 
                    TEXT_Y_SPACING, 
                    sorted_coins[i]->portfolio_colour);

      // Print coin code and percentage
      tft->print(sorted_coins[i]->coin_code);
      tft->setCursor(PERCENTAGE_TEXT_X_OFFSET, TEXT_Y_OFFSET + drawing_count * TEXT_Y_SPACING);
      tft->setTextColor(WHITE);
      tft->print(coin_total / (*total_value) * 100, 1);
      tft->print('%');

      drawing_count++;
    }
  }

  // Draw pie chart proportional to the amount of coin owned
  double startAngle = 0;
  for (int i = 0; i < COIN_COUNT; i++) {
    if (sorted_coins[i]->amount > 0) {
      double coin_total = sorted_coins[i]->current_price * sorted_coins[i]->amount;
      double endAngle = startAngle + (coin_total / *total_value) * 360.0;

      if (tft->width() == 128){
        fillSegment(tft->width() / 2 + PIE_CHART_CENTER_X_OFFSET_128, 
                  tft->height() / 2 + PIE_CHART_CENTER_Y_OFFSET_128,
                  startAngle, endAngle, 
                  PIE_CHART_RADIUS_OUTER_128, PIE_CHART_RADIUS_INNER_128,
                  sorted_coins[i]->portfolio_colour);
      } else  {
        fillSegment(tft->width() / 2 + PIE_CHART_CENTER_X_OFFSET_160, 
                  tft->height() / 2 + PIE_CHART_CENTER_Y_OFFSET_160,
                  startAngle, endAngle, 
                  PIE_CHART_RADIUS_OUTER_160, PIE_CHART_RADIUS_INNER_160,
                  sorted_coins[i]->portfolio_colour);
      }
      

      startAngle = endAngle;
    }
  }
}

void Portfolio::press(int currency) {
  // dont go to next page if there isnt one
  if (getElementCount() <= COIN_PAGE_LIMIT) {
    return;
  }

  // No pages in candle mode
  if (display_mode == DisplayMode::CANDLE) {
    return;
  }

  // go to the next page and display
  if (page_base + COIN_PAGE_LIMIT >= getElementCount()) {
    page_base = 0;
  } else {
    page_base += COIN_PAGE_LIMIT;
  }
  display(currency);
}

// Display the portfolio
void Portfolio::display(int currency) {
  tft->fillRect(0, 0, tft->width(), tft->height(), BLACK);

  // If in portfolio mode, draw the portfolio upon update
  double total_value = 0;
  getTotalValue(&total_value);
  drawValue(&total_value, currency);

  // Check that there are coins with non-zero values owned
  int i = 0;
  double coin_price_owned[COIN_COUNT];

  for (int i = 0; i < COIN_COUNT; i++) {
    coin_price_owned[i] = 0;
  }

  // Store cost of each owned coin
  for (int i = 0; i < COIN_COUNT; i++) {
    coin_price_owned[i] =
      coins[i]->current_price * coins[i]->amount;
    sorted_coins[i] = coins[i];
  }

  // Perform bubble sort on sorted_coins based on the amount values
  for (int i = 0; i < COIN_COUNT - 1; i++) {
    for (int j = 0; j < COIN_COUNT - i - 1; j++) {
      if ((sorted_coins[j]->amount * sorted_coins[j]->current_price) < (sorted_coins[j + 1]->amount * sorted_coins[j + 1]->current_price)) {
        // Swap sorted_coins[j] and sorted_coins[j + 1]
        COIN *temp = sorted_coins[j];
        sorted_coins[j] = sorted_coins[j + 1];
        sorted_coins[j + 1] = temp;
      }
    }
  }

  // user could have removed elements while on 2nd page, check for this
  if (getElementCount() <= COIN_PAGE_LIMIT) {
    page_base = 0;
  }

  // Display current display mode
  if (display_mode == DisplayMode::BAR) {
    drawBarSummary(&total_value, currency);
  } else if (display_mode == DisplayMode::PIE) {
    drawPieSummary(&total_value);
  } else if (display_mode == DisplayMode::CANDLE) {
    drawCandleChart(&total_value, currency);
  }
}

// Store the current portfolio as a double in the passed double
void Portfolio::getTotalValue(double *total_value) {
  for (int i = 0; i < COIN_COUNT; i++) {
    if (coins[i]->amount > 0) {
      *total_value +=
        coins[i]->current_price * coins[i]->amount;
    }
  }
}

// Get the current total portfolio value as a float
float Portfolio::getFloatValue() {
  float value = 0;
  for (int i = 0; i < COIN_COUNT; i++) {
    if (coins[i]->amount > 0) {
      value +=
        coins[i]->current_price * coins[i]->amount;
    }
  }
  return value;
}

void Portfolio::fillSegment(int x, int y, double startAngle, double endAngle, int r, int innerR, unsigned int colour) {
    // Ensure the inner radius is smaller than the outer radius
    if (innerR >= r) {
        return; // Invalid case: inner radius should be smaller than outer radius
    }

    // Normalize angles to [0, 360) range
    startAngle = fmod(startAngle, 360.0);
    if (startAngle < 0) startAngle += 360.0;

    endAngle = fmod(endAngle, 360.0);
    if (endAngle < 0) endAngle += 360.0;

    // Handle the case where endAngle is less than startAngle (crossing 0 degrees)
    if (endAngle < startAngle) endAngle += 360.0;

    // Calculate the bounding box
    int minX = std::max(0, x - r);
    int maxX = std::min(tft->width() - 1, x + r);
    int minY = std::max(0, y - r);
    int maxY = std::min(tft->height() - 1, y + r);

    // Draw the segment with a hollow region
    for (int py = minY; py <= maxY; py++) {
        // Variables to keep track of the outer and inner bounds
        int outerLeftX = -1;
        int outerRightX = -1;
        int innerLeftX = -1;
        int innerRightX = -1;

        for (int px = minX; px <= maxX; px++) {
            // Determine if (px, py) is within the outer segment
            if (isInsideSegment(px, py, x, y, startAngle, endAngle, r, 0)) {
                if (outerLeftX == -1) {
                    outerLeftX = px;
                }
                outerRightX = px;
            }

            // Determine if (px, py) is within the inner segment
            if (isInsideSegment(px, py, x, y, startAngle, endAngle, innerR, 0)) {
                if (innerLeftX == -1) {
                    innerLeftX = px;
                }
                innerRightX = px;
            }
        }

        // Draw lines considering the inner radius
        if (outerLeftX != -1 && outerRightX != -1) {
            if (innerLeftX == -1) {
                // No inner circle, draw the full line
                tft->drawLine(outerLeftX, py, outerRightX, py, colour);
            } else {
                // Draw outer parts before and after the inner circle
                if (outerLeftX < innerLeftX) {
                    tft->drawLine(outerLeftX, py, innerLeftX - 1, py, colour);
                }
                if (outerRightX > innerRightX) {
                    tft->drawLine(innerRightX + 1, py, outerRightX, py, colour);
                }
            }
        }
    }
}

// Helper method to check if a point is inside the segment
bool Portfolio::isInsideSegment(int px, int py, int x, int y, double startAngle, double endAngle, int r, int innerR) {
    // Check if the point is within the outer circle radius and outside the inner radius
    int dx = px - x;
    int dy = py - y;
    double distanceSquared = dx * dx + dy * dy;
    double outerRadiusSquared = r * r;
    double innerRadiusSquared = innerR * innerR;

    if (distanceSquared > outerRadiusSquared || distanceSquared < innerRadiusSquared) {
        return false;
    }

    // Calculate the angle of the point relative to the center (x, y)
    double angle = atan2(py - y, px - x) * 180.0 / M_PI;
    if (angle < 0) angle += 360.0;

    // Normalize the angles for comparison
    startAngle = fmod(startAngle, 360.0);
    endAngle = fmod(endAngle, 360.0);

    while (startAngle < 0) startAngle += 360.0;
    while (endAngle < 0) endAngle += 360.0;

    if (startAngle <= endAngle) {
        // Simple case: angle range does not cross 0 degrees
        return (angle >= startAngle && angle <= endAngle);
    } else {
        // Case where the segment crosses 0 degrees
        return (angle >= startAngle || angle <= endAngle);
    }
}

void Portfolio::clear() {
  // reset page base
  page_base = 0;

  // Clear amounts
  for (int i = 0; i < COIN_COUNT; i++) {
    coins[i]->amount = 0;
  }
  // clearCandles();
}
