#include "idt.h"

void IDTEntry::SetOffset(uint64_t offset)
{
    OffsetLow = (uint16_t)(offset & 0x000000000000ffff);
    OffsetMiddle = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    OffsetHigh = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTEntry::GetOffset()
{
    uint64_t offset = 0;
    offset |= (uint64_t)OffsetLow;
    offset |= (uint64_t)OffsetMiddle << 16;
    offset |= (uint64_t)OffsetHigh << 32;
    return offset;
}

void IDT_SetGate(int entryOffset, void* handler, uint16_t segmentDescriptor, uint8_t typeAttrib, IDTDescriptor idtr)
{
    IDTEntry* interrupt = (IDTEntry*)(idtr.Offset + entryOffset * sizeof(IDTEntry));
    interrupt->SetOffset((uint64_t)handler);
    interrupt->TypeAttrib = typeAttrib;
    interrupt->SegmentSelector = segmentDescriptor;
}
