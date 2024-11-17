#pragma once
#include <stdint.h>

struct PageDirectoryEntry
{
    bool Present : 1;
    bool ReadWrite : 1;
    bool Supervisor : 1;
    bool WriteThrough : 1;
    bool CacheDisabled : 1;
    bool Accessed : 1;
    bool Reserved0 : 1;
    bool LargerPages : 1;
    bool Reserved1 : 1;
    uint8_t Avaliable : 3;
    uint64_t Address : 52;
};

struct PageTable
{
    PageDirectoryEntry entries[512];
} __attribute__((aligned(0x1000)));

