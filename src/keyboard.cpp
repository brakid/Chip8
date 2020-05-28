#include <iostream>
#include <cstdio>
#include "keyboard.h"

using namespace std;

Keyboard::Keyboard(WINDOW* window) {
    this->window = window;
    
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
        case ERR:
            return true;
    }
    return false;
}

bool isERR(char character) {
    return character == ERR;
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
    wclear(window); 
    wmove(window, 0, 0);
    wrefresh(window);
    wprintw(window, "Press key: [0-9a-f]");
    wechochar(window, '\n');
    char character = 0;
    while(!validateCharacter(character)) {
        character = wgetch(window);
    }

    bool keyIsPressed = !isERR(character); // ERR = no key pressed

    if (keyIsPressed) {
        keyPressed = keyIsPressed;
        keyValue = getValue(character);
        wprintw(window, "Pressed key: ");
        waddch(window, character);
        wechochar(window, '\n');
    } else {
        keyPressed = false;
    }
}