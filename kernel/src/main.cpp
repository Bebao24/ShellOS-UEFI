/*
    kernel.cpp: Entry point from the bootloader
*/

#include <stdint.h>
#include <stddef.h>
#include <framebuffer.h>
#include <font.h>
#include <BasicRenderer.h>
#include <cstring>
#include <PageFrameAllocator.h>

struct BootInfo
{
    GOP_Framebuffer_t* framebuffer;
    PSF1_FONT* font;
    EFI_MEMORY_DESCRIPTOR* mMap;
    uint64_t mMapSize; // UINTN is uint64_t
    uint64_t mDescriptorSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void kmain(BootInfo* bootInfo)
{
    BasicRenderer renderer(bootInfo->framebuffer, bootInfo->font);
    renderer.clearScreen(BACKGROUND_COLOR);

    renderer.printf("Hello World from printf!\n");
    renderer.printf("Test: 0x%x\n", 0x123);

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;

    PageFrameAllocator allocator;
    allocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mDescriptorSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    allocator.LockPages(&_KernelStart, kernelPages);

    void* address = allocator.RequestPage();
    renderer.printf("Address: 0x%x", address);

    while (1)
    {
        __asm__ __volatile__("hlt");
    }
}
