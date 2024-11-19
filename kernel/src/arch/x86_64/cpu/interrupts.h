#pragma once
#include <stdint.h>

struct interrupt_frame
{
    uint64_t rsp;   // Stack pointer at the time of the interrupt
    uint64_t rflags; // Flags register (RFLAGS)
    uint64_t cs;     // Code segment register
    uint64_t rip;    // Instruction pointer at the time of the interrupt
};

void __attribute__((interrupt)) PageFault_Handler(interrupt_frame* frame);
void __attribute__((interrupt)) DoubleFaults_Handler(interrupt_frame* frame);
void __attribute__((interrupt)) GPFault_Handler(interrupt_frame* frame);
void __attribute__((interrupt)) DividedBy0Fault_Handler(interrupt_frame* frame);

void __attribute__((interrupt)) Keyboard_Handler(interrupt_frame* frame);


