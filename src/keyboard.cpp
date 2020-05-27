#include <iostream>
#include <cstdio>
#include "keyboard.h"

using namespace std;

Keyboard::Keyboard(bool isBlocking) {
    this->isBlocking = isBlocking;
    keyPressed = false;
    keyValue = 0x00;
}

bool Keyboard::isKeyPressed() {
    return keyPressed;
}

uint8_t Keyboard::getKeyValue() {
    return keyValue;
}

bool validateCharacter(char character) {
    switch(character) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case '\n':
            return true;
    }
    return false;
}

bool isKeyEnter(char character) {
    return character == '\n';
}

uint8_t getValue(char character) {
    switch(character) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return character - '0';
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            return character - 'a' + 10;
    }
    return 0x00;
}

void Keyboard::readKey() {
    if (!isBlocking) {
        return; // do not check for keyboard input
    }

    char character = 0;
    while(!validateCharacter(character)) {
        cout << "Press key: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, b, c, d, e, f, no key = enter" << endl;
        character = getchar();
    }

    bool keyIsPressed = !isKeyEnter(character);

    if (keyIsPressed) {
        keyPressed = keyIsPressed;
        keyValue = getValue(character);
    } else {
        keyPressed = false;
    }
}