#include "ata.h"
#include <io.h>

#define ATA_PRIMARY_BASE       0x1F0
#define ATA_STATUS_REGISTER    (ATA_PRIMARY_BASE + 0x07)
#define ATA_CMD_REGISTER       (ATA_PRIMARY_BASE + 0x07)
#define ATA_DATA_REGISTER      ATA_PRIMARY_BASE
#define ATA_ERROR_REGISTER     (ATA_PRIMARY_BASE + 0x01)

enum ATA_CMD
{
    ATA_CMD_READ = 0x20,
    ATA_CMD_WRITE = 0x30,
};

/*
    Selecting device
    master = 0
    slave = 1
*/
void ATA_SelectDevice(uint8_t device)
{
    x64_outb(ATA_PRIMARY_BASE + 6, 0xA0 | (device << 4));
}

void ATA_Wait()
{
    while (x64_inb(ATA_STATUS_REGISTER) & 0x80); // Wait until the BSY bit is clear
}

void ATA_WaitDRQ()
{
    while(!(x64_inb(ATA_STATUS_REGISTER) & 0x08)); // Wait until the DRQ bit is set
}

/*
    ATA_CheckError() will return a error code
*/
int ATA_CheckError()
{
    uint8_t status = x64_inb(ATA_STATUS_REGISTER);

    // Check if an error has occur by checking the ERR bit
    if (status & 0x01)
    {
        uint8_t errorCode = x64_inb(ATA_ERROR_REGISTER);

        // Check for difference error
        if (errorCode & 0x20) {
            // ABRT - Command aborted
            return -1;
        }
        if (errorCode & 0x10) {
            // MCR - Media Change Request
            return -2;
        }
        if (errorCode & 0x04) {
            // IDNF - ID not found (e.g., no drive)
            return -3;
        }
        if (errorCode & 0x02) {
            // UNC - Uncorrectable Data error
            return -4;
        }
        if (errorCode & 0x01) {
            // ECC - Error Corrected (PIO data error)
            return -5;
        }
    }
    return 0;
}

bool ATA_ReadSectors(uint32_t lba, uint32_t sectorsCount, void* bufferOut)
{
    // Convert LBA to CHS
    uint16_t cylinder_high = (lba >> 24) & 0xFF;
    uint16_t cylinder_low = (lba >> 16) & 0xFF;
    uint8_t head = (lba >> 8) & 0x0F;
    uint8_t sector = (lba & 0xFF) ? (lba & 0xFF) : 1;

    // Output the information
    x64_outb(ATA_PRIMARY_BASE + 2, sectorsCount); // 0x1F2 is the sectors count port
    x64_outb(ATA_PRIMARY_BASE + 3, sector); // 0x1F3 is the sector port
    x64_outb(ATA_PRIMARY_BASE + 4, head); // 0x1F4 is the head port
    x64_outb(ATA_PRIMARY_BASE + 5, cylinder_low); // 0x1F5 is the cylinder low port
    x64_outb(ATA_PRIMARY_BASE + 6, cylinder_high); // 0x1F6 is the cylinder high port

    // Send the read command
    x64_outb(ATA_CMD_REGISTER, ATA_CMD_READ);

    ATA_Wait();

    // Check for errors
    int errorCode = ATA_CheckError();
    if (errorCode != 0)
    {
        // An error has occured
        return false;
    }

    ATA_WaitDRQ();

    // Read the data from the data register
    for (uint32_t i = 0; i < sectorsCount * 256; i++)
    {
        ((uint16_t*)bufferOut)[i] = x64_inw(ATA_DATA_REGISTER);
    }

    return true;
}


