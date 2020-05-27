#include "fileloader.h"

bool loadFileContent(const string& fileName, uint8_t* memoryData, int memoryLength) {
    cout << "Reading file " << fileName << endl;

    ifstream fileStream(fileName, ios::in | ios::binary);

    if (fileStream.is_open()) {
        int index = 0;
        while (fileStream.good() && index < memoryLength) {
            memoryData[index] = fileStream.get();
            index++;
        }
        cout << "Read " << (index + 1) << " bytes" << endl;
        return true;
    } else {
        cout << "Error opening file " << fileName << endl;
        return false;
    }
}