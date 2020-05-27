#include <iostream>
#include <thread>
#include <chrono>
#include "cpu.h"
#include "display.h"
#include "keyboard.h"
#include "fileloader.h"

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))
#define DATA_LENGTH MEMORY_SIZE - PROGRAM_START_ADDRESS

using namespace std;

void sleep(uint64_t milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(200));
};

//uint8_t memoryData[10] = { 0x60, 0x08, 0x61, 0x00, 0x50, 0x10, 0x00, 0x00, 0x12, 0x00 };
//uint8_t memoryData[12] = { 0x22, 0x08, 0x61, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x60, 0xFF, 0x00, 0xEE };
//uint8_t memoryData[22] = { 0x0, 0xe0, 0x60, 0x0A, 0x61, 0x1, 0x62, 0x0, 0x80, 0x15, 0x50, 0x20, 0x12, 0x8, 0x22, 0x12, 0x12, 0x0, 0x6f, 0x42, 0x0, 0xee };
//uint8_t memoryData[4] = { 0xD0, 0x05, 0xF0, 0x0A };
//uint8_t memoryData[10] = { 0x60, 0x08, 0x61, 0x3d, 0x62, 0x00, 0xF0, 0x29, 0xD1, 0x25 };
uint8_t memoryData[DATA_LENGTH];

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Expecting binary file name to load" << endl;
        return 0;
    }

    string fileName(argv[1]);

    bool success = loadFileContent(fileName, memoryData, DATA_LENGTH);

    if (!success) {
        cout << "Could not read file " << fileName << endl;
        return 1;
    }

    Display* display = new Display();
    Keyboard* keyboard = new Keyboard();
    Chip8CPU* chip8Cpu = new Chip8CPU(display, keyboard);
    
    cout << "Starting Chip8 Emulator" << endl;

    chip8Cpu->loadMemory(memoryData, LENGTH(memoryData));

    display->draw();
    
    bool wasSuccessful = false;
    do {
        wasSuccessful = chip8Cpu->runCycle();
        cout << "Emulating cycle: " << (wasSuccessful ? "success" : "failed") << endl;
        keyboard->readKey();
        sleep(200);
    } while (wasSuccessful);
    cout << "Emulation ended" << endl;

    display->draw();
    
    delete chip8Cpu;
    delete display;
    delete keyboard;
    return 0;
}