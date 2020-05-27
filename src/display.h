#ifndef DISPLAY
#define DISPLAY

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

class Display {
private:
    uint8_t display[DISPLAY_HEIGHT][DISPLAY_WIDTH];

    bool drawSpriteBit(uint8_t columnIndex, uint8_t rowIndex, bool spriteBit);
    bool drawSpriteByte(uint8_t columnIndex, uint8_t rowIndex, uint8_t sprite);
public:
    Display();

    void draw();
    void clean();
    
    bool drawSprite(uint8_t left, uint8_t top, uint8_t* spritesPointer, uint8_t spritesByteCount);
};

#endif