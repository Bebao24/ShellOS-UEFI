#include "paging.h"

void PageDirectoryEntry::SetFlag(PT_Flag flag, bool enable)
{
    uint64_t bitIndexer = (uint64_t)1 << flag;
    value &= ~(bitIndexer);

    if (enable)
    {
        value |= bitIndexer;
    }
}

bool PageDirectoryEntry::GetFlag(PT_Flag flag)
{
    uint64_t bitIndexer = (uint64_t)1 << flag;
    return value & bitIndexer > 0 ? true : false;
}

void PageDirectoryEntry::SetAddress(uint64_t address)
{
    address &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (address << 12);
}

uint64_t PageDirectoryEntry::GetAddress()
{
    return (value & 0x000ffffffffff000) >> 12;
}

