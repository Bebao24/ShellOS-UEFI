#include "interrupts.h"
#include <BasicRenderer.h>

void __attribute__((interrupt)) PageFault_Handler(interrupt_frame* frame)
{
    GlobalRenderer->clearScreen(BACKGROUND_COLOR);
    GlobalRenderer->setColor(COLOR(255, 55, 0));
    GlobalRenderer->printf("ERROR: Page fault!\n");
    while (true)
    {
        asm volatile("cli; hlt");
    }
}

