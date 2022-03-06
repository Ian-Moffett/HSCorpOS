#ifndef ISR_H
#define ISR_H

#include "IDT.h"
#include "../drivers/FrameBuffer.h"

// Interrupt attribute makes function use IRET.
__attribute__((interrupt)) void div_0_handler(int_frame_t* int_frame);


#endif
