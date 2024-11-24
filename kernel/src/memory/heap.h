#pragma once
#include <stdint.h>
#include <stddef.h>

struct HeapSegHeader
{
    size_t size;
    HeapSegHeader* next;
    HeapSegHeader* last;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHeader* Split(size_t splitSize);
};

void InitializeHeap(void* heapAddress, size_t pageCount);

void* malloc(size_t size);
void free(void* address);

void ExpandHeap(size_t size);

