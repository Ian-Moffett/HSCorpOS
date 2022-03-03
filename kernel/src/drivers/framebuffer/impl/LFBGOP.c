#include "../LFBGOP.h"

void putChar(framebuffer_t* framebuffer, psf1_font* font, unsigned int color, char chr, unsigned int xOffset, unsigned int yOffset) {
    unsigned int* pixPtr = (unsigned int*)framebuffer->baseAddr;
    char* fontPtr = font->glyphBuffer + (chr * font->header->chsize);
    for (unsigned long y = yOffset; y < yOffset + 16; y++){
        for (unsigned long x = xOffset; x < xOffset+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOffset))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * framebuffer->ppsl)) = color;
                }

        }
        fontPtr++;
    }
}
