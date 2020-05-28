#include <iostream>
#include "cpu.h"

using namespace std;

uint8_t font_sprites[FONT_SPRITE_BYTES] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};

void Chip8CPU::init() {
    stack = new std::stack<uint16_t>();

    for(int index = 0; index < MEMORY_SIZE; index++) {
        memory[index] = 0x00;
    }

    for(int index = 0; index < FONT_SPRITE_BYTES; index++) {
        memory[index] = font_sprites[index];
    }

    for(int index = 0; index < REGISTERS; index++) {
        V[index] = 0x00;
    }

    I = 0x0000;
    programmCounter = PROGRAM_START_ADDRESS;

    delayTimer = 0;
    soundTimer = 0;

    display->clean();
}

Chip8CPU::Chip8CPU(WINDOW* window, Display* display, Keyboard* keyboard) {
    this->window = window;
    this->display = display;
    this->keyboard = keyboard;
    init();
}

Chip8CPU::~Chip8CPU() {
    delete stack;
}

void Chip8CPU::loadMemory(uint8_t* memoryData, uint16_t memoryDataLength) {
    for (uint16_t offset = 0; offset <= memoryDataLength; offset++) {
        uint8_t data = *(memoryData + offset);
        memory[PROGRAM_START_ADDRESS + offset] = data;
    }
}

