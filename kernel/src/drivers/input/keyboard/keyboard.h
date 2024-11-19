#pragma once
#include <stdint.h>
#include <stdbool.h>

#define KEY_INFO_ADDRESS 0x1600

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39

struct KeyInfo
{
    char key;
    bool shiftOn;
    bool capsOn;
} __attribute__((packed));

void InitKeyboard();
char TranslateScancode(uint8_t scancode, bool uppercase);

void HandleKeyboard();


