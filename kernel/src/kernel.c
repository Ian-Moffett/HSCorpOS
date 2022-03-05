#include "drivers/FrameBuffer.h"
#include "drivers/memory/meminfo.h"
#include <stdint.h>


canvas_t defaultcanvas = {
    .x = 10,
    .y = 10
};

void _start(framebuffer_t* lfb, psf1_font_t* font, memory_info_t mem_info) {
    defaultcanvas.lfb = lfb;
    defaultcanvas.font = font;

    uint64_t mMapEntries = mem_info.mMapSize / mem_info.mMapDescSize;

    for (int i = 0; i < mMapEntries; ++i) {
        memdesc_t* desc = (memdesc_t*)((uint64_t)mem_info.mMap + (i * mem_info.mMapDescSize));
        kwrite(&defaultcanvas, MSEGMENT_TYPES[desc->type], 0xFFFFFFFF);
        kwrite(&defaultcanvas, "\n", 0xFFFFFFFF);
    }

}
