#pragma one
#include <stdint.h>

struct EFI_MEMORY_DESCRIPTOR
{
    uint32_t type;
    void* physicalAddr;
    void* virtualAddr;
    uint64_t numPages;
    uint64_t attributes;
};


