#include "cstring"
#include <stdint.h>

void* memcpy(void* dst, const void* src, size_t size)
{
    uint8_t* u8Dst = (uint8_t*)dst;
    const uint8_t* u8Src = (const uint8_t*)src;

    for (size_t i = 0; i < size; i++)
    {
        u8Dst[i] = u8Src[i];
    }

    return dst;
}

void* memset(void* ptr, int value, size_t size)
{
    uint8_t* u8Ptr = (uint8_t*)ptr;

    for (size_t i = 0; i < size; i++)
    {
        u8Ptr[i] = value;
    }
    
    return ptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t size)
{
    const uint8_t* u8Ptr1 = (const uint8_t*)ptr1;
    const uint8_t* u8Ptr2 = (const uint8_t*)ptr2;

    for (size_t i = 0; i < size; i++)
    {
        if (u8Ptr1[i] != u8Ptr2[i])
        {
            return 1;
        }
    }
    
    return 0;
}

