#include "../IDT.h"

static idt_desc_t idt[256];
static idtr_t idtr;

static __attribute__((interrupt)) void isr_stub(int_frame_t*) {}


void set_idt_entry(unsigned char entry, void* isr, unsigned char flags) {
    /*
    idt_desc_t* desc = &idt[entry];
    desc->isr_low = (uint64_t)isr & 0xFFFF;
    desc->selector = 0x08;
    desc->ist = 0;
    desc->attr = flags;
    desc->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
    desc->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    desc->reserved = 0;
    */

    uint64_t addr = (uint64_t)isr;
    idt[entry].isr_low = addr & 0xFFFF;
    idt[entry].isr_mid = (addr & 0xFFFF0000) >> 16;
    idt[entry].isr_high = (addr & 0xFFFFFFFF00000000) >> 32;
    idt[entry].attr = flags;
    idt[entry].selector = 0x08;
    idt[entry].reserved = 0x0;
    idt[entry].ist = 0;
}


void idt_install() {
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * 255;
    idtr.base = (uint64_t)&idt[0];

    for (int i = 0; i < 32; ++i) {
        // set_idt_entry(i, isr_stub, INT_GATE_FLAGS);
    }

    __asm__ __volatile__("lidt %0" : : "memory" (idtr));
}
