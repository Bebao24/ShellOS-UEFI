#pragma once
#include <stdint.h>

void x64_outb(uint16_t port, uint8_t value);
uint8_t x64_inb(uint16_t port);

void x64_outw(uint16_t port, uint16_t value);
uint16_t x64_inw(uint16_t port);

void x64_iowait();


