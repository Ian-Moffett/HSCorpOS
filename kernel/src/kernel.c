#include "drivers/FrameBuffer.h"
#include "drivers/memory/meminfo.h"
#include "memory/GDT.h"
#include "interrupts/IDT.h"
#include "interrupts/ISR.h"
#include <stdint.h>


canvas_t defaultcanvas = {
    .x = 10,
    .y = 10,
    .prevX = 10,
};

void test() {
}


__attribute__((interrupt)) void dmmy_gpf_handler(int_frame_t*) { 
    clearScreen(&defaultcanvas, 0xFFFFFFFF);
    __asm__ __volatile__("cli; hlt");
    while (1);
}


void _start(framebuffer_t* lfb, psf1_font_t* font, memory_info_t mem_info) {
    static gdt_desc_t gdt_desc;
    gdt_desc.offset = (uint64_t)&gdt;
    gdt_desc.size = sizeof(gdt) - 1;
    
    loadGdt(&gdt_desc);

    set_idt_entry(0xD, dmmy_gpf_handler, TRAP_GATE_FLAGS);
    set_idt_entry(0x0, div0_handler, TRAP_GATE_FLAGS);
    idt_install();

    // __asm__ __volatile__("int $0x0");
    
    defaultcanvas.lfb = lfb;
    defaultcanvas.font = font; 

    uint64_t mMapEntries = mem_info.mMapSize / mem_info.mMapDescSize;

    
    kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);

    for (int i = 0; i < mMapEntries; ++i) {        
        memdesc_t* desc = (memdesc_t*)((uint64_t)mem_info.mMap + (i * mem_info.mMapDescSize));
        kwrite(&defaultcanvas, hex2str((uint64_t)desc->physicalAddress), 0xA600CD);
        kwrite(&defaultcanvas, " => ", 0xFFFFFFFF);
        kwrite(&defaultcanvas, MSEGMENT_TYPES[desc->type], 0xFD0C21);
        kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);
    }

    // INTERRUPT ZONE.
    
    if (0 / 0 == 1);

    __asm__ __volatile__("cli; hlt");
}
