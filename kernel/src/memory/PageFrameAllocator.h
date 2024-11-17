#pragma once
#include <efiMemory.h>
#include <stdint.h>
#include <bitmap.h>

class PageFrameAllocator
{
public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapSize, uint64_t mDescriptorSize);

    void FreePage(void* address);
    void LockPage(void* address);
    void ReservedPage(void* address);
    void UnreservedPage(void* address);

    void FreePages(void* address, uint64_t pagesCount);
    void LockPages(void* address, uint64_t pagesCount);
    void ReservedPages(void* address, uint64_t pagesCount);
    void UnreservedPages(void* address, uint64_t pagesCount);

    void* RequestPage();

    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();

private:
    Bitmap pageBitmap;

    void InitializeBitmap(void* bitmapBuffer, size_t bufferSize);
};

extern PageFrameAllocator GlobalAllocator;

