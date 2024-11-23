#pragma once
#include <stdint.h>
#include <stdbool.h>

/*
    Selecting device
    master = 0
    slave = 1
*/
void ATA_SelectDevice(uint8_t device);

bool ATA_ReadSectors(uint32_t lba, uint32_t sectorsCount, void* bufferOut);
