#include "../PIC.h"

void pic_sendEOI(unsigned char irq) {
    if (irq >= 8) {
        outportb(PIC2_COMMAND, PIC_EOI);
    }

    outportb(PIC1_COMMAND, PIC_EOI);
}
