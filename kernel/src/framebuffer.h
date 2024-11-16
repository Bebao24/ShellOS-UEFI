#pragma once
#include <stdint.h>
#include <stddef.h>

struct GOP_Framebuffer_t
{
    void* BaseAddress;
    size_t BufferSize;
    uint32_t width;
    uint32_t height;
    uint32_t PixelsPerScanLine;
};

