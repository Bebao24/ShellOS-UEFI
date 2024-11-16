/*
    kernel.cpp: Entry point from the bootloader
*/

#include <stdint.h>
#include <stddef.h>
#include <framebuffer.h>
#include <font.h>
#include <BasicRenderer.h>

struct BootInfo
{
    GOP_Framebuffer_t* framebuffer;
    PSF1_FONT* font;
};

extern "C" void kmain(BootInfo* bootInfo)
{
    BasicRenderer renderer(bootInfo->framebuffer, bootInfo->font);
    renderer.clearScreen(BACKGROUND_COLOR);

    renderer.printf("Hello World from printf!\n");
    renderer.printf("Test: 0x%x\n", 0x123);

    while (1)
    {
        __asm__ __volatile__("hlt");
    }
}
