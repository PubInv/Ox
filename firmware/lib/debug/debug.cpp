#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <iostream>

void serial_init(uint32_t baud){
#ifdef ARDUINO
  Serial.begin(baud);
#else
  printf("%s", "Serial init");
#endif
}

void serial_out_s(const char *s){
#ifdef ARDUINO
  Serial.print(s);
#else
  printf("%s", s);
#endif
}

void serial_out_b(bool b){
  #ifdef ARDUINO
    Serial.print(b);
  #else
    fputs(b ? "true" : "false", stdout);
  #endif
}

void serial_out_uint8(uint8_t i){
  #ifdef ARDUINO
    Serial.print(b); // needs to be uint32_t
  #else
    std::cout << i;
  #endif
}
