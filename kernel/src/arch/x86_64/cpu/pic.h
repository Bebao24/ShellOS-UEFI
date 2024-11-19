#pragma once
#include <stdint.h>

void PIC_Configure(uint8_t offsetPIC1, uint8_t offsetPIC2);
void PIC_SendEOI(int irq);
void PIC_Disable();
void PIC_Mask(int irq);
void PIC_Unmask(int irq);



