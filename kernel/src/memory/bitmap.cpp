#include "bitmap.h"

bool Bitmap::operator[](uint64_t index)
{
    return this->Get(index);
}

bool Bitmap::Get(uint64_t index)
{
    if (index > this->bufferSize * 8) return false;
    
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8; // The remainder
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    if ((this->bitmapBuffer[byteIndex] & bitIndexer) > 0)
    {
        return true;
    }
    return false;
}

bool Bitmap::Set(uint64_t index, bool value)
{
    if (index > this->bufferSize * 8) return false;
    
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8; // The remainder
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    this->bitmapBuffer[byteIndex] &= ~(bitIndexer); // Unset that bit
    if (value)
    {
        // If value == true, set that bit
        this->bitmapBuffer[byteIndex] |= bitIndexer;
    }

    return true;
}



