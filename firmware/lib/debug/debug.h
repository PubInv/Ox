#ifndef DEBUG_H
#define DEBUG_H

#include <inttypes.h>

void serial_init(uint32_t baud);
void serial_out_s(const char *string);
void serial_out_b(bool b);
void serial_out_uint8(uint8_t i);

#endif
