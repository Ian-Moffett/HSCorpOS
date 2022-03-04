#include "drivers/FrameBuffer.h"

canvas_t defaultcanvas = {
    .x = 10,
    .y = 10
};

void _start(framebuffer_t* lfb, psf1_font_t* font) {
    defaultcanvas.lfb = lfb;
    defaultcanvas.font = font;
    kwrite(&defaultcanvas, "Hello, World!\nMeow meow meow.\n", 0xFFFFFFFF);
}
