#ifndef PIC_H
#define PIC_H

#define PIC1_COMMAND 0x20       // Master PIC.
#define PIC1_DATA 0x21          // Master data line.
#define PIC2_COMMAND 0xA0       // Slave PIC.
#define PIC2_DATA 0xA1        // Slave data line.

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01
#define PIC_EOI 0x20

#include "IO.h"

void pic_sendEOI(unsigned char irq);


#endif
