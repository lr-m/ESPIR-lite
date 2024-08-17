#include "Element.h"

#define PAGE_HEIGHT 14
#define TRIANGLE_HEIGHT 6
#define TRIANGLE_WIDTH 5
#define TRIANGLE_X_OFFSET 5
#define TRIANGLE_Y_OFFSET 4
#define LABEL_X_OFFSET 16

#ifndef Page_h
#define Page_h

class Page : public Element
{
public:
    Page(const char *label);
    Page();

    void display() override;
    void displaySelected() override;
    void press() override;
    void moveLeft() override;
    void moveRight() override;
    int getHeight() override;
    bool serialize(uint8_t *buffer, int *index, uint16_t length_limit) override;
    bool deserialize(uint8_t *buffer, int *index, uint16_t length_limit) override;
    void toDefault() override;
    void intInput(uint32_t input);

    virtual void pageMoveUp() = 0;
    virtual void pageMoveDown() = 0;
    virtual void pageMoveLeft() = 0;
    virtual void pageMoveRight() = 0;
    virtual void pagePress() = 0;
    virtual void pageBack() = 0;
    virtual void pageDisplay() = 0;
    virtual bool pageSerialize(uint8_t *buffer, int *index, uint16_t length_limit) = 0;
    virtual bool pageDeserialize(uint8_t *buffer, int *index, uint16_t length_limit) = 0;
    virtual void pageDefault() = 0;
    virtual void pageIntInput(uint32_t input) = 0;

    bool isEntered() const { return entered; }
    void setEntered(boolean new_entered) { entered = new_entered; }
    void setLabel(const char *new_label) { label = new_label; }

private:
    const char *label;
    bool entered = false;
};

#endif