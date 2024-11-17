#pragma once
#include <stdint.h>
#include <efiMemory.h>

uint64_t getMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mDescriptorSize);


