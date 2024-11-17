#pragma once
#include <maths.h>
#include <framebuffer.h>
#include <font.h>

#define COLOR(r, g, b) ((r << 16 ) | (g << 8) | (b))

#define BACKGROUND_COLOR COLOR(0, 0, 255)
#define FOREGROUND_COLOR COLOR(255, 255, 255)

class BasicRenderer
{
public:
    BasicRenderer(GOP_Framebuffer_t* framebuffer, PSF1_FONT* font);
    void putPixel(uint32_t x, uint32_t y, uint32_t color);
    void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
    void clearScreen(uint32_t color);

    void drawChar(uint32_t x, uint32_t y, uint32_t color, char c);

    void setColor(uint32_t color);
    
    void putc(char c);
    void puts(const char* string);
    void printf(const char* fmt, ...);

private:
    Vector2 cursorPos;

    GOP_Framebuffer_t* fb;
    PSF1_FONT* font;
    uint32_t textColor;

    void printf_unsigned(unsigned long long number, int radix);
    void printf_signed(long long number, int radix);
};

extern BasicRenderer* GlobalRenderer;

