#include "../ISR.h"



__attribute__((interrupt)) void div_0_handler(int_frame_t* int_frame) {
    while (1);
}
