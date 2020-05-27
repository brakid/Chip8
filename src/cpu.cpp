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

Chip8CPU::Chip8CPU(Display* display, Keyboard* keyboard) {
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

    cout << "Program Counter: " << hex << (int)programmCounter << ", Opcode: " << hex << (int)opcode << endl;

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
                            cout << "Clean display" << endl;
                            break;
                        }
                        case 0xE: {
                            uint16_t returnAddress = stack->top();
                            cout << "Return back to: " << hex << (int)returnAddress << endl;
                            programmCounter = returnAddress;
                            break;
                        }
                        default: {
                            handleUnsupportedOpcode(opcode);
                            unknownOpcode = true;
                            break;
                        }
                    }
                    break;
                }
                default: { // opcode is 0x0NNN
                    handleUnsupportedOpcode(opcode);
                    unknownOpcode = true;
                    break;
                    /*
                    uint16_t returnAddress = programmCounter;
                    stack->push(returnAddress);
                    uint16_t callAddress = extractAddressFromOpcode(opcode);
                    cout << "Calling program at: " << hex << (int)callAddress << ", return address: " << hex << (int)returnAddress << endl;
                    programmCounter = callAddress - INCREASE;
                    break;
                    */
                }
            }
            break;
        }
        case 0x1: {
            uint16_t jumpAddress = extractAddressFromOpcode(opcode);
            cout << "Jump to: " << hex << (int)jumpAddress << endl;
            programmCounter = jumpAddress - INCREASE; // as the counter is increased after each cycle
            break;
        }
        case 0x2: {
            uint16_t returnAddress = programmCounter;
            stack->push(returnAddress);
            uint16_t subroutineAddress = extractAddressFromOpcode(opcode);
            cout << "Calling subroutine at: " << hex << (int)subroutineAddress << ", return address: " << hex << (int)returnAddress << endl;
            programmCounter = subroutineAddress - INCREASE; // as the counter is increased after each cycle
            break;
        }
        case 0x3: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            cout << "Comparing: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " == " << hex << (int)constant << endl;
            
            if (V[registerIndex] == constant) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0x4: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            cout << "Comparing: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " != " << hex << (int)constant << endl;
            
            if (V[registerIndex] != constant) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0x5: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];
            
            cout << "Comparing: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " == V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
            
            if (V[firstRegisterIndex] == V[secondRegisterIndex]) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0x6: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            cout << "Set: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " to " << hex << (int)constant << endl;
            V[registerIndex] = constant;

            break;
        }
        case 0x7: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            
            cout << "Add: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " + " << hex << (int)constant << endl;
            V[registerIndex]+= constant;
            
            break;
        }
        case 0x8: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];

            switch(opcodeNibbels[3]) {
                case 0x0: {
                    cout << "Set: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " to V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
                    
                    V[firstRegisterIndex] = V[secondRegisterIndex];
                    break;
                }
                case 0x1: {
                    cout << "Set: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " to V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " | V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
                    
                    V[firstRegisterIndex] |= V[secondRegisterIndex];
                    break;
                }
                case 0x2: {
                    cout << "Set: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " to V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " & V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
                    
                    V[firstRegisterIndex] &= V[secondRegisterIndex];
                    break;
                }
                case 0x3: {
                    cout << "Set: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " to V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " ^ V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
                    
                    V[firstRegisterIndex] ^= V[secondRegisterIndex];
                    break;
                }
                case 0x4: {
                    cout << "Add: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " + V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
                    
                    uint16_t fullResult = V[firstRegisterIndex] + V[secondRegisterIndex];
                    uint8_t carryOverFlag = (fullResult & 0x0100) >> 8;
                    uint8_t result = (fullResult & 0x00FF);
                    
                    V[firstRegisterIndex] = result;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x5: {
                    cout << "Subtract: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " - V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
                    
                    uint8_t carryOverFlag = (V[firstRegisterIndex] > V[secondRegisterIndex]) ? 0x1 : 0x0;
                    uint8_t result = V[firstRegisterIndex] - V[secondRegisterIndex];
                    
                    V[firstRegisterIndex] = result;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x6: {
                    cout << "Shift: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " >> 1" << endl;
                    
                    uint8_t carryOverFlag = V[firstRegisterIndex] & 0x01;
                    
                    V[firstRegisterIndex] >>= 1;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0x7: {
                    cout << "Set: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " to V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << " - V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << endl;
                    
                    uint8_t carryOverFlag = (V[secondRegisterIndex] > V[firstRegisterIndex]) ? 0x1 : 0x0;
                    uint8_t result = V[secondRegisterIndex] - V[firstRegisterIndex];
                    
                    V[firstRegisterIndex] = result;
                    V[F] = carryOverFlag;
                    break;
                }
                case 0xE: {
                    cout << "Shift: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " << 1" << endl;
                    
                    uint8_t carryOverFlag = V[firstRegisterIndex] & 0x80;
                    V[firstRegisterIndex] <<= 1;
                    V[F] = carryOverFlag;
                    break;
                }
                default: {
                    handleUnsupportedOpcode(opcode);
                    unknownOpcode = true;
                    break;
                }
            }
            break;
        }
        case 0x9: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];
            
            cout << "Comparing: V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << " != V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << endl;
            
            if (V[firstRegisterIndex] != V[secondRegisterIndex]) {
                increaseProgramCounter(programmCounter); // skip next command
            }
            break;
        }
        case 0xA: {
            uint16_t address = extractAddressFromOpcode(opcode);
            
            cout << "Set: I to: " << hex << (int)address << endl;
            
            I = address;
            break;
        }
        case 0xB: {
            uint16_t jumpAddress = extractAddressFromOpcode(opcode);
            
            cout << "Jump to: V[0]=" << hex << (int)V[0] << " + " << hex << (int)jumpAddress << endl;
            
            programmCounter = V[0] + jumpAddress - INCREASE; // as the counter is increased after each cycle
            break;
        }
        case 0xC: {
            uint8_t registerIndex = opcodeNibbels[1];
            uint8_t constant = getValue(opcodeNibbels[2], opcodeNibbels[3]);
            uint8_t random = rand() & 0xFF;
            
            cout << "Set: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " to random()=" << hex << (int)random << " & " << hex << (int)constant << endl;
            
            V[registerIndex] = random & constant;
            break;
        }
        case 0xD: {
            uint8_t firstRegisterIndex = opcodeNibbels[1];
            uint8_t secondRegisterIndex = opcodeNibbels[2];
            uint8_t nibble = opcodeNibbels[3];
            
            cout << "Display: n=" << (int)nibble << " bytes from I=" << hex << (int)I << " at (V[" << hex << (int)firstRegisterIndex << "]=" << hex << (int)V[firstRegisterIndex] << ", V[" << hex << (int)secondRegisterIndex << "]=" << hex << (int)V[secondRegisterIndex] << ")" << endl;
            
            uint8_t* spritesPointer = &memory[I];
            uint8_t spritesByteCount = nibble;
            uint8_t left = V[firstRegisterIndex];
            uint8_t top = V[secondRegisterIndex];
            
            bool hasCollision = display->drawSprite(left, top, spritesPointer, spritesByteCount);

            V[F] = hasCollision;

            break;
        }
        case 0xE: {
            uint8_t registerIndex = opcodeNibbels[1];
            switch(getValue(opcodeNibbels[2], opcodeNibbels[3])) {
                case 0x9E: {
                    cout << "Is key pressed with value: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
                    
                    if (keyboard->isKeyPressed() == true && keyboard->getKeyValue() == V[registerIndex]) {
                        increaseProgramCounter(programmCounter); // skip next command
                    }
                    break;
                }
                case 0xA1: {
                    cout << "Is key not pressed with value: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
                    
                    if (keyboard->isKeyPressed() == false || keyboard->getKeyValue() != V[registerIndex]) {
                        increaseProgramCounter(programmCounter); // skip next command
                    }
                    break;
                }
                default: {
                    handleUnsupportedOpcode(opcode);
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
                    cout << "Set: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " to DT=" << hex << (int)delayTimer << endl;
            
                    V[registerIndex] = delayTimer;
                    break;
                }
                case 0x0A: {
                    if (!keyboard->isKeyPressed()) {
                        cout << "Waiting for key press" << endl;
                        programmCounter -= INCREASE; // rewind to this command
                        break;
                    }
                    uint8_t keyValue = keyboard->getKeyValue();

                    cout << "Set: V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << " to K=" << hex << (int)keyValue << endl;
            
                    V[registerIndex] = keyValue;
                    break;
                }
                case 0x15: {
                    cout << "Set: DT to V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
            
                    delayTimer = V[registerIndex];
                    break;
                }
                case 0x18: {
                    cout << "Set: ST to V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
            
                    soundTimer = V[registerIndex];
                    break;
                }
                case 0x1E: {
                    cout << "Set: I to I=" << hex << (int)I << " + V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
            
                    I += V[registerIndex];
                    break;
                }
                case 0x29: {
                    cout << "Set: I to sprite address of digit V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
            
                    I = FONT_SPRITE_START_ADDRESS + (V[registerIndex] * FONT_SPRITE_LENGTH);
                    break;
                }
                case 0x33: {
                    cout << "Set: I=" << hex << (int)I <<", I+1, I+2 to BCD representation of V[" << hex << (int)registerIndex << "]=" << hex << (int)V[registerIndex] << endl;
            
                    uint8_t ones = V[registerIndex] % 10;
                    uint8_t tens =  (V[registerIndex] / 10) % 10;
                    uint8_t hundreds = (V[registerIndex] / 100) % 10;
                    
                    memory[I] = hundreds;
                    memory[I+1] = tens;
                    memory[I+2] = ones;
                    break;
                }
                case 0x55: {
                    cout << "Unload registers V[0]..V[" << hex << (int)registerIndex << "] to memory[I] and following" << endl;
            
                    for (uint8_t offset = 0; offset <= registerIndex; offset++) {
                        memory[I + offset] = V[offset];
                    }
                    break;
                }

                case 0x65: {
                    cout << "Load registers V[0]..V[" << hex << (int)registerIndex << "] from memory[I] and following" << endl;
            
                    for (uint8_t offset = 0; offset <= registerIndex; offset++) {
                        V[offset] = memory[I + offset];
                    }
                    break;
                }
                default: {
                    handleUnsupportedOpcode(opcode);
                    unknownOpcode = true;
                    break;
                }
            }
            break;
        }
        default: {
            handleUnsupportedOpcode(opcode);
            unknownOpcode = true;
            break;
        }
    }
    increaseProgramCounter(programmCounter);

    if (soundTimer > 0) {
        cout << "\a";
    }

    decreaseTimer(delayTimer);
    decreaseTimer(soundTimer);


    return !unknownOpcode;
}

void handleUnsupportedOpcode(uint16_t opcode) {
    cout << "Unsupported Opcode: " << hex << (int)opcode << endl;
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