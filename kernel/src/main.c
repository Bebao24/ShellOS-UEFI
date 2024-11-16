#include <stdint.h>
#include <stddef.h>

typedef struct
{
    void* BaseAddress;
    size_t BufferSize;
    uint32_t width;
    uint32_t height;
    uint32_t PixelsPerScanLine;
} GOP_Framebuffer_t;

typedef struct
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charSize;
} PSF1_HEADER;

typedef struct
{
    PSF1_HEADER* fontHeader;
    void* glyphBuffer;
} PSF1_FONT;
typedef struct
{
    GOP_Framebuffer_t* framebuffer;
    PSF1_FONT* font;
} BootInfo;

void putChar(GOP_Framebuffer_t* framebuffer, PSF1_FONT* font, uint32_t color, uint32_t x, uint32_t y, char c)
{
    uint32_t* fb = (uint32_t*)framebuffer->BaseAddress;
    char* fontPtr = font->glyphBuffer + (c * font->fontHeader->charSize);
    for (uint32_t yy = y; yy < y + 16; yy++)
    {
        for (uint32_t xx = x; xx < x + 8; xx++)
        {
            if ((*fontPtr & (0b10000000 >> (xx - x))) > 0)
            {
                *(uint32_t*)(fb + xx + (yy * framebuffer->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void kmain(BootInfo* bootInfo)
{
    // Draw something to the screen
    putChar(bootInfo->framebuffer, bootInfo->font, 0xFFFFFFFF, 10, 10, 'H');

    while (1)
    {
        __asm__ __volatile__("hlt");
    }
}
