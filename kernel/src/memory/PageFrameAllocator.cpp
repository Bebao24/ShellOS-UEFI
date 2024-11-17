#include "PageFrameAllocator.h"
#include <stdbool.h>
#include <memory.h>

#include <cstring>

static uint64_t freeMemory;
static uint64_t usedMemory;
static uint64_t reservedMemory;
static bool initialize = false;

PageFrameAllocator GlobalAllocator;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mDescriptorSize)
{
    if (initialize) return;

    initialize = true;

    uint64_t mMapEntries = mMapSize / mDescriptorSize;

    void* largestFreeMemorySegment = NULL;
    size_t largestFreeMemorySegmentSize = 0;

    for (uint64_t i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mDescriptorSize));

        if (descriptor->type == 7)
        {
            // This memory segment is avaliable
            if (descriptor->numPages * 4096 > largestFreeMemorySegmentSize)
            {
                largestFreeMemorySegment = descriptor->physicalAddr;
                largestFreeMemorySegmentSize = descriptor->numPages * 4096;
            }
        }
    }

    uint64_t memorySize = getMemorySize(mMap, mMapEntries, mDescriptorSize);
    freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;

    InitializeBitmap(largestFreeMemorySegment, bitmapSize);

    ReservedPages(0, memorySize / 4096 + 1);
    for (uint64_t i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mDescriptorSize));

        if (descriptor->type == 7)
        {
            // The memory segment is free, Unreserve it
            UnreservedPages(descriptor->physicalAddr, descriptor->numPages);
        }
    }

    ReservedPages(0, 0x100);
    LockPages(this->pageBitmap.bitmapBuffer, this->pageBitmap.bufferSize / 4096 + 1);
}

uint64_t pageBitmapIndex = 0;
void* PageFrameAllocator::RequestPage()
{
    for (; pageBitmapIndex < this->pageBitmap.bufferSize * 8; pageBitmapIndex++)
    {
        if (this->pageBitmap[pageBitmapIndex] == true) continue;
        
        LockPage((void*)(pageBitmapIndex * 4096));
        return (void*)(pageBitmapIndex * 4096);
    }

    return NULL; // We will be able to swap pages in the future
}


void PageFrameAllocator::InitializeBitmap(void* bitmapBuffer, size_t bufferSize)
{
    this->pageBitmap.bufferSize = bufferSize;
    this->pageBitmap.bitmapBuffer = (uint8_t*)bitmapBuffer;

    memset(bitmapBuffer, 0, bufferSize);
}

void PageFrameAllocator::FreePage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (this->pageBitmap[index] == false) return;

    this->pageBitmap.Set(index, false);
    freeMemory += 4096;
    usedMemory -= 4096;

    if (pageBitmapIndex > index)
    {
        pageBitmapIndex = index;
    }
}

void PageFrameAllocator::LockPage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (this->pageBitmap[index] == true) return;

    this->pageBitmap.Set(index, true);
    freeMemory -= 4096;
    usedMemory += 4096;
}

void PageFrameAllocator::ReservedPage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (this->pageBitmap[index] == true) return;

    this->pageBitmap.Set(index, true);
    freeMemory -= 4096;
    reservedMemory += 4096;
}

void PageFrameAllocator::UnreservedPage(void* address)
{
    uint64_t index = (uint64_t)address / 4096;
    if (this->pageBitmap[index] == false) return;

    this->pageBitmap.Set(index, false);
    freeMemory += 4096;
    reservedMemory -= 4096;

    if (pageBitmapIndex > index)
    {
        pageBitmapIndex = index;
    }
}

void PageFrameAllocator::FreePages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        FreePage((void*)((uint64_t)address + (i * 4096)));
    }
}

void PageFrameAllocator::LockPages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        LockPage((void*)((uint64_t)address + (i * 4096)));
    }
}

void PageFrameAllocator::ReservedPages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        ReservedPage((void*)((uint64_t)address + (i * 4096)));
    }
}

void PageFrameAllocator::UnreservedPages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        UnreservedPage((void*)((uint64_t)address + (i * 4096)));
    }
}

uint64_t PageFrameAllocator::GetFreeRAM()
{
    return freeMemory;
}

uint64_t PageFrameAllocator::GetUsedRAM()
{
    return usedMemory;
}

uint64_t PageFrameAllocator::GetReservedRAM()
{
    return reservedMemory;
}

