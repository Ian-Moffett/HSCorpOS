#include "../IDT.h"

static idt_desc_t idt[256];
static idtr_t idtr;

static __attribute__((interrupt)) void isr_stub(int_frame_t*) {}


void set_idt_entry(unsigned char entry, void* isr, unsigned char flags) { 
    uint64_t addr = (uint64_t)isr;
    idt[entry].isr_low = addr & 0xFFFF;
    idt[entry].isr_high = (addr >> 32) & 0xFFFFFFFF;
    idt[entry].isr_mid = (addr >> 16) & 0xFFFF;
    idt[entry].selector = 0x08;
    idt[entry].reserved = 0x0;
    idt[entry].attr = flags;
    idt[entry].ist = 0;
}


void idt_install() {
    uint8_t master_bm, slave_bm;

    // Saving master & slave bitmask.

    master_bm = inportb(PIC1_DATA);
    io_wait();
    slave_bm = inportb(PIC2_DATA);
    io_wait();

    // Initalizing master PIC.

    outportb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // Initalizing slave PIC.

    outportb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // Remap PICs.
    outportb(PIC1_DATA, 0x20);
    io_wait();
    outportb(PIC2_DATA, 0x28);
    io_wait();

    // Tell PICs about each others existance.

    outportb(PIC1_DATA, 4);
    io_wait();
    outportb(PIC2_DATA, 2);
    io_wait();

    // Tell them to operate in 8086 mode.
    outportb(PIC1_DATA, ICW4_8086);
    io_wait();
    outportb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore bitmaks.
    outportb(PIC1_DATA, master_bm);
    io_wait();
    outportb(PIC2_DATA, slave_bm);

    // Setup IDTR.
    idtr.limit = (uint16_t)sizeof(idt_desc_t) * 255;
    idtr.base = (uint64_t)&idt[0];

    for (int i = 0; i < 32; ++i) {
        // set_idt_entry(i, isr_stub, INT_GATE_FLAGS);
    }

    __asm__ __volatile__("lidt %0" : : "memory" (idtr));
}
