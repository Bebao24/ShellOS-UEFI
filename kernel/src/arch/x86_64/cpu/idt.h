#pragma once
#include <stdint.h>

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

struct IDTEntry
{
    uint16_t OffsetLow;
    uint16_t SegmentSelector;
    uint8_t ist;
    uint8_t TypeAttrib;
    uint16_t OffsetMiddle;
    uint32_t OffsetHigh;
    uint32_t Reserved;

    void SetOffset(uint64_t offset);
    uint64_t GetOffset();
};

struct IDTDescriptor
{
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed));

void IDT_SetGate(int entryOffset, void* handler, uint16_t segmentDescriptor, uint8_t typeAttrib, IDTDescriptor idtr);