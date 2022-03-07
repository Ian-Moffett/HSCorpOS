#ifndef ISR_H
#define ISR_H

#include "IDT.h"
#include "../drivers/FrameBuffer.h"

void display_details(unsigned int idt_idx, int_frame_t* frame);

// Interrupt attribute makes function use IRET.
__attribute__((interrupt)) void div0_handler(int_frame_t* frame);

#endif
