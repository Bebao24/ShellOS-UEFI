#include "heap.h"
#include "PageFrameAllocator.h"
#include <paging/PageTableManager.h>
#include <BasicRenderer.h>

void* heapStart;
void* heapEnd;
HeapSegHeader* lastSeg;

void InitializeHeap(void* heapAddress, size_t pageCount)
{
    void* pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++)
    {
        g_PageTableManager.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void*)((size_t)pos + 0x1000); // Increment the position by 4096
    }

    size_t heapLength = pageCount * 0x1000;

    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapSegHeader* startSeg = (HeapSegHeader*)heapAddress;
    startSeg->size = heapLength - sizeof(HeapSegHeader);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    lastSeg = startSeg;
}

void* malloc(size_t size)
{
    // Road the size to a multiple of 0x10 (128 bits)
    if (size % 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;

    HeapSegHeader* currentSeg = (HeapSegHeader*)heapStart;


    while (true)
    {
        if (currentSeg->free)
        {
            if (currentSeg->size > size)
            {
                currentSeg->Split(size); // We need to split the segment so that it won't give too much memory
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegHeader));
            }
            if (currentSeg->size == size)
            {
                // No need to split the segment
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegHeader));
            }
        }
        if (currentSeg->next == NULL) break; // Should expand the heap size
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return malloc(size); // Recursively call malloc()
}

void free(void* address)
{
    HeapSegHeader* segment = (HeapSegHeader*)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

void ExpandHeap(size_t size)
{
    // Road up to a multiple of 0x1000 (4096)
    if (size % 0x1000 > 0)
    {
        size -= (size % 0x1000);
        size += 0x1000;
    }

    size_t pageCount = size / 0x1000;
    HeapSegHeader* newSeg = (HeapSegHeader*)heapEnd;

    for (size_t i = 0; i < pageCount; i++)
    {
        g_PageTableManager.MapMemory(heapEnd, GlobalAllocator.RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000); // Increment heap end by 0x1000
    }

    newSeg->free = true;
    newSeg->last = lastSeg;
    lastSeg->next = newSeg;
    lastSeg = newSeg;
    newSeg->next = NULL;
    newSeg->size = size - sizeof(HeapSegHeader);
    newSeg->CombineBackward();
}

void HeapSegHeader::CombineForward()
{
    if (this->next == NULL) return;
    if (!this->next->free) return;

    if (this->next == lastSeg) lastSeg = this;

    if (this->next->next != NULL)
    {
        this->next->next->last = this;
    }

    this->size = this->size + this->next->size + sizeof(HeapSegHeader);
    this->next = this->next->next;
}

void HeapSegHeader::CombineBackward()
{
    if (this->last != NULL && this->last->free) this->last->CombineForward();
}

HeapSegHeader* HeapSegHeader::Split(size_t splitSize)
{
    if (splitSize < 0x10) return NULL;
    int64_t splitSegSize = this->size - splitSize - sizeof(HeapSegHeader);
    if (splitSegSize < 0x10) return NULL;

    HeapSegHeader* newSplitSeg = (HeapSegHeader*)((size_t)this + splitSize + sizeof(HeapSegHeader));
    this->next->last = newSplitSeg;
    newSplitSeg->next = this->next;
    this->next = newSplitSeg;
    newSplitSeg->last = this;
    newSplitSeg->size = splitSegSize;
    newSplitSeg->free = true;
    this->size = splitSize;

    if (lastSeg == this) lastSeg = newSplitSeg;

    return newSplitSeg;
}
