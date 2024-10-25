#include "Page.h"

Page::Page(const char *label) : label(label), Element(PAGE) {}
Page::Page() : Element(PAGE) {}

// Displays the button when its not hovered over/selected
void Page::display()
{
    int buttonX = MENU_SEP;
    int buttonY = getTop() + getDisplayOffset();
    int buttonWidth = getWidth() - 2 * MENU_SEP;
    int buttonHeight = PAGE_HEIGHT;

    // Display background
    getScreen()->fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, 2, NOT_SELECTED_COLOUR);

    // Display triangle
    int triangleX = buttonX + TRIANGLE_X_OFFSET;
    int triangleY = getTop() + TRIANGLE_Y_OFFSET;
    int triangleWidth = TRIANGLE_WIDTH;
    int triangleHeight = TRIANGLE_HEIGHT;

    getScreen()->fillTriangle(triangleX, triangleY, triangleX + triangleWidth, triangleY + triangleHeight / 2, triangleX, triangleY + triangleHeight, SELECTED_TEXT_COLOUR);

    // Display label
    int labelX = buttonX + LABEL_X_OFFSET;
    int labelY = buttonY + (buttonHeight - 8) / 2;

    getScreen()->setCursor(labelX, labelY);
    getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
    getScreen()->print(label);
}

// Displays the button when it is hovered over/selected
void Page::displaySelected()
{
    int buttonX = MENU_SEP;
    int buttonY = getTop() + getDisplayOffset();
    int buttonWidth = getWidth() - 2 * MENU_SEP;
    int buttonHeight = PAGE_HEIGHT;

    // Display background
    getScreen()->fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, 2, SELECTED_COLOUR);

    // Display triangle
    int triangleX = buttonX + TRIANGLE_X_OFFSET;
    int triangleY = getTop() + TRIANGLE_Y_OFFSET;
    int triangleWidth = TRIANGLE_WIDTH;
    int triangleHeight = TRIANGLE_HEIGHT;

    getScreen()->fillTriangle(triangleX, triangleY, triangleX + triangleWidth, triangleY + triangleHeight / 2, triangleX, triangleY + triangleHeight, NOT_SELECTED_TEXT_COLOUR);

    // Display label
    int labelX = buttonX + LABEL_X_OFFSET;
    int labelY = buttonY + (buttonHeight - 8) / 2;

    getScreen()->setCursor(labelX, labelY);
    getScreen()->setTextColor(NOT_SELECTED_TEXT_COLOUR);
    getScreen()->print(label);
}

// Presses the button, and triggers action function
void Page::press()
{
    if (!isEntered())
    {
        entered = true;
        pageDisplay();
    }
    else
    {
        pagePress();
    }
}

// Moves left within the button (might add a help button within)
void Page::moveLeft()
{
    if (isEntered())
    {
        pageMoveLeft();
    }
}

// Moves right within the button (might add a help button within)
void Page::moveRight()
{
    if (isEntered())
    {
        pageMoveRight();
    }
}

// Gets the height of the button in the menu
int Page::getHeight()
{
    return PAGE_HEIGHT + MENU_SEP;
}

// get byte representation of component
bool Page::serialize(uint8_t *buffer, int *index, uint16_t length_limit)
{
    return pageSerialize(buffer, index, length_limit);
}

// set values based on byte representation
bool Page::deserialize(uint8_t *buffer, int *index, uint16_t length_limit)
{
    return pageDeserialize(buffer, index, length_limit);
}

// return to default value
void Page::toDefault(){
    pageDefault();
}

void Page::intInput(uint32_t input){
    pageIntInput(input);
}