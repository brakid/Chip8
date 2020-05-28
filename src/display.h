#ifndef DISPLAY
#define DISPLAY

#include <ncurses.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

class Display {
private:
    uint8_t display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    WINDOW* window;
    bool dispayDidChange;

    bool drawSpriteBit(uint8_t columnIndex, uint8_t rowIndex, bool spriteBit);
    bool drawSpriteByte(uint8_t columnIndex, uint8_t rowIndex, uint8_t sprite);
public:
    Display(WINDOW* window);

    void draw();
    void clean();
    
    bool drawSprite(uint8_t left, uint8_t top, uint8_t* spritesPointer, uint8_t spritesByteCount);
};

#endif