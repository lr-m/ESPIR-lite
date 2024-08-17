#include "Selector.h"

Selector::Selector(const char *label, const char **option_labels, int *option_values, uint8_t option_count, const char *id, uint8_t starting) : id(id), option_count(option_count), option_labels(option_labels), option_values(option_values), label(label), selectedOptionIndex(starting), Element(SELECTOR), starting(starting) {}

// This displays the selector as if it is not being hovered over/selected
void Selector::display()
{
    getScreen()->setTextColor(DARK_GREY);
    getScreen()->setCursor(MENU_SEP, (getTop() + getDisplayOffset()) + SELECTOR_TITLE_OFFSET);
    getScreen()->print(label);

    drawItems();
}

// This displays the selector as if it is being hovered over/selected
void Selector::displaySelected()
{
    getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
    getScreen()->setCursor(MENU_SEP, (getTop() + getDisplayOffset()) + SELECTOR_TITLE_OFFSET);
    getScreen()->print(label);

    drawItems();
}

// Draws all the selector options
void Selector::drawItems()
{
    uint16_t total_width = 0;
    uint16_t current_element_in_row = 0;
    uint16_t curr_row_count = 0;

    // Display each option label horizontally with a red box
    for (int i = 0; i < option_count; ++i)
    {
        int16_t x, y;
        uint16_t width, height;

        getScreen()->getTextBounds(option_labels[i], MENU_SEP, (getTop() + getDisplayOffset()) + SELECTOR_TITLE_HEIGHT + SELECTOR_TITLE_OFFSET * (i + 1), &x, &y, &width, &height);
        if (MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING + width + SELECTOR_OPTION_BG_PADDING > getWidth())
        {
            total_width = 0;
            curr_row_count++;
            current_element_in_row = 0;
        }

        // display selected option
        if (i != selectedOptionIndex)
        {
            getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
            getScreen()->fillRoundRect(MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING, SELECTOR_OPTION_BG_OFFSET + (getTop() + getDisplayOffset()) + curr_row_count * SELECTOR_ROW_HEIGHT, width + SELECTOR_OPTION_BG_PADDING, SELECTOR_OPTION_BG_HEIGHT, 2, NOT_SELECTED_COLOUR);
        }
        else
        {
            getScreen()->setTextColor(NOT_SELECTED_TEXT_COLOUR);
            getScreen()->fillRoundRect(MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING, SELECTOR_OPTION_BG_OFFSET + (getTop() + getDisplayOffset()) + curr_row_count * SELECTOR_ROW_HEIGHT, width + SELECTOR_OPTION_BG_PADDING, SELECTOR_OPTION_BG_HEIGHT, 2, SELECTED_COLOUR);
        }

        getScreen()->setCursor(MENU_SEP + total_width + SELECTOR_OPTION_BG_PADDING / 2 + current_element_in_row * SELECTOR_SPACING, SELECTOR_OPTION_TEXT_OFFSET + (getTop() + getDisplayOffset()) + curr_row_count * SELECTOR_ROW_HEIGHT); // Adjust the position as needed
        getScreen()->print(option_labels[i]);
        total_width += width;
        current_element_in_row++;
    }
}

