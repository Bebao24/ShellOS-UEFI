/*
    kernel.cpp: Entry point from the bootloader
*/

#include <stdint.h>
#include <stddef.h>
#include <framebuffer.h>
#include <font.h>
#include <BasicRenderer.h>
#include <memory.h>
#include <bitmap.h>
#include <cstring>

struct BootInfo
{
    GOP_Framebuffer_t* framebuffer;
    PSF1_FONT* font;
    EFI_MEMORY_DESCRIPTOR* mMap;
    uint64_t mMapSize; // UINTN is uint64_t
    uint64_t mDescriptorSize;
};

extern "C" void kmain(BootInfo* bootInfo)
{
    BasicRenderer renderer(bootInfo->framebuffer, bootInfo->font);
    renderer.clearScreen(BACKGROUND_COLOR);

    renderer.printf("Hello World from printf!\n");
    renderer.printf("Test: 0x%x\n", 0x123);

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;

    uint8_t buffer[20];
    memset(buffer, 0, sizeof(buffer));

    Bitmap testBitmap(buffer, sizeof(buffer)); 
    testBitmap.Set(0, true);
    testBitmap.Set(2, true);
    testBitmap.Set(4, true);
    testBitmap.Set(6, true);
    testBitmap.Set(8, true);

    for (int i = 0; i < 20; i++)
    {
        renderer.printf(testBitmap[i] ? "true" : "false");
        renderer.putc('\n');
    }

    while (1)
    {
        __asm__ __volatile__("hlt");
    }
}
