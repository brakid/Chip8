
#ifndef KEYBOARD
#define KEYBOARD

#include <ncurses.h>

class Keyboard {
private:
    WINDOW* window;
    bool volatile keyPressed;
    uint8_t volatile keyValue;
public:
    Keyboard(WINDOW* window);

    bool isKeyPressed();
    uint8_t getKeyValue();
    void readKey();
};

#endif