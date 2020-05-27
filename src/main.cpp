#include <iostream>
#include <thread>
#include <chrono>
#include <time.h> 
#include "cpu.h"
#include "display.h"
#include "keyboard.h"
#include "fileloader.h"

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))
#define DATA_LENGTH MEMORY_SIZE - PROGRAM_START_ADDRESS

using namespace std;

void sleep(uint64_t milliseconds) {
    this_thread::sleep_for(chrono::milliseconds(milliseconds));
};

uint8_t memoryData[DATA_LENGTH];

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        cout << "Expecting binary file name to load, optional: blocking/ nonblocking" << endl;
        return 0;
    }

    string fileName(argv[1]);

    bool isBlocking = true;
    if (argc == 3) {
        string blockingArgument(argv[2]);
        if (blockingArgument == "nonblocking") {
            isBlocking = false;
        }
    }

    cout << "Is blocking for keyboard input: " << (isBlocking ? "yes" : "no") << endl;

    bool success = loadFileContent(fileName, memoryData, DATA_LENGTH);

    if (!success) {
        cout << "Could not read file " << fileName << endl;
        return 1;
    }

    Display* display = new Display();
    Keyboard* keyboard = new Keyboard(isBlocking);
    Chip8CPU* chip8Cpu = new Chip8CPU(display, keyboard);
    
    cout << "Starting Chip8 Emulator" << endl;

    chip8Cpu->loadMemory(memoryData, LENGTH(memoryData));

    display->draw();
    
    bool wasSuccessful = false;
    do {
        wasSuccessful = chip8Cpu->runCycle();
        cout << "Emulating cycle: " << (wasSuccessful ? "success" : "failed") << endl;
        display->draw();
        keyboard->readKey();
        sleep(15); // ~60Hz
    } while (wasSuccessful);
    cout << "Emulation ended" << endl;

    display->draw();
    
    delete chip8Cpu;
    delete display;
    delete keyboard;

    return 0;
}