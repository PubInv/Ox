#ifdef ARDUINO
#include <Arduino.h>


#include "shift.h"
#include <config.h>

shift_pins sp;

void shift_init(){
  sp.latch = ST_CP;
  sp.clock = SH_CP;
  sp.data = DS;

  pinMode(sp.latch, OUTPUT);
  pinMode(sp.clock, OUTPUT);
  pinMode(sp.data, OUTPUT);
}

void shift_out(shift_pins sp, uint8_t data_out)
{
    // data_out can be max of 256 per operation
    digitalWrite(sp.latch, LOW);
    shiftOut(sp.data, sp.clock, LSBFIRST, data_out);
    digitalWrite(sp.latch, HIGH);
}
#endif