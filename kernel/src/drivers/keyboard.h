#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "PIC.h"
#include "../interrupts/ISR.h"
#include <stdbool.h>

extern int keybuf;

__attribute__((interrupt)) void kb_isr(int_frame_t* frame);


#endif
