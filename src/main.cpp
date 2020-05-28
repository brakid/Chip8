#include <iostream>
#include <thread>
#include <chrono>
#include <time.h>
#include <ncurses.h>
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
        cout << "Expecting binary file name to load, optional: delay between cycles (default 15ms)" << endl;
        return 0;
    }

    string fileName(argv[1]);

    bool success = loadFileContent(fileName, memoryData, DATA_LENGTH);

    if (!success) {
        cout << "Could not read file " << fileName << endl;
        return 1;
    }

    uint64_t delay = 15; // 15ms delay ~60Hz

    if (argc == 3) {
        uint64_t value = atoi(argv[2]);
        if (value > 0) {
            delay = value;
        } else {
            cout << "Ignoring passed delay : " << value << endl;
        }
    }
    cout << "Using delay: " << (int)delay << " ms" << endl;

    initscr();
    noecho();
    cbreak();

    WINDOW* displayWindow = newwin(DISPLAY_HEIGHT + 3, DISPLAY_WIDTH + 3, 0, 0);
    WINDOW* keyboardWindow = newwin(5, 80, 0, DISPLAY_WIDTH +  10);
    WINDOW* cpuWindow = newwin((DISPLAY_HEIGHT + 3 - 10), 80, 10, DISPLAY_WIDTH +  10);
    
    // keyboardWindow is the only window with inputs
    keypad(keyboardWindow, TRUE);
    nodelay(keyboardWindow, TRUE);
    
    scrollok(displayWindow, TRUE);
    scrollok(keyboardWindow, TRUE);
    scrollok(cpuWindow, TRUE);

    Display* display = new Display(displayWindow);
    Keyboard* keyboard = new Keyboard(keyboardWindow);
    Chip8CPU* chip8Cpu = new Chip8CPU(cpuWindow, display, keyboard);
    
    wclear(cpuWindow); 
    wmove(cpuWindow, 1, 0);
    wrefresh(cpuWindow);
    wprintw(cpuWindow, "Starting Chip8 Emulator");
    wechochar(cpuWindow, '\n');

    chip8Cpu->loadMemory(memoryData, LENGTH(memoryData));

    display->draw();
    
    bool wasSuccessful = false;
    do {
        wasSuccessful = chip8Cpu->runCycle();
        wprintw(cpuWindow, "Emulating cycle: ");
        wprintw(cpuWindow, (wasSuccessful ? "success" : "failed"));
        wechochar(cpuWindow, '\n');
        display->draw();
        keyboard->readKey();
        sleep(delay);
    } while (wasSuccessful);
    
    wprintw(cpuWindow, "Emulation ended. Press key to exit.");
    wechochar(cpuWindow, '\n');

    wgetch(cpuWindow);
    
    delwin(cpuWindow);
    delwin(displayWindow);
    delwin(keyboardWindow);
    endwin();
    
    delete chip8Cpu;
    delete display;
    delete keyboard;

    return 0;
}