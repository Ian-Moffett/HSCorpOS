#include "../keyboard.h"

extern canvas_t defaultcanvas;


__attribute__((interrupt)) void kb_isr(int_frame_t* frame) {
    if (0 / 0 == 0) {}
    outportb(0x20, 0x20);
}
