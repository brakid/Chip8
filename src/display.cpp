#include <iostream>
#include "display.h"

using namespace std;

Display::Display() {
    clean();
}

void Display::draw() {
    for (int index = 0; index < DISPLAY_WIDTH + 2; index++) {
        cout << "=";    
    }
    cout << endl;

    for (int rowIndex = 0; rowIndex < DISPLAY_HEIGHT; rowIndex++) {
        cout << "=";
        for(int columnIndex = 0; columnIndex < DISPLAY_WIDTH; columnIndex++) {
            uint8_t pixelValue = display[rowIndex][columnIndex] & 0x1; // last bit is relevant only
            cout << (pixelValue == 0x1 ? "*" : " ");
        }
        cout << "=" << endl;
    }
    for (int index = 0; index < DISPLAY_WIDTH + 2; index++) {
        cout << "=";    
    }
    cout << endl;
}

void Display::clean() {
    for (int rowIndex = 0; rowIndex < DISPLAY_HEIGHT; rowIndex++) {
        for(int columnIndex = 0; columnIndex < DISPLAY_WIDTH; columnIndex++) {
            display[rowIndex][columnIndex] = 0x0;
        }   
    }
}

bool getBitFromByte(uint8_t byte, uint8_t position) {
    return (byte >> (position & 0x07)) & 0x1;
}

bool Display::drawSpriteBit(uint8_t columnIndex, uint8_t rowIndex, bool spriteBit) {
    uint8_t xorResult = (display[rowIndex][columnIndex] ^ spriteBit) & 0x01;
    display[rowIndex][columnIndex] = xorResult;

    return xorResult == 0x00 ? true : false;
}

bool Display::drawSpriteByte(uint8_t columnIndex, uint8_t rowIndex, uint8_t sprite) {
    bool collision = false;
    for (uint8_t offset = 0; offset < 8; offset++) {
        collision |= drawSpriteBit((columnIndex + offset) % DISPLAY_WIDTH, rowIndex, getBitFromByte(sprite, 8 - offset));
    }

    return collision;
}

bool Display::drawSprite(uint8_t left, uint8_t top, uint8_t* spritesPointer, uint8_t spritesByteCount) {
    bool collision = false;
    for (uint8_t offset = 0; offset < spritesByteCount; offset++) {
        collision |= drawSpriteByte(left, (top + offset) % DISPLAY_HEIGHT, *(spritesPointer + offset));
    }

    return collision;
}