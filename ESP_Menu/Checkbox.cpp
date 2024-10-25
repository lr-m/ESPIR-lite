#include "Checkbox.h"

Checkbox::Checkbox(const char *label, const char *id, bool starting) : label(label), id(id), Element(CHECKBOX), value(starting), starting(starting) {}

// Displays the checkbox when its not hovered over/selected
void Checkbox::display()
{
    getScreen()->setCursor(MENU_SEP, (getTop() + getDisplayOffset()));
    getScreen()->setTextColor(DARK_GREY);
    getScreen()->print(label);

    drawCheckbox();
}

// Displays the checkbox when it is hovered over/selected
void Checkbox::displaySelected()
{
    getScreen()->setCursor(MENU_SEP, (getTop() + getDisplayOffset()));
    getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
    getScreen()->print(label);

    drawCheckbox();
}

// Draws the red/green checkbox
void Checkbox::drawCheckbox()
{
    if (value)
    {
        getScreen()->fillRoundRect(getWidth() - MENU_SEP - CHECKBOX_SIZE, (getTop() + getDisplayOffset()), CHECKBOX_SIZE, CHECKBOX_SIZE, 2, GREEN);
    }
    else
    {
        getScreen()->fillRoundRect(getWidth() - MENU_SEP - CHECKBOX_SIZE, (getTop() + getDisplayOffset()), CHECKBOX_SIZE, CHECKBOX_SIZE, 2, RED);
    }
}

// Presses the checkbox, and triggers action function
void Checkbox::press()
{
    value = !value;
    drawCheckbox();
}

// serialize checkbox
bool Checkbox::serialize(uint8_t* buffer, int* byte_index, uint16_t length_limit)
{
    // Check bounds before any writing
    if (*byte_index < 0 || *byte_index + sizeof(uint8_t) > length_limit) {
        return false;
    }

    // Now safe to write the value
    buffer[*byte_index] = getValue() ? 1 : 0;
    (*byte_index)++;
    
    return true;
}

// deserialize checkbox
bool Checkbox::deserialize(uint8_t *buffer, int *byte_index, uint16_t length_limit)
{
    if (*byte_index < 0)
    {
        return false;
    }

    // check we are reading inside buffer
    if ((*byte_index) + 1 >= length_limit)
    {
        return false;
    }

    if (buffer[*byte_index] == 0)
    {
        setValue(false);
    }
    else
    {
        setValue(true);
    }

    // Update byte index for the next operation
    (*byte_index)++;

    return true;
}