// Draws only single option on screen (determined by passed index)
void Selector::drawSingleItem(int index)
{
    uint16_t total_width = 0;
    uint16_t current_element_in_row = 0;
    uint16_t current_row_count = 0;

    // Display each option label horizontally with a red box
    for (int i = 0; i < option_count; ++i)
    {
        int16_t x, y;
        uint16_t width, height;

        getScreen()->getTextBounds(option_labels[i], MENU_SEP, (getTop() + getDisplayOffset()) + SELECTOR_TITLE_HEIGHT + SELECTOR_TITLE_OFFSET * (i + 1), &x, &y, &width, &height);
        if (MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING + width + SELECTOR_OPTION_BG_PADDING > getWidth())
        {
            total_width = 0;
            current_row_count++;
            current_element_in_row = 0;
        }

        if (i == index)
        {
            if (i != selectedOptionIndex)
            {
                getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
                getScreen()->fillRoundRect(MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING, SELECTOR_OPTION_BG_OFFSET + (getTop() + getDisplayOffset()) + current_row_count * SELECTOR_ROW_HEIGHT, width + SELECTOR_OPTION_BG_PADDING, SELECTOR_OPTION_BG_HEIGHT, 2, NOT_SELECTED_COLOUR);
            }
            else
            {
                getScreen()->setTextColor(NOT_SELECTED_TEXT_COLOUR);
                getScreen()->fillRoundRect(MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING, SELECTOR_OPTION_BG_OFFSET + (getTop() + getDisplayOffset()) + current_row_count * SELECTOR_ROW_HEIGHT, width + SELECTOR_OPTION_BG_PADDING, SELECTOR_OPTION_BG_HEIGHT, 2, SELECTED_COLOUR);
            }

            getScreen()->setCursor(MENU_SEP + total_width + SELECTOR_OPTION_BG_PADDING / 2 + current_element_in_row * SELECTOR_SPACING, SELECTOR_OPTION_TEXT_OFFSET + (getTop() + getDisplayOffset()) + current_row_count * SELECTOR_ROW_HEIGHT); // Adjust the position as needed
            getScreen()->print(option_labels[i]);
            break;
        }
        total_width += width;
        current_element_in_row++;
    }
}

// Moves the seleted item to be the item on the left of the current (or wrap around)
void Selector::moveLeft()
{
    int last = selectedOptionIndex;
    if (option_count > 0)
    {
        if (selectedOptionIndex == 0)
        {
            selectedOptionIndex = option_count - 1; // Wrap around to the bottom
        }
        else
        {
            --selectedOptionIndex;
        }
    }
    drawSingleItem(last);
    drawSingleItem(selectedOptionIndex);
}

// Moves the seleted item to be the item on the right of the current (or wrap around)
void Selector::moveRight()
{
    int last = selectedOptionIndex;
    if (option_count > 0)
    {
        if (selectedOptionIndex == option_count - 1)
        {
            selectedOptionIndex = 0; // Wrap around to the top
        }
        else
        {
            ++selectedOptionIndex;
        }
    }
    drawSingleItem(last);
    drawSingleItem(selectedOptionIndex);
}

// Returns the height of this object in the menu
int Selector::getHeight()
{
    uint16_t total_width = 0;
    uint16_t current_element_in_row = 0;
    uint16_t curr_row_count = 0;

    // Display each option label horizontally with a red box
    for (int i = 0; i < option_count; ++i)
    {
        int16_t x, y;
        uint16_t width, height;

        getScreen()->getTextBounds(option_labels[i], MENU_SEP, (getTop() + getDisplayOffset()) + SELECTOR_TITLE_HEIGHT + SELECTOR_TITLE_OFFSET * (i + 1), &x, &y, &width, &height);
        if (MENU_SEP + total_width + current_element_in_row * SELECTOR_SPACING + width + SELECTOR_OPTION_BG_PADDING > getWidth())
        {
            total_width = 0;
            curr_row_count++;
            current_element_in_row = 0;
        }
        total_width += width;
        current_element_in_row++;
    }

    return SELECTOR_HEIGHT + MENU_SEP + SELECTOR_ROW_HEIGHT * curr_row_count;
}

// Gets the selected value of the selector
int Selector::getValue()
{
    return option_values[selectedOptionIndex];
}

// get byte representation of component
bool Selector::serialize(uint8_t *buffer, int *byte_index, uint16_t length_limit)
{
    if (*byte_index < 0)
    {
        return false;
    }

    // check we arent writing outside buffer
    if ((*byte_index) + 1 >= length_limit){
        return false;
    }

    // store the selected index (uint8_t)
    buffer[*byte_index] = getSelectedIndex();

    // Update byte index for the next operation
    (*byte_index)++;

    return true;
}

// load value from byte representation
bool Selector::deserialize(uint8_t *buffer, int *byte_index, uint16_t length_limit)
{
    if (*byte_index < 0)
    {
        return false;
    }

    // check we arent reading outside buffer
    if ((*byte_index) + 1 >= length_limit){
        return false;
    }

    // store the selected index (uint8_t)
    setSelectedIndex(buffer[*byte_index]);

    // Update byte index for the next operation
    (*byte_index)++;

    return true;
}