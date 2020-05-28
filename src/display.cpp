#include <iostream>
#include "display.h"

using namespace std;

Display::Display(WINDOW* window) {
    this->window = window;

    clean();
}

void Display::draw() {
    if (!dispayDidChange) {
        return;
    }

    wclear(window); 
    wmove(window, 0, 0);
    wrefresh(window);

    for (int index = 0; index < DISPLAY_WIDTH + 2; index++) {
        waddch(window, '=');
    }
    waddch(window, '\n');
    
    for (int rowIndex = 0; rowIndex < DISPLAY_HEIGHT; rowIndex++) {
        waddch(window, '=');
        for(int columnIndex = 0; columnIndex < DISPLAY_WIDTH; columnIndex++) {
            uint8_t pixelValue = display[rowIndex][columnIndex] & 0x1; // last bit is relevant only
            waddch(window, (pixelValue == 0x1 ? '*' : ' '));
        }
        waddch(window, '=');
        waddch(window, '\n');
    }
    for (int index = 0; index < DISPLAY_WIDTH + 2; index++) {
        waddch(window, '=');    
    }
    wechochar(window, '\n');

    dispayDidChange = false;
}

void Display::clean() {
    for (int rowIndex = 0; rowIndex < DISPLAY_HEIGHT; rowIndex++) {
        for(int columnIndex = 0; columnIndex < DISPLAY_WIDTH; columnIndex++) {
            display[rowIndex][columnIndex] = 0x0;
        }   
    }
    wclear(window); 
    wmove(window, 1, 0);
    wrefresh(window);
    dispayDidChange = true;
}

bool getBitFromByte(uint8_t byte, uint8_t position) {
    return (byte >> (position & 0x07)) & 0x1;
}

bool Display::drawSpriteBit(uint8_t columnIndex, uint8_t rowIndex, bool spriteBit) {
    bool deletePixel = display[rowIndex][columnIndex] == 0x01 && spriteBit == true;
    
    uint8_t xorResult = (display[rowIndex][columnIndex] ^ spriteBit) & 0x01;
    display[rowIndex][columnIndex] = xorResult;

    return deletePixel;
}

bool Display::drawSpriteByte(uint8_t columnIndex, uint8_t rowIndex, uint8_t sprite) {
    bool collision = false;
    for (uint8_t offset = 0; offset < 8; offset++) {
        collision |= drawSpriteBit((columnIndex + offset) % DISPLAY_WIDTH, rowIndex, getBitFromByte(sprite, 7 - offset));
    }

    return collision;
}

bool Display::drawSprite(uint8_t left, uint8_t top, uint8_t* spritesPointer, uint8_t spritesByteCount) {
    bool collision = false;
    for (uint8_t offset = 0; offset < spritesByteCount; offset++) {
        collision |= drawSpriteByte(left, (top + offset) % DISPLAY_HEIGHT, *(spritesPointer + offset));
    }
    dispayDidChange = true;

    return collision;
}