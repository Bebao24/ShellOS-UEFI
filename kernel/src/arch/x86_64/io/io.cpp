#include "io.h"

void x64_outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t x64_inb(uint16_t port)
{
    uint8_t returnValue;
    asm volatile("inb %1, %0" : "=a"(returnValue) : "Nd"(port));
    return returnValue;
}

void x64_outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t x64_inw(uint16_t port)
{
    uint16_t returnValue;
    asm volatile("inw %1, %0" : "=a"(returnValue) : "Nd"(port));
    return returnValue;
}

void x64_iowait()
{
    x64_outb(0x80, 0);
}


