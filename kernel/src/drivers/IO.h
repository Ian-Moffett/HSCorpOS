#ifndef IO_H
#define IO_H

#include <stdint.h>

void outportb(uint16_t port, uint8_t value);
uint8_t inportb(uint16_t port);
void io_wait();


#endif
