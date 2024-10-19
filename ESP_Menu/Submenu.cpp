#include "Submenu.h"

Submenu::Submenu(const char *label, Adafruit_GFX *screen) : label(label), entered(false), selectedElementIndex(0), Element(SUBMENU), drawn(false) { setScreen(screen); }

// Displays the submenu option as if it is not selected/hovered over
void Submenu::display()
{
    // Draw the rounded rectangle
    getScreen()->fillRoundRect(MENU_SEP, (getTop() + getDisplayOffset()), getWidth() - 2 * MENU_SEP, SUBMENU_HEIGHT, 2, NOT_SELECTED_COLOUR);

    // Draw three vertical black lines within the rounded rectangle
    for (int i = 0; i < SUBMENU_LINE_COUNT; ++i)
    {
        int lineY = (getTop() + getDisplayOffset()) + SUBMENU_LINE_V_OFFSET + i * SUBMENU_LINE_SEP; // Adjust the starting position of the lines
        getScreen()->drawFastHLine(SUBMENU_LINE_H_OFFSET, lineY, SUBMENU_LINE_H_LENGTH, SELECTED_TEXT_COLOUR);
    }

    // Draw the label
    getScreen()->setCursor(MENU_SEP + SUBMENU_LABEL_X_OFFSET, (getTop() + getDisplayOffset()) + (BUTTON_HEIGHT - 8) / 2); // Adjust the cursor position
    getScreen()->setTextColor(SELECTED_TEXT_COLOUR);
    getScreen()->print(label);
}

// Displays the submenu as if it is selected/hovered over
void Submenu::displaySelected()
{
    // Draw the rounded rectangle
    getScreen()->fillRoundRect(MENU_SEP, (getTop() + getDisplayOffset()), getWidth() - 2 * MENU_SEP, BUTTON_HEIGHT, 2, SELECTED_COLOUR);

    // Draw three vertical black lines within the rounded rectangle
    for (int i = 0; i < SUBMENU_LINE_COUNT; ++i)
    {
        int lineY = (getTop() + getDisplayOffset()) + SUBMENU_LINE_V_OFFSET + i * SUBMENU_LINE_SEP; // Adjust the starting position of the lines
        getScreen()->drawFastHLine(SUBMENU_LINE_H_OFFSET, lineY, SUBMENU_LINE_H_LENGTH, NOT_SELECTED_TEXT_COLOUR);
    }

    // Draw the label
    getScreen()->setCursor(MENU_SEP + SUBMENU_LABEL_X_OFFSET, (getTop() + getDisplayOffset()) + (BUTTON_HEIGHT - 8) / 2); // Adjust the cursor position
    getScreen()->setTextColor(NOT_SELECTED_TEXT_COLOUR);
    getScreen()->print(label);
}

// Get the value in this menu or a submenu based on the passed ID
bool Submenu::getSelectorValue(const char *target_id, int *destination)
{
    for (int i = 0; i < elements.size(); i++)
    {
        if (elements[i]->getType() == SUBMENU)
        {
            // recursively search the submenu
            Submenu *submenu = static_cast<Submenu *>(elements[i]);
            if (submenu->getSelectorValue(target_id, destination))
            {
                return true;
            }
        }
        else if (elements[i]->getType() == SELECTOR)
        {
            Selector *selector = static_cast<Selector *>(elements[i]);
            // check if ID matches
            if (strcmp(selector->getId(), target_id) == 0)
            {
                *destination = selector->getValue();
                return true;
            }
        }
    }
    return false; // not found matching ID
}

bool Submenu::getCheckboxValue(const char *target_id, bool *destination)
{
    for (int i = 0; i < elements.size(); i++)
    {
        if (elements[i]->getType() == SUBMENU)
        {
            // recursively search the submenu
            Submenu *submenu = static_cast<Submenu *>(elements[i]);
            if (submenu->getCheckboxValue(target_id, destination))
            {
                return true;
            }
        }
        else if (elements[i]->getType() == CHECKBOX)
        {
            Checkbox *checkbox = static_cast<Checkbox *>(elements[i]);
            // check if ID matches
            if (strcmp(checkbox->getId(), target_id) == 0)
            {
                *destination = checkbox->getValue();
                return true;
            }
        }
    }
    return false; // not found matching ID
}

bool Submenu::getSliderValue(const char *target_id, int *destination)
{
    for (int i = 0; i < elements.size(); i++)
    {
        if (elements[i]->getType() == SUBMENU)
        {

            // recursively search the submenu
            Submenu *submenu = static_cast<Submenu *>(elements[i]);
            if (submenu->getSliderValue(target_id, destination))
            {
                return true;
            }
        }
        else if (elements[i]->getType() == SLIDER)
        {
            Slider *slider = static_cast<Slider *>(elements[i]);
            // check if ID matches
            if (strcmp(slider->getId(), target_id) == 0)
            {
                *destination = slider->getValue();
                return true;
            }
        }
    }
    return false; // not found matching ID
}

// Indicates that the submenu button has been pressed, or the subelement has been pressed depending on current state
void Submenu::press()
{
    if (!drawn)
    {
        drawMenu();
    }
    else if (elements[selectedElementIndex]->getType() == SUBMENU)
    {
        entered = true;
        elements[selectedElementIndex]->press();
    }
    else if (elements[selectedElementIndex]->getType() == PAGE)
    {
        entered = true;
        elements[selectedElementIndex]->press();
    }
    else
    {
        elements[selectedElementIndex]->press();
    }
}

// Adds a button to this submenu
void Submenu::addElement(Element *element)
{
    elements.push_back(element);
    element->setScreen(getScreen());
    element->setWidth(getScreen()->width());
    element->setTop(getMenuHeight());
    setMenuHeight(getMenuHeight() + element->getHeight());

    if (getMenuHeight() > getScreen()->height())
    {
        setScrollable(true);
    }
}

// Indicates the user wants to move to the element above (or wrap to the element below)
void Submenu::moveUp()
{
    // If in a submenu, navigate this instead of base
    if (entered && elements[selectedElementIndex]->getType() == SUBMENU)
    {
        Submenu *submenu = static_cast<Submenu *>(elements[selectedElementIndex]);
        submenu->moveUp();
    }
    else if (entered && elements[selectedElementIndex]->getType() == PAGE)
    {
        Page *page = static_cast<Page *>(elements[selectedElementIndex]);
        page->pageMoveUp();
    }
    else
    {
        // Stop displaying current element as selected
        elements[selectedElementIndex]->display();
        // Adjust the current selected index
        if (elements.size() > 0)
        {
            if (selectedElementIndex == 0)
            {
                selectedElementIndex = elements.size() - 1; // Wrap around to the bottom

                if (getScrollable())
                {
                    // Basically need to simulate moving down until we reach the last page
                    while (elements[selectedElementIndex]->getTop() + elements[selectedElementIndex]->getHeight() > getEndDisplayHeight())
                    {
                        for (int i = 0; i < elements.size(); i++)
                        {
                            elements[i]->setDisplayOffset(elements[i]->getDisplayOffset() - (getEndDisplayHeight() - getBaseDisplayHeight()));
                        }
                        setBaseDisplayHeight(getBaseDisplayHeight() + (getEndDisplayHeight() - getBaseDisplayHeight()));
                        int current_bottom = getBaseDisplayHeight();
                        for (int i = 0; i < elements.size(); i++)
                        {
                            // Check the full element isn't below current view window
                            if (elements[i]->getTop() < getBaseDisplayHeight())
                            {
                                continue;
                            }

                            // Check the element isn't above the view window
                            if ((elements[i]->getTop() + elements[i]->getHeight()) > getBaseDisplayHeight() + getScreen()->height())
                            {
                                continue;
                            }

                            current_bottom += elements[i]->getHeight();
                            setEndDisplayHeight(current_bottom);
                        }
                    }
                    drawMenu();
                    return;
                }
            }
            else
            {
                --selectedElementIndex;

                if (elements[selectedElementIndex]->getTop() < getBaseDisplayHeight())
                {
                    // Get all the elements above that fit into the screen space
                    setBaseDisplayHeight(getBaseDisplayHeight() - getScreen()->height());

                    // Get the highest point of all the elements that fit on the screen, this needs to be at '0'
                    int highest_visible = INT_MAX;
                    for (int i = 0; i < elements.size(); i++)
                    {
                        elements[i]->setDisplayOffset(elements[i]->getDisplayOffset() + getScreen()->height());
                        if (elements[i]->getTop() > getBaseDisplayHeight())
                        {
                            if (elements[i]->getTop() < highest_visible)
                            {
                                highest_visible = elements[i]->getTop();
                            }
                        }
                    }

                    // Adjust the display offsets to put the elements in the correct position
                    for (int i = 0; i < elements.size(); i++)
                    {
                        elements[i]->setDisplayOffset(elements[i]->getDisplayOffset() - (highest_visible - getBaseDisplayHeight()));
                    }
                    setBaseDisplayHeight(getBaseDisplayHeight() + (highest_visible - getBaseDisplayHeight()));
                    drawMenu();
                    return;
                }
            }
        }
        // Show new element selected
        elements[selectedElementIndex]->displaySelected();
    }
}

// Indicates the user wants to move to the element below (or wrap around to the element at the top)
void Submenu::moveDown()
{
    // If in a submenu, navigate this instead of base
    if (entered && elements[selectedElementIndex]->getType() == SUBMENU)
    {
        Submenu *submenu = static_cast<Submenu *>(elements[selectedElementIndex]);
        submenu->moveDown();
    }
    else if (entered && elements[selectedElementIndex]->getType() == PAGE)
    {
        Page *page = static_cast<Page *>(elements[selectedElementIndex]);
        page->pageMoveDown();
    }
    else
    {
        // Stop displaying current element as selected
        elements[selectedElementIndex]->display();
        // Adjust the current selected index
        if (elements.size() > 0)
        {
            if (selectedElementIndex == elements.size() - 1)
            {
                selectedElementIndex = 0; // Wrap around to the top
                if (scrollable)
                {
                    setBaseDisplayHeight(0);
                    for (int i = 0; i < elements.size(); i++)
                    {
                        elements[i]->setDisplayOffset(0);
                    }
                    drawMenu();
                    return;
                }
            }
            else
            {
                ++selectedElementIndex;
                if (elements[selectedElementIndex]->getTop() >= getEndDisplayHeight())
                {
                    for (int i = 0; i < elements.size(); i++)
                    {
                        elements[i]->setDisplayOffset(elements[i]->getDisplayOffset() - (getEndDisplayHeight() - getBaseDisplayHeight()));
                    }
                    setBaseDisplayHeight(getBaseDisplayHeight() + (getEndDisplayHeight() - getBaseDisplayHeight()));
                    drawMenu();
                    return;
                }
            }
        }
        // Show new element selected
        elements[selectedElementIndex]->displaySelected();
    }
}

// Moves left within the submenu
void Submenu::moveLeft()
{
    // make sure not passing into unentered submenu
    if (entered || (elements[selectedElementIndex]->getType() != SUBMENU))
    {
        elements[selectedElementIndex]->moveLeft();
    }
}

// Moves right within the submenu
void Submenu::moveRight()
{
    if (entered || (elements[selectedElementIndex]->getType() != SUBMENU))
    {
        elements[selectedElementIndex]->moveRight();
    }
}

// Displays the menu on the screen (draws all elements)
void Submenu::drawMenu()
{
    getScreen()->setTextSize(1);
    if (getScrollable())
    {
        for (int i = 0; i < elements.size(); i++)
        {
            elements[i]->setWidth(getScreen()->width() - SCROLLBAR_WIDTH - SCROLLBAR_PADDING);
        }
    }

    getScreen()->fillScreen(BACKGROUND_COLOUR);
    int current_top = getBaseDisplayHeight();
    int current_bottom = current_top;

    for (int i = 0; i < elements.size(); i++)
    {
        // Check the full element isn't below current view window
        if (elements[i]->getTop() < getBaseDisplayHeight())
        {
            continue;
        }

        // Check the element isn't above the view window
        if ((elements[i]->getTop() + elements[i]->getHeight()) > getBaseDisplayHeight() + getScreen()->height())
        {
            continue;
        }

        if (i == selectedElementIndex)
        {
            elements[i]->displaySelected();
        }
        else
        {
            elements[i]->display();
        }
        current_bottom += elements[i]->getHeight();
        current_top += elements[i]->getHeight();
        setEndDisplayHeight(current_bottom);
    }

    if (getScrollable())
    {
        drawScrollbar(getBaseDisplayHeight(), getEndDisplayHeight());
    }

    drawn = true;
}

// Draws the scrollbar based on the amount of the menu being displayed
void Submenu::drawScrollbar(int top, int bottom)
{
    // draw the background
    getScreen()->fillRoundRect(getScreen()->width() - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, getScreen()->height(), 2, NOT_SELECTED_COLOUR);

    // calculate bar position and height
    int top_y = map(top, 0, getMenuHeight(), 0, getScreen()->height());
    int bar_height = map(bottom, 0, getMenuHeight(), 0, getScreen()->height()) - top_y;

    // draw the bar
    getScreen()->fillRoundRect(getScreen()->width() - SCROLLBAR_WIDTH, top_y, SCROLLBAR_WIDTH, bar_height, 2, SELECTED_COLOUR);
}

// Indicates the user wants to go back
void Submenu::back()
{
    if (entered && elements[selectedElementIndex]->getType() == SUBMENU)
    {
        Submenu *submenu = static_cast<Submenu *>(elements[selectedElementIndex]);
        if (submenu->isInSubmenu())
        {
            submenu->back();
        }
        else
        {
            // also set drawn to false for submenu
            submenu->setDrawn(false);
            entered = false;
            drawMenu();
        }
    }
    else if (entered && elements[selectedElementIndex]->getType() == PAGE)
    {
        Page *page = static_cast<Page *>(elements[selectedElementIndex]);
        page->pageBack();
        if (!page->isEntered())
        {
            // also set drawn to false for submenu
            entered = false;
            drawMenu();
        }
    }
}

// Gets the height of the submenu button
int Submenu::getHeight()
{
    return SUBMENU_HEIGHT + MENU_SEP;
}

// get byte representation of subcomponents
bool Submenu::serialize(uint8_t *buffer, int *byte_index, uint16_t length_limit)
{
    for (int i = 0; i < elements.size(); i++)
    {
        // check that the element serialized correctly
        if (!elements[i]->serialize(buffer, byte_index, length_limit)){
            return false;
        }

        // check that we haven't exceeded the buffer
        if (*byte_index >= length_limit){
			return false;
		}
    }
    return true;
}

// load subcomponent values from byte representation
bool Submenu::deserialize(uint8_t *buffer, int *byte_index, uint16_t length_limit)
{
    for (int i = 0; i < elements.size(); i++)
    {
        // check that the element deserialized correctly
        if (!elements[i]->deserialize(buffer, byte_index, length_limit)){
            return false;
        }
    }
    return true;
}

// set all subcomponents to default and return to root menu
void Submenu::toDefault()
{
    for (int i = 0; i < elements.size(); i++)
    {
        elements[i]->toDefault();
    }
    entered = false;
    setDrawn(false);
}

void Submenu::intInput(uint32_t input){
	if (entered && elements[selectedElementIndex]->getType() == PAGE)
	{
		// If we are in a page, pass to page, and if the page has exited, draw menu
		Page *page = static_cast<Page *>(elements[selectedElementIndex]);
		page->intInput(input);
	} else if (entered && elements[selectedElementIndex]->getType() == SUBMENU)
	{
		// If we are in a page, pass to page, and if the page has exited, draw menu
		Submenu *submenu = static_cast<Submenu *>(elements[selectedElementIndex]);
		submenu->intInput(input);
	}
}