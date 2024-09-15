/*
  Portfolio.cpp - Portfolio interface for ST7735
  Copyright (c) 2022 Luke Mills.  All right reserved.
*/

#include "Portfolio.h"

// Constructor for Portfolio Editor
Portfolio::Portfolio(Adafruit_GFX *tft,
                     COIN **coin_arr, Value_Drawer *drawer)
  : value_drawer(drawer), tft(tft), coins(coin_arr) {
    uint8_t candle_offset = 0;
    if (tft->width() == 128){
      candle_offset = CANDLE_OFFSET_Y_128;
      coin_page_limit = PAGE_LIMIT_128;
    } else if (tft->width() == 160){
      candle_offset = CANDLE_OFFSET_Y_160;
      coin_page_limit = PAGE_LIMIT_160;
      if (tft->height() == 80){
        coin_page_limit = PAGE_LIMIT_160_80;
      }
    } else if (tft->width() == 240){
      candle_offset = CANDLE_OFFSET_Y_320;
      coin_page_limit = PAGE_LIMIT_320;
    } else if (tft->width() == 320){
      candle_offset = CANDLE_OFFSET_Y_320;
      coin_page_limit = PAGE_LIMIT_320;
    }
    candle_chart = new Candle_Chart(tft, candle_offset, tft->height() - 12, 1);
}

