#include "../ISR.h"



__attribute__((interrupt)) void div_0_handler(int_frame_t* int_frame) {
    __asm__ __volatile__("cli; hlt");
}
