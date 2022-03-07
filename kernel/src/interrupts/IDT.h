#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "../drivers/PIC.h"

#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define INT_GATE_USER_FLAGS 0xEE 

typedef struct {
    uint16_t isr_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attr;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed)) idt_desc_t;


typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;


typedef struct {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) int_frame_t; 

void set_idt_entry(unsigned char entry, void* isr, unsigned char flags);
void idt_install();


#endif
