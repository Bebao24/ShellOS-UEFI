#include "BasicRenderer.h"
#include <stdarg.h>
#include <stdbool.h>

BasicRenderer::BasicRenderer(GOP_Framebuffer_t* framebuffer, PSF1_FONT* font)
    :fb(framebuffer), font(font)
{
    this->cursorPos = { 0, 0 };
    this->textColor = FOREGROUND_COLOR;
}

void BasicRenderer::putPixel(uint32_t x, uint32_t y, uint32_t color)
{
    *(uint32_t*)((uint64_t)fb->BaseAddress + (x * 4) + (y * fb->PixelsPerScanLine * 4)) = color;
}

void BasicRenderer::drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (uint32_t yy = 0; yy < h; yy++)
    {
        for (uint32_t xx = 0; xx < w; xx++)
        {
            this->putPixel(x + xx, y + yy, color);
        }
    }
}

void BasicRenderer::clearScreen(uint32_t color)
{
    // Won't use putPixel() because of performace issue
    for (uint32_t y = 0; y < fb->height; y++)
    {
        uint64_t pixelPtrBase = (uint64_t)fb->BaseAddress + ((fb->PixelsPerScanLine * 4) * y);
        for (uint32_t* pixelPtr = (uint32_t*)pixelPtrBase; pixelPtr < (uint32_t*)(pixelPtrBase + (fb->PixelsPerScanLine * 4)); pixelPtr++)
        {
            *pixelPtr = color;
        }
    }
}

void BasicRenderer::drawChar(uint32_t x, uint32_t y, uint32_t color, char c)
{
    uint32_t* fbAddr = (uint32_t*)fb->BaseAddress;
    char* fontPtr = (char*)font->glyphBuffer + (c * font->fontHeader->charSize);
    for (uint32_t yy = y; yy < y + 16; yy++)
    {
        for (uint32_t xx = x; xx < x + 8; xx++)
        {
            if ((*fontPtr & (0b10000000 >> (xx - x))) > 0)
            {
                *(uint32_t*)(fbAddr + xx + (yy * fb->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void BasicRenderer::putc(char c)
{
    switch (c)
    {
        case '\r':
            cursorPos.x = 0;
            break;
        case '\n':
            cursorPos.x = 0;
            cursorPos.y += 16;
            break;
        case '\t':
            for (uint8_t i = 0; i < 4 - (cursorPos.x % 4); i++)
            {
                putc(' ');
            }
            break;
        default:
            drawChar(cursorPos.x, cursorPos.y, this->textColor, c);
            cursorPos.x += 8;
            break;
    }

    if (cursorPos.x + 8 > fb->width)
    {
        // New line
        cursorPos.x = 0;
        cursorPos.y += 16;
    }

    if (cursorPos.y + 16 > fb->height)
    {
        // TODO: Implement scrollback() function
        // Just clear the screen for now
        clearScreen(BACKGROUND_COLOR);
    }
}

void BasicRenderer::puts(const char* string)
{
    while (*string)
    {
        putc(*string);
        string++;
    }
}

const char g_HexChars[] = "0123456789abcdef";

void BasicRenderer::printf_unsigned(unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        putc(buffer[pos]);
}

void BasicRenderer::printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        putc('-');
        printf_unsigned(-number, radix);
    }
    else printf_unsigned(number, radix);
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

void BasicRenderer::printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   putc((char)va_arg(args, int));
                                break;

                    case 's':   
                                puts(va_arg(args, const char*));
                                break;

                    case '%':   putc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_signed(va_arg(args, int), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG:        printf_signed(va_arg(args, long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_signed(va_arg(args, long long), radix);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_unsigned(va_arg(args, unsigned int), radix);
                                                        break;
                                                        
                        case PRINTF_LENGTH_LONG:        printf_unsigned(va_arg(args, unsigned  long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_unsigned(va_arg(args, unsigned  long long), radix);
                                                        break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                break;
        }

        fmt++;
    }

    va_end(args);
}



