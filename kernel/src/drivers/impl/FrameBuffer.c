#include "../FrameBuffer.h"


void putChar(framebuffer_t* framebuffer, psf1_font_t* psf1_font, unsigned int color, char chr, unsigned int xOff, unsigned int yOff) {
    unsigned int* pixPtr = (unsigned int*)framebuffer->baseAddr;
    char* fontPtr = psf1_font->glyphBuffer + (chr * psf1_font->header->chsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * framebuffer->ppsl)) = color;
                }

        }
        fontPtr++;
    }
}



void kwrite(canvas_t* canvas, char* str) {
}
