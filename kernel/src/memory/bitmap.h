#pragma once
#include <stdint.h>
#include <stddef.h>

class Bitmap
{
public:
    bool operator[](uint64_t index);
    bool Set(uint64_t index, bool value);

    uint8_t* bitmapBuffer;
    size_t bufferSize;

private:
    bool Get(uint64_t index);
};


