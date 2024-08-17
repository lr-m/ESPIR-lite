#include "Button.h"

Button::Button(const char *label, std::function<void()> onClick) : label(label), onClick(onClick), Element(BUTTON) {}

// Displays the button when its not hovered over/selected
void Button::display() {
    int buttonX = MENU_SEP;
    int buttonY = getTop() + getDisplayOffset();
    int buttonWidth = getWidth() - 2 * MENU_SEP;
    int buttonHeight = BUTTON_HEIGHT;

    // Display background
    getScreen()->fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, 2, NOT_SELECTED_COLOUR);

    // Display triangle
    int triangleX = buttonX + TRIANGLE_X_OFFSET;
    int triangleY = getTop() + TRIANGLE_Y_OFFSET;
    int triangleWidth = TRIANGLE_WIDTH;
    int triangleHeight = TRIANGLE_HEIGHT;

    getScreen()->drawTriangle(triangleX, triangleY, triangleX + triangleWidth, triangleY + triangleHeight / 2, triangleX, triangleY + triangleHeight, SELECTED_TEXT_COLOUR);

    // Display label
    int labelX = buttonX + LABEL_X_OFFSET;
    int labelY = buttonY + (buttonHeight - 8) / 2;

    getScreen()->setCursor(labelX, labelY);
    getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
    getScreen()->print(label);
}

// Displays the button when it is hovered over/selected
void Button::displaySelected() {
    int buttonX = MENU_SEP;
    int buttonY = getTop() + getDisplayOffset();
    int buttonWidth = getWidth() - 2 * MENU_SEP;
    int buttonHeight = BUTTON_HEIGHT;

    // Display background
    getScreen()->fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight, 2, SELECTED_COLOUR);

    // Display triangle
    int triangleX = buttonX + TRIANGLE_X_OFFSET;
    int triangleY = getTop() + TRIANGLE_Y_OFFSET;
    int triangleWidth = TRIANGLE_WIDTH;
    int triangleHeight = TRIANGLE_HEIGHT;

    getScreen()->drawTriangle(triangleX, triangleY, triangleX + triangleWidth, triangleY + triangleHeight / 2, triangleX, triangleY + triangleHeight, NOT_SELECTED_TEXT_COLOUR);

    // Display label
    int labelX = buttonX + LABEL_X_OFFSET;
    int labelY = buttonY + (buttonHeight - 8) / 2;

    getScreen()->setCursor(labelX, labelY);
    getScreen()->setTextColor(NOT_SELECTED_TEXT_COLOUR);
    getScreen()->print(label);
}


// Presses the button, and triggers action function
void Button::press()
{
    if (onClick != nullptr)
    {
        displaySelected();
        onClick();
    }
}

// Gets the height of the button in the menu
int Button::getHeight()
{
    return BUTTON_HEIGHT + MENU_SEP;
}