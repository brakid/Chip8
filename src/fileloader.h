#ifndef FILELOADER
#define FILELOADER

#include <iostream>
#include <fstream>
#include "cpu.h"

using namespace std;

bool loadFileContent(const string& fileName, uint8_t* memoryData, int memoryLength);

#endif