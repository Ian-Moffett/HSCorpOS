#include "../ISR.h"


extern canvas_t defaultcanvas;


static void display_details(unsigned int idt_idx, int_frame_t* frame) {
    clearScreen(&defaultcanvas, 0xFF0000);
    kwrite(&defaultcanvas, "CPU EXCEPTION: ", 0xFFFFFFFF);
    kwrite(&defaultcanvas, hex2str(idt_idx), 0xFFFFFFFF);
    kwrite(&defaultcanvas, "\n\n<==== DUMPING REGISTER STATE ====>\n", 0xFFFFFFFF);
    kwrite(&defaultcanvas, "RIP: ", 0xFFFFFFFF);
    kwrite(&defaultcanvas, hex2str(frame->rip), 0xFFFFFFFF);
    kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);
    kwrite(&defaultcanvas, "CS: ", 0xFFFFFFFF);
    kwrite(&defaultcanvas, hex2str(frame->cs), 0xFFFFFFFF);
    kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);
    kwrite(&defaultcanvas, "RFLAGS: ", 0xFFFFFFFF);
    kwrite(&defaultcanvas, hex2str(frame->rflags), 0xFFFFFFFF);
    kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);
    kwrite(&defaultcanvas, "RSP: ", 0xFFFFFFFF);
    kwrite(&defaultcanvas, hex2str(frame->rsp), 0xFFFFFFFF);
    kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);
    kwrite(&defaultcanvas, "SS: ", 0xFFFFFFFF);
    kwrite(&defaultcanvas, hex2str(frame->ss), 0xFFFFFFFF);
    kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);

}


__attribute__((interrupt)) void div0_handler(int_frame_t* frame) {
    display_details(0x0, frame);
    __asm__ __volatile__("cli; hlt");    
}
