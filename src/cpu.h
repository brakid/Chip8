
#ifndef CPU
#define CPU

#define MEMORY_SIZE 4096
#define REGISTERS 16
#define F 15
#define PROGRAM_START_ADDRESS 0x0200
#define FONT_SPRITE_START_ADDRESS 0x0000
#define FONT_SPRITE_LENGTH 5
#define FONT_SPRITE_BYTES 16 * FONT_SPRITE_LENGTH
#define INCREMENT 2

#include <stack>
#include "display.h"
#include "keyboard.h"

using namespace std;

class Chip8CPU {
private:
    std::stack<uint16_t>* stack;
    
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[REGISTERS];

    uint16_t I;
    uint16_t programmCounter;

    uint8_t delayTimer;
    uint8_t soundTimer;

    WINDOW* window;
    Display* display;
    Keyboard* keyboard;

    void init();

    bool decodeOpcode(uint16_t opcode);
public:
    Chip8CPU(WINDOW* window, Display* display, Keyboard* keyboard);
    ~Chip8CPU();

    bool runCycle();
    void loadMemory(uint8_t* memoryData, uint16_t memoryDataLength);
};

void handleUnsupportedOpcode(WINDOW* window, uint16_t opcode);
uint16_t extractAddressFromOpcode(uint16_t opcode); // takes 0x0FFF
uint8_t getValue(uint8_t highNibble, uint8_t lowNibble);;
void incrementProgramCounter(uint16_t& programCounter);
void decrementTimer(uint8_t& timer);

#endif