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
#include <paging/PageTableManager.h>
#include <memory.h>
#include <gdt.h>

#include <idt.h>
#include <interrupts.h>
#include <pic.h>

#define PIC_REMAP_OFFSET 0x20
#define IRQ_OFFSET(offset) (PIC_REMAP_OFFSET + offset)

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

BasicRenderer renderer(NULL, NULL);

extern "C" void kmain(BootInfo* bootInfo)
{
    BasicRenderer renderer = BasicRenderer(bootInfo->framebuffer, bootInfo->font);
    GlobalRenderer = &renderer;
    GlobalRenderer->clearScreen(BACKGROUND_COLOR);

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mDescriptorSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    PageTableManager pageTableManager(PML4);

    // Identity mapping the entire system memory
    for (uint64_t i = 0; i < getMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mDescriptorSize); i += 0x1000)
    {
        pageTableManager.MapMemory((void*)i, (void*)i);
    }

    // Identity mapping the GOP framebuffer
    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
    for (uint64_t i = fbBase; i < fbBase + fbSize; i += 0x1000)
    {
        pageTableManager.MapMemory((void*)i, (void*)i);
    }

    asm volatile("mov %0, %%cr3" : : "r"(PML4)); // Pass the PML4 to the CPU

    GlobalRenderer->printf("Paging initialized!\n");

    GDT_Initialize();
    GlobalRenderer->printf("GDT initialized!\n");

    IDTDescriptor idtr;
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    IDT_SetGate(0x0E, (void*)PageFault_Handler, 0x08, IDT_TA_InterruptGate, idtr);
    IDT_SetGate(0x08, (void*)DoubleFaults_Handler, 0x08, IDT_TA_InterruptGate, idtr);
    IDT_SetGate(0xD, (void*)GPFault_Handler, 0x08, IDT_TA_InterruptGate, idtr);
    IDT_SetGate(0x00, (void*)DividedBy0Fault_Handler, 0x08, IDT_TA_InterruptGate, idtr);
    
    IDT_SetGate(IRQ_OFFSET(1), (void*)Keyboard_Handler, 0x08, IDT_TA_InterruptGate, idtr);

    asm volatile("lidt %0" : : "m"(idtr));

    PIC_Configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);
    PIC_Unmask(1);

    asm volatile("sti"); // Re-enable interrupt

    GlobalRenderer->printf("IDT initialized!\n");
    
    GlobalRenderer->printf("Hello World!\n");

    while (1)
    {
        __asm__ __volatile__("hlt");
    }
}
