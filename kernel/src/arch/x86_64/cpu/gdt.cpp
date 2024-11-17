#include "gdt.h"

__attribute__((aligned(0x1000)))
GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // NULL entry
    {0, 0, 0, 0x9a, 0xa0, 0}, // Kernel Code
    {0, 0, 0, 0x92, 0xa0, 0}, // Kernel data
    {0, 0, 0, 0x00, 0x00, 0}, // User NULL
    {0, 0, 0, 0xFA, 0xa0, 0}, // User Code
    {0, 0, 0, 0xF2, 0xa0, 0} // User Data
};

extern "C" void LoadGDT(GDTDescriptor* descriptor);

void GDT_Initialize()
{
    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Limit = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);
}

