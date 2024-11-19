#include "interrupts.h"
#include <BasicRenderer.h>
#include <pic.h>
#include <io.h>

#define PANIC_COLOR COLOR(255, 55, 0)

void panic(const char* message)
{
    GlobalRenderer->clearScreen(BACKGROUND_COLOR);
    GlobalRenderer->setColor(PANIC_COLOR);
    GlobalRenderer->printf(message);
}

void __attribute__((interrupt)) PageFault_Handler(interrupt_frame* frame)
{
    (void)frame;

    panic("ERROR: Page fault #PF");

    while (true)
    {
        asm volatile("cli; hlt");
    }
}

void __attribute__((interrupt)) DoubleFaults_Handler(interrupt_frame* frame)
{
    panic("ERROR: Double faults");
    while (true)
    {
        asm volatile("cli; hlt");
    }
}

void __attribute__((interrupt)) GPFault_Handler(interrupt_frame* frame)
{
    panic("ERROR: General protection fault");

    while (true)
    {
        asm volatile("cli; hlt");
    }
}

void __attribute__((interrupt)) DividedBy0Fault_Handler(interrupt_frame* frame)
{
    panic("ERROR: Divided by 0");
    frame->rip++; // Move to the next instruction by increment the instruction ptr (ignore the divided by 0)
}

void __attribute__((interrupt)) Keyboard_Handler(interrupt_frame* frame)
{
    (void)frame;

    uint8_t scancode = x64_inb(0x60);
    GlobalRenderer->printf("Key pressed");

    PIC_SendEOI(1);
}

