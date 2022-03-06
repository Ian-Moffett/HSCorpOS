#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef struct {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) gdt_desc_t;


typedef struct {
    uint16_t limit0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access;
    uint8_t limit1;
    uint8_t flags;
} __attribute__((packed)) gdt_entry_t;


typedef struct {
    gdt_entry_t null;
    gdt_entry_t kernelcode;
    gdt_entry_t kerneldata;
    gdt_entry_t usernull;
    gdt_entry_t usercode;
    gdt_entry_t userdata;
} __attribute__((packed)) gdt_t;
extern gdt_t gdt;
void loadGdt(gdt_desc_t* desc);

#endif
