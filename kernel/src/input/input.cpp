#include "input.h"
#include <keyboard/keyboard.h>

char getKey()
{
    KeyInfo* keyInfo = (KeyInfo*)KEY_INFO_ADDRESS;
    while (!keyInfo->key) asm volatile("hlt");

    char ascii = keyInfo->key;
    keyInfo->key = 0;

    return ascii;
}



