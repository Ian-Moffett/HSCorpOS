#include "../IDT.h"

static idt_desc_t idt[256];
static idtr_t idtr;


void set_idt_entry(unsigned char entry, void* isr, unsigned char flags) {
    uint64_t addr = (uint64_t)isr;
    idt[entry].isr_addr_low = addr & 0xFFFF;                         // Grabs the low bits.
    idt[entry].isr_addr_middle = (addr & 0xFFFF0000) >> 16;          // Grabs the middle bits.
    idt[entry].isr_addr_high = (addr & 0xFFFFFFFF00000000) >> 32;    // Grabs the higher bits.
    idt[entry].dpl = 0;                                              // Ring 0 for now, we may have to change this later.
    idt[entry].p = 1;                                                // It is present.
    idt[entry].attr = flags;                                    
    idt[entry].selector = 0x08;
    idt[entry].reserved = 0x0;
    idt[entry].reserved2 = 0x0;
}


void idt_install() {
    idtr.limit = (unsigned char)(sizeof(idt));
    idtr.base = (uint64_t)&idt;
    __asm__ __volatile__("lidt %0" : : "memory" (idtr));
}