// Draw the current value of the portfolio
void Portfolio::drawValue(double *total_value, int currency) {
  tft->setTextColor(WHITE);

  if (tft->width() == 128){
    tft->setCursor(3, 4);
    value_drawer->drawPrice(9, *total_value, 2, 2, currency);
  } else if (tft->width() == 160) {
    tft->setCursor(3, 4);
    value_drawer->drawPrice(12, *total_value, 2, 2, currency);
  } else if (tft->width() == 240) {
    tft->setCursor(0, 2);
    value_drawer->drawPrice(9, *total_value, 2, 4, currency);
  }else if (tft->width() == 320) {
    tft->setCursor(3, 2);
    value_drawer->drawPrice(12, *total_value, 2, 4, currency);
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

      if (tft->width() == 128){
        tft->fillRect((current_total * (tft->width() / *total_value)), BAR_Y_128,
                    1 + (coin_total * (tft->width() / *total_value)), BAR_THICKNESS_128,
                    sorted_coins[i]->portfolio_colour);
      } else if (tft->width() == 160){
        tft->fillRect((current_total * (tft->width() / *total_value)), BAR_Y_160,
                    1 + (coin_total * (tft->width() / *total_value)), BAR_THICKNESS_160,
                    sorted_coins[i]->portfolio_colour);
      } else if (tft->width() == 240){
        tft->fillRect((current_total * (tft->width() / *total_value)), BAR_Y_320,
                    1 + (coin_total * (tft->width() / *total_value)), BAR_THICKNESS_320,
                    sorted_coins[i]->portfolio_colour);
      }else if (tft->width() == 320){
        tft->fillRect((current_total * (tft->width() / *total_value)), BAR_Y_320,
                    1 + (coin_total * (tft->width() / *total_value)), BAR_THICKNESS_320,
                    sorted_coins[i]->portfolio_colour);
      }

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

// Display the coin breakdown screen with a proportional bar chart
void Portfolio::drawBarSummary(double *total_value, int currency) {
  drawPropBar(total_value);

  // Draw coin code, cost of owned, and % of portfolio
  int drawing_count = 0;
  double coin_total = 0;

  // Determine the text and rectangle properties based on screen width
  int text_size, text_x_offset, text_y_offset, text_y_spacing, color_rect_x, color_rect_width, value_text_x_offset, performance_text_x_offset, rect_y_offset;

  if (tft->width() == 128) {
    text_size = TEXT_SIZE_128;
    text_x_offset = TEXT_X_OFFSET_128;
    text_y_offset = TEXT_Y_OFFSET_128;
    text_y_spacing = TEXT_Y_SPACING_128;
    color_rect_x = COLOR_RECT_X_128;
    color_rect_width = COLOR_RECT_WIDTH_128;
    value_text_x_offset = VALUE_TEXT_X_OFFSET_128;
    performance_text_x_offset = PERFORMANCE_TEXT_X_OFFSET_128;
    rect_y_offset = -1;
  } else if (tft->width() == 160) {
    text_size = TEXT_SIZE_160;
    text_x_offset = TEXT_X_OFFSET_160;
    text_y_offset = TEXT_Y_OFFSET_160;
    text_y_spacing = TEXT_Y_SPACING_160;
    color_rect_x = COLOR_RECT_X_160;
    color_rect_width = COLOR_RECT_WIDTH_160;
    value_text_x_offset = VALUE_TEXT_X_OFFSET_160;
    performance_text_x_offset = PERFORMANCE_TEXT_X_OFFSET_160;
    rect_y_offset = -1;
  } else if (tft->width() == 240) {
    text_size = TEXT_SIZE_320;
    text_x_offset = TEXT_X_OFFSET_320;
    text_y_offset = TEXT_Y_OFFSET_320;
    text_y_spacing = TEXT_Y_SPACING_320;
    color_rect_x = COLOR_RECT_X_320;
    color_rect_width = COLOR_RECT_WIDTH_320;
    value_text_x_offset = VALUE_TEXT_X_OFFSET_240;
    performance_text_x_offset = PERFORMANCE_TEXT_X_OFFSET_240;
    rect_y_offset = -3;
  } else if (tft->width() == 320) {
    text_size = TEXT_SIZE_320;
    text_x_offset = TEXT_X_OFFSET_320;
    text_y_offset = TEXT_Y_OFFSET_320;
    text_y_spacing = TEXT_Y_SPACING_320;
    color_rect_x = COLOR_RECT_X_320;
    color_rect_width = COLOR_RECT_WIDTH_320;
    value_text_x_offset = VALUE_TEXT_X_OFFSET_320;
    performance_text_x_offset = PERFORMANCE_TEXT_X_OFFSET_320;
    rect_y_offset = -3;
  }

  for (int i = page_base; i < page_base + coin_page_limit; i++) {
    if (i >= COIN_COUNT) {
      break;
    }
    if (sorted_coins[i]->amount > 0) {
      coin_total = sorted_coins[i]->current_price * sorted_coins[i]->amount;

      // Set text properties and cursor position
      tft->setTextSize(text_size);
      tft->setCursor(text_x_offset, text_y_offset + drawing_count * text_y_spacing);
      
      // Draw color indicator rectangle
      tft->fillRect(color_rect_x, text_y_offset + rect_y_offset + drawing_count * text_y_spacing, color_rect_width, text_y_spacing, sorted_coins[i]->portfolio_colour);
      
      // Print coin code and set text color
      tft->print(sorted_coins[i]->coin_code);
      tft->setTextColor(WHITE);

      // Draw value
      tft->setCursor(value_text_x_offset, text_y_offset + drawing_count * text_y_spacing);
      value_drawer->drawPrice(7, coin_total, 2, text_size, currency);

      // Draw percentage change
      tft->setCursor(performance_text_x_offset, text_y_offset + drawing_count * text_y_spacing);
      value_drawer->drawPercentageChange(4, sorted_coins[i]->current_change, 2, text_size);

      drawing_count++;
    }
  }
}

void Portfolio::drawPieSummary(double *total_value) {
  double coin_total = 0;
  int drawing_count = 0;

  // Determine properties based on screen width (same as before)
  int text_size, text_x_offset, text_y_offset, text_y_spacing, color_rect_x, color_rect_width, percentage_text_x_offset, rect_y_offset, pie_chart_center_x, pie_chart_center_y, pie_chart_radius_outer, pie_chart_radius_inner;

  if (tft->width() == 128) {
    text_size = TEXT_SIZE_128;
    text_x_offset = TEXT_X_OFFSET_128;
    text_y_offset = TEXT_Y_OFFSET_128;
    text_y_spacing = TEXT_Y_SPACING_128;
    color_rect_x = COLOR_RECT_X_128;
    color_rect_width = COLOR_RECT_WIDTH_128;
    percentage_text_x_offset = PERCENTAGE_TEXT_X_OFFSET_128;
    rect_y_offset = -1;
    pie_chart_center_x = PIE_CHART_CENTER_X_OFFSET_128;
    pie_chart_center_y = PIE_CHART_CENTER_Y_OFFSET_128;
    pie_chart_radius_outer = PIE_CHART_RADIUS_OUTER_128;
    pie_chart_radius_inner = PIE_CHART_RADIUS_INNER_128;
  } else if (tft->width() == 160) {
    text_size = TEXT_SIZE_160;
    text_x_offset = TEXT_X_OFFSET_160;
    text_y_offset = TEXT_Y_OFFSET_160;
    text_y_spacing = TEXT_Y_SPACING_160;
    color_rect_x = COLOR_RECT_X_160;
    color_rect_width = COLOR_RECT_WIDTH_160;
    percentage_text_x_offset = PERCENTAGE_TEXT_X_OFFSET_160;
    rect_y_offset = -1;

    if (tft->height() == 80){
      pie_chart_center_x = PIE_CHART_CENTER_X_OFFSET_160_80;
      pie_chart_center_y = PIE_CHART_CENTER_Y_OFFSET_160_80;
      pie_chart_radius_outer = PIE_CHART_RADIUS_OUTER_160_80;
      pie_chart_radius_inner = PIE_CHART_RADIUS_INNER_160_80;
    } else {
      pie_chart_center_x = PIE_CHART_CENTER_X_OFFSET_160;
      pie_chart_center_y = PIE_CHART_CENTER_Y_OFFSET_160;
      pie_chart_radius_outer = PIE_CHART_RADIUS_OUTER_160;
      pie_chart_radius_inner = PIE_CHART_RADIUS_INNER_160;
    }
    
  } else if (tft->width() == 240) {
    text_size = TEXT_SIZE_320;
    text_x_offset = TEXT_X_OFFSET_320;
    text_y_offset = TEXT_Y_OFFSET_320;
    text_y_spacing = TEXT_Y_SPACING_320;
    color_rect_x = COLOR_RECT_X_320;
    color_rect_width = COLOR_RECT_WIDTH_320;
    percentage_text_x_offset = PERCENTAGE_TEXT_X_OFFSET_240;
    rect_y_offset = -1;
    pie_chart_center_x = PIE_CHART_CENTER_X_OFFSET_240;
    pie_chart_center_y = PIE_CHART_CENTER_Y_OFFSET_240;
    pie_chart_radius_outer = PIE_CHART_RADIUS_OUTER_240;
    pie_chart_radius_inner = PIE_CHART_RADIUS_INNER_240;
  } else if (tft->width() == 320) {
    text_size = TEXT_SIZE_320;
    text_x_offset = TEXT_X_OFFSET_320;
    text_y_offset = TEXT_Y_OFFSET_320;
    text_y_spacing = TEXT_Y_SPACING_320;
    color_rect_x = COLOR_RECT_X_320;
    color_rect_width = COLOR_RECT_WIDTH_320;
    percentage_text_x_offset = PERCENTAGE_TEXT_X_OFFSET_320;
    rect_y_offset = -1;
    pie_chart_center_x = PIE_CHART_CENTER_X_OFFSET_320;
    pie_chart_center_y = PIE_CHART_CENTER_Y_OFFSET_320;
    pie_chart_radius_outer = PIE_CHART_RADIUS_OUTER_320;
    pie_chart_radius_inner = PIE_CHART_RADIUS_INNER_320;
  }

  // Array to store angle ranges and colors
    double angles[COIN_COUNT][2];
    unsigned int colors[COIN_COUNT];
    int segment_count = 0;
    double startAngle = 0;

    for (int i = 0; i < COIN_COUNT; i++) {
        if (i >= COIN_COUNT) {
            break;
        }

        if (sorted_coins[i]->amount > 0) {
            // only draw if in current range
            if (i + page_base < page_base + coin_page_limit){
              double drawing_coin_total = sorted_coins[i + page_base]->current_price * sorted_coins[i + page_base]->amount;

              // Set text properties and draw coin details
              tft->setTextSize(text_size);
              tft->setCursor(text_x_offset, text_y_offset + drawing_count * text_y_spacing);

              // Draw color indicator rectangle
              tft->fillRect(color_rect_x, text_y_offset + rect_y_offset + drawing_count * text_y_spacing, color_rect_width, text_y_spacing, sorted_coins[i + page_base]->portfolio_colour);

              // Print coin code and percentage
              tft->print(sorted_coins[i + page_base]->coin_code);
              tft->setCursor(percentage_text_x_offset, text_y_offset + drawing_count * text_y_spacing);

              tft->setTextColor(WHITE);
              tft->print(drawing_coin_total / (*total_value) * 100, 1);
              tft->print('%');

              drawing_count++;
            }

            // Calculate the angle range for this segment
            double coin_total = sorted_coins[i]->current_price * sorted_coins[i]->amount;
            double endAngle = startAngle + (coin_total / *total_value) * 360.0;

            // Store the angle range and color for this segment
            angles[segment_count][0] = startAngle;
            angles[segment_count][1] = endAngle;
            colors[segment_count] = sorted_coins[i]->portfolio_colour;
            segment_count++;

            // Move to the next segment
            startAngle = endAngle;
        }
    }

    // Now, scan all pixels in the bounding box of the donut
    pie_chart_center_x += tft->width() / 2;
    pie_chart_center_y += tft->height() / 2;

    int xMin = pie_chart_center_x - pie_chart_radius_outer;
    int xMax = pie_chart_center_x + pie_chart_radius_outer;
    int yMin = pie_chart_center_y - pie_chart_radius_outer;
    int yMax = pie_chart_center_y + pie_chart_radius_outer;

    for (int x = xMin; x <= xMax; x++) {
        for (int y = yMin; y <= yMax; y++) {
            // Compute the distance from the center
            int dx = x - pie_chart_center_x;
            int dy = y - pie_chart_center_y;
            double distSquared = dx * dx + dy * dy;

            // Check if the pixel is within the donut's radius range
            if (distSquared >= pie_chart_radius_inner * pie_chart_radius_inner &&
                distSquared <= pie_chart_radius_outer * pie_chart_radius_outer) {
                
                // Compute the angle of the pixel relative to the center
                double angle = atan2(dy, dx) * 180.0 / M_PI;
                if (angle < 0) angle += 360.0; // Normalize to [0, 360)

                // Find the segment that this angle falls into
                for (int i = 0; i < segment_count; i++) {
                    if (angle >= angles[i][0] && angle <= angles[i][1]) {
                        tft->drawPixel(x, y, colors[i]);
                        break;
                    }
                }
            }
        }
    }
}

void Portfolio::press(int currency) {
  // dont go to next page if there isnt one
  if (getElementCount() <= coin_page_limit) {
    return;
  }

  // No pages in candle mode
  if (display_mode == DisplayMode::CANDLE) {
    return;
  }

  // go to the next page and display
  if (page_base + coin_page_limit >= getElementCount()) {
    page_base = 0;
  } else {
    page_base += coin_page_limit;
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
  if (getElementCount() <= coin_page_limit) {
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

void Portfolio::clear() {
  // reset page base
  page_base = 0;

  // Clear amounts
  for (int i = 0; i < COIN_COUNT; i++) {
    coins[i]->amount = 0;
  }
  // clearCandles();
}
