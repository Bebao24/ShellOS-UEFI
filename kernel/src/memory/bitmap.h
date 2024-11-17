#pragma once
#include <stdint.h>
#include <stddef.h>

class Bitmap
{
public:
    Bitmap(uint8_t* bitmapBuffer, size_t bufferSize);
    bool operator[](uint64_t index);
    bool Set(uint64_t index, bool value);

private:
    bool Get(uint64_t index);

    uint8_t* bitmapBuffer;
    size_t bufferSize;
};