bool Chip8CPU::runCycle() {
    uint16_t opcode = memory[programmCounter] << 8 | memory[programmCounter+1];

    wprintw(window, "Program Counter: 0x%04x, Opcode: 0x%04x\n", programmCounter, opcode);
    
    bool unknownOpcode = false;
    
    uint8_t opcodeNibbels[4];
    opcodeNibbels[0] = (opcode >> 12) & 0x0F;
    opcodeNibbels[1] = (opcode >> 8) & 0x0F;
    opcodeNibbels[2] = (opcode >> 4) & 0x0F;
    opcodeNibbels[3] = opcode & 0x0F;
    
    switch(opcodeNibbels[0]) {
        case 0x0: {
            switch(getValue(opcodeNibbels[1], opcodeNibbels[2])) {
                case 0x0E: {
                    switch(opcodeNibbels[3]) {
                        case 0x0: {
                            display->clean();
                            
                            wprintw(window, "Clean display\n");
                            break;
                        }
                        case 0xE: {
                            uint16_t returnAddress = stack->top();
                            stack->pop();
                            
                            wprintw(window, "Return back to: 0x%04x\n", returnAddress);
                            
                            programmCounter = returnAddress;
                            break;
                        }
                        default: {
                            handleUnsupportedOpcode(window, opcode);
                            unknownOpcode = true;
                            break;
                        }
                    }
                    break;
                }
                default: { // opcode is 0x0NNN
                    uint16_t returnAddress = programmCounter;
                    stack->push(returnAddress);
                    uint16_t callAddress = extractAddressFromOpcode(opcode);

                    wprintw(window, "Calling program at: 0x%04x, return address: 0x%04x\n", callAddress, returnAddress);
                    
                    programmCounter = callAddress - INCREASE;
                    break;
                }
            }
            break;
        }
        case 0x1: {
            uint16_t jumpAddress = extractAddressFromOpcode(opcode);
            
            wprintw(window, "Jump to: 0x%04x\n", jumpAddress);
            
            programmCounter = jumpAddress - INCREASE; // as the counter is increased after each cycle
            break;
        }
        case 0x2: {
            uint16_t returnAddress = programmCounter;
            stack->push(returnAddress);
            uint16_t subroutineAddress = extractAddressFromOpcode(opcode);
            
            wprintw(window, "Calling subroutine at: 0x%04x, return address: 0x%04x\n", subroutineAddress, returnAddress);
            
            programmCounter = subroutineAddress - INCREASE; // as the counter is increased after each cycle
            break;
        }
        case 0x3: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            wprintw(window, "Comparing: V[0x%01x]=0x%02x == 0x%02x\n", registerIndex, V[registerIndex], constant);
            
            if (V[registerIndex] == constant) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0x4: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            wprintw(window, "Comparing: V[0x%01x]=0x%02x != 0x%02x\n", registerIndex, V[registerIndex], constant);
            
            if (V[registerIndex] != constant) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0x5: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];
            
            wprintw(window, "Comparing: V[0x%01x]=0x%02x == V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
            
            if (V[firstRegisterIndex] == V[secondRegisterIndex]) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0x6: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            wprintw(window, "Set: V[0x%01x]=0x%02x = 0x%02x\n", registerIndex, V[registerIndex], constant);
            
            V[registerIndex] = constant;

            break;
        }
        case 0x7: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            wprintw(window, "Add: V[0x%01x]=0x%02x += 0x%02x\n", registerIndex, V[registerIndex], constant);
            
            V[registerIndex]+= constant;
            
            break;
        }
        case 0x8: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];

            switch(opcodeNibbels[3]) {
                case 0x0: {
                    wprintw(window, "Set: V[0x%01x]=0x%02x = V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
                    
                    V[firstRegisterIndex] = V[secondRegisterIndex];
                    break;
                }
                case 0x1: {
                    wprintw(window, "Set: V[0x%01x]=0x%02x != V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
                    
                    V[firstRegisterIndex] |= V[secondRegisterIndex];
                    break;
                }
                case 0x2: {
                    wprintw(window, "Set: V[0x%01x]=0x%02x &= V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
                    
                    V[firstRegisterIndex] &= V[secondRegisterIndex];
                    break;
                }
                case 0x3: {
                    wprintw(window, "Set: V[0x%01x]=0x%02x ^= V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
                    
                    V[firstRegisterIndex] ^= V[secondRegisterIndex];
                    break;
                }
                case 0x4: {
                    wprintw(window, "Add: V[0x%01x]=0x%02x += V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
                    
                    uint16_t fullResult = V[firstRegisterIndex] + V[secondRegisterIndex];
                    uint8_t carryOverFlag = (fullResult > 255) ? 0x01 : 0x00;
                    uint8_t result = (fullResult & 0x00FF);
                    
                    V[firstRegisterIndex] = result;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x5: {
                    wprintw(window, "Subtract: V[0x%01x]=0x%02x -= V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
                    
                    uint8_t carryOverFlag = (V[firstRegisterIndex] > V[secondRegisterIndex]) ? 0x01 : 0x00;
                    uint8_t result = V[firstRegisterIndex] - V[secondRegisterIndex];
                    
                    V[firstRegisterIndex] = result;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x6: {
                    wprintw(window, "Shift: V[0x%01x]=0x%02x >> 1\n", firstRegisterIndex, V[firstRegisterIndex]);
                    
                    uint8_t carryOverFlag = V[firstRegisterIndex] & 0x01;
                    
                    V[firstRegisterIndex] >>= 1;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x7: {
                    wprintw(window, "Subtract: V[0x%01x] = V[0x%01x]=0x%02x - V[0x%01x]=0x%02x\n", firstRegisterIndex, secondRegisterIndex, V[secondRegisterIndex], firstRegisterIndex, V[firstRegisterIndex]);
                    
                    uint8_t carryOverFlag = (V[secondRegisterIndex] > V[firstRegisterIndex]) ? 0x01 : 0x00;
                    uint8_t result = V[secondRegisterIndex] - V[firstRegisterIndex];
                    
                    V[firstRegisterIndex] = result;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0xE: {
                    wprintw(window, "Shift: V[0x%01x]=0x%02x << 1\n", firstRegisterIndex, V[firstRegisterIndex]);
                    
                    uint8_t carryOverFlag = (V[firstRegisterIndex] >= 0x80) ? 0x01 : 0x00;
                    
                    V[firstRegisterIndex] <<= 1;
                    V[F] = carryOverFlag;
                    break;
                }
                default: {
                    handleUnsupportedOpcode(window, opcode);
                    unknownOpcode = true;
                    break;
                }
            }
            break;
        }
        case 0x9: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];
            
            wprintw(window, "Comparing: V[0x%01x]=0x%02x != V[0x%01x]=0x%02x\n", firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
            
            if (V[firstRegisterIndex] != V[secondRegisterIndex]) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0xA: {
            uint16_t address = extractAddressFromOpcode(opcode);
            
            wprintw(window, "Set: I=0x%04x to 0x%04x\n", I, address);
            
            I = address;
            break;
        }
        case 0xB: {
            uint16_t jumpAddress = extractAddressFromOpcode(opcode);
            
            wprintw(window, "Jump to: V[0]=0x%02x + 0x%04x\n", V[0], jumpAddress);
            
            programmCounter = V[0] + jumpAddress - INCREASE; // as the counter is increased after each cycle
            break;
        }
        case 0xC: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            uint8_t random = rand() & 0xFF;
            
            wprintw(window, "Set: V[0x%01x]=0x%02x = random()=0x%02x & 0x%02x\n", registerIndex, V[registerIndex], random, constant);
            
            V[registerIndex] = random & constant;
            break;
        }
        case 0xD: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];
            uint8_t nibble = opcodeNibbels[3];
            
            wprintw(window, "Display: n=0x%01x bytes from I=0x%04x at (V[0x%01x]=0x%02x, V[0x%01x]=0x%02x)\n", nibble, I, firstRegisterIndex, V[firstRegisterIndex], secondRegisterIndex, V[secondRegisterIndex]);
            
            uint8_t* spritesPointer = &memory[I];
            uint8_t spritesByteCount = nibble;
            uint8_t left = V[firstRegisterIndex];
            uint8_t top = V[secondRegisterIndex];
            
            bool hasCollision = display->drawSprite(left, top, spritesPointer, spritesByteCount);

            V[F] = hasCollision ? 0x01 : 0x00;
            break;
        }
        case 0xE: {
            uint8_t registerIndex = opcodeNibbels[1];
            switch(getValue(opcodeNibbels[2], opcodeNibbels[3])) {
                case 0x9E: {
                    wprintw(window, "Is key pressed with value V[0x%01x]=0x%02x\n", registerIndex, V[registerIndex]);
                    
                    if (keyboard->isKeyPressed() == true && keyboard->getKeyValue() == V[registerIndex]) {
                        increaseProgramCounter(programmCounter); // skip next command
                    }
                    break;
                }
                case 0xA1: {
                    wprintw(window, "Is key not pressed with value V[0x%01x]=0x%02x\n", registerIndex, V[registerIndex]);
                    
                    if (keyboard->isKeyPressed() == false || keyboard->getKeyValue() != V[registerIndex]) {
                        increaseProgramCounter(programmCounter); // skip next command
                    }
                    break;
                }
                default: {
                    handleUnsupportedOpcode(window, opcode);
                    unknownOpcode = true;
                    break;
                }
            }
            break;
        }
        case 0xF: {
            uint8_t registerIndex = opcodeNibbels[1];
            switch(getValue(opcodeNibbels[2], opcodeNibbels[3])) {
                case 0x07: {
                    wprintw(window, "Set: V[0x%01x]=0x%02x = DT=0x%02x\n", registerIndex, V[registerIndex], delayTimer);
                    
                    V[registerIndex] = delayTimer;
                    break;
                }
                case 0x0A: {
                    if (!keyboard->isKeyPressed()) {
                        wprintw(window, "Waiting for key press\n");

                        programmCounter -= INCREASE; // rewind to this command
                        break;
                    }
                    uint8_t keyValue = keyboard->getKeyValue();

                    wprintw(window, "Set: V[0x%01x]=0x%02x = K=0x%02x\n", registerIndex, V[registerIndex], keyValue);

                    V[registerIndex] = keyValue;
                    break;
                }
                case 0x15: {
                    wprintw(window, "Set: DT=0x%02x = V[0x%01x]=0x%02x\n", delayTimer, registerIndex, V[registerIndex]);
                    
                    delayTimer = V[registerIndex];
                    break;
                }
                case 0x18: {
                    wprintw(window, "Set: ST=0x%02x = V[0x%01x]=0x%02x\n", soundTimer, registerIndex, V[registerIndex]);
                    
                    soundTimer = V[registerIndex];
                    break;
                }
                case 0x1E: {
                    wprintw(window, "Add: I=0x%02x += V[0x%01x]=0x%02x\n", I, registerIndex, V[registerIndex]);
                    
                    I += V[registerIndex];
                    uint8_t carryOverFlag = (I > 255) ? 0x01 : 0x00;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x29: {
                    wprintw(window, "Set: I=0x%02x = sprite address of digit V[0x%01x]=0x%02x\n", I, registerIndex, V[registerIndex]);
                    
                    I = FONT_SPRITE_START_ADDRESS + (V[registerIndex] * FONT_SPRITE_LENGTH);
                    break;
                }
                case 0x33: {
                    wprintw(window, "Set: memory[I=0x%04x, I+1, I+2] = BCD representation of V[0x%01x]=0x%02x\n", I, registerIndex, V[registerIndex]);
                    
                    uint8_t ones = V[registerIndex] % 10;
                    uint8_t tens =  (V[registerIndex] / 10) % 10;
                    uint8_t hundreds = (V[registerIndex] / 100) % 10;
                    
                    memory[I] = hundreds;
                    memory[I+1] = tens;
                    memory[I+2] = ones;
                    break;
                }
                case 0x55: {
                    wprintw(window, "Unload registers V[0x0]..V[0x%01x] to memory[I=0x%04x,..]\n", registerIndex, I);
                    
                    for (uint8_t offset = 0; offset <= registerIndex; offset++) {
                        memory[I + offset] = V[offset];
                    }
                    break;
                }

                case 0x65: {
                    wprintw(window, "Load registers V[0x0]..V[0x%01x] from memory[I=0x%04x,..]\n", registerIndex, I);
                    
                    for (uint8_t offset = 0; offset <= registerIndex; offset++) {
                        V[offset] = memory[I + offset];
                    }
                    break;
                }
                default: {
                    handleUnsupportedOpcode(window, opcode);
                    unknownOpcode = true;
                    break;
                }
            }
            break;
        }
        default: {
            handleUnsupportedOpcode(window, opcode);
            unknownOpcode = true;
            break;
        }
    }
    increaseProgramCounter(programmCounter);

    if (soundTimer > 0) {
        beep();
    }

    decreaseTimer(delayTimer);
    decreaseTimer(soundTimer);

    return !unknownOpcode;
}

void handleUnsupportedOpcode(WINDOW* window, uint16_t opcode) {
    wprintw(window, "Unsupported Opcode: 0x%04x\n", opcode);
}

uint16_t extractAddressFromOpcode(uint16_t opcode) {
    return 0x0FFF & opcode;
}

uint8_t getValue(uint8_t highNibble, uint8_t lowNibble) {
    return (highNibble & 0x0F) << 4 | (lowNibble & 0x0F);
}

void increaseProgramCounter(uint16_t& programCounter) {
    programCounter = programCounter + INCREASE;
}

void decreaseTimer(uint8_t& timer) {
    if (timer > 0) {
        timer--;
    }
}