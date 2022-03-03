#include "../IO.h"


void outportb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}


uint8_t inportb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}


void io_wait() {
    __asm__ __volatile("outb %%al, $0x80" : : "a"(0));
}
