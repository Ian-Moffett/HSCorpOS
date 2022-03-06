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



void kwrite(canvas_t* canvas, const char* const STR, unsigned int color) {
    for (int i = 0; i < strlen(STR); ++i) {
        if (STR[i] == '\n') {
            canvas->x = canvas->prevX;
            canvas->y += 20;
            continue;
        }

        if (canvas->y >= canvas->lfb->height) {
            canvas->prevX += 300;
            canvas->y = 10;
        }

        putChar(canvas->lfb, canvas->font, color, STR[i], canvas->x, canvas->y);
        canvas->x += 8;
    }
}
