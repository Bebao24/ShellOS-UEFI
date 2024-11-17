#pragma once

struct interrupt_frame;
void __attribute__((interrupt)) PageFault_Handler(interrupt_frame* frame);


