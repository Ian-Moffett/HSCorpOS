#ifndef MOUSE_H
#define MOUSE_H

#define PS2XSign        0b00010000     // So we can get X signed bit.
#define PS2YSign        0b00100000
#define PS2XOverflow    0b01000000
#define PS2YOverflow    0b10000000
#define PS2LButton      0b00000001
#define PS2MButton      0b00000010
#define PS2RButton      0b00000100


#include "IO.h"
#include "FrameBuffer.h"
#include "../interrupts/IDT.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    long x;
    long y;
} point_t;


void mouse_init();
__attribute__((interrupt)) void mouse_isr(int_frame_t*);

#endif
