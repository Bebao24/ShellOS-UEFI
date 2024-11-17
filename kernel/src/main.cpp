/*
    kernel.cpp: Entry point from the bootloader
*/

#include <stdint.h>
#include <stddef.h>
#include <framebuffer.h>
#include <font.h>
#include <BasicRenderer.h>
#include <memory.h>

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

    uint64_t totalMemory = getMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mDescriptorSize) / 1024 / 1024;
    renderer.printf("Total memory size: %llu MB\n", totalMemory);

    while (1)
    {
        __asm__ __volatile__("hlt");
    }
}
