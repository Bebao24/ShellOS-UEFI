#pragma once
#include <stdint.h>

struct PSF1_HEADER
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charSize;
};

struct PSF1_FONT
{
    PSF1_HEADER* fontHeader;
    void* glyphBuffer;
};

