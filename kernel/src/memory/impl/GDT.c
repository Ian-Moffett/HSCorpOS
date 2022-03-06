#include "../GDT.h"


gdt_t gdt = {
    {0, 0, 0, 0x00, 0x00, 0},   // Null.
    {0, 0, 0, 0x9a, 0xa0, 0},   // Kernel data.
    {0, 0, 0, 0x92, 0xa0, 0},   // Kernel code.
    {0, 0, 0, 0x00, 0x00, 0},   // User null.
    {0, 0, 0, 0x9a, 0xa0, 0},   // User data.
    {0, 0, 0, 0x92, 0xa0, 0},   // User code.
};
