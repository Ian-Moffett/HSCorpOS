#ifndef MOUSE_H
#define MOUSE_H


#include "IO.h"
#include "../interrupts/IDT.h"
#include <stdint.h>

void mouse_init();
__attribute__((interrupt)) void mouse_isr(int_frame_t*);

#endif
