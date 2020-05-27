
#ifndef KEYBOARD
#define KEYBOARD

class Keyboard {
private:
    bool isBlocking;
    bool volatile keyPressed;
    uint8_t volatile keyValue;
public:
    Keyboard(bool isBlocking);

    bool isKeyPressed();
    uint8_t getKeyValue();
    void readKey();
};

#endif