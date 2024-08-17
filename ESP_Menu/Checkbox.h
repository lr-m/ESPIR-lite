#include "Element.h"

#define CHECKBOX_HEIGHT 12
#define CHECKBOX_SIZE 8
#define CHECKBOX_PADDING 10

#ifndef Checkbox_h
#define Checkbox_h

class Checkbox : public Element
{
public:
    Checkbox(const char *label, const char *id, bool starting);
    void display() override;
    void displaySelected() override;
    void press() override;
    void moveLeft() override{};
    void moveRight() override{};
    int getHeight() override { return CHECKBOX_HEIGHT; };
    void toDefault() override{ value = starting; };

    bool serialize(uint8_t *buffer, int *index, uint16_t length_limit) override;
    bool deserialize(uint8_t *buffer, int *index, uint16_t length_limit) override;

    void drawCheckbox();
    const char *getId() const { return id; }
    bool getValue() const { return value; }
    void setValue(bool new_value) { value = new_value; }

private:
    const char *label;
    const char *id;
    bool value;
    bool starting;
};

#endif