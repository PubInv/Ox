#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif

#include <shift.h>
//#include <config.h>
#include <inttypes.h>

// Shift register
#define DS 13 // 747HC pin 14 - serial data
#define ST_CP 12 // 747HC pin 12 - storage register clock (latch)
#define SH_CP 27 // 747HC pin 11 - shift register clock

shift_pins sp;

void shiftInit(){
  sp.latch = ST_CP;
  sp.clock = SH_CP;
  sp.data = DS;

#ifdef ARDUINO
  pinMode(sp.latch, OUTPUT);
  pinMode(sp.clock, OUTPUT);
  pinMode(sp.data, OUTPUT);
#else
  std::cout << "Shift init" << std::endl;
#endif
}

void shiftOut(uint8_t data_out)
{
    // data_out can be max of 256 per operation
#ifdef ARDUINO
    digitalWrite(sp.latch, LOW);
    shiftOut(sp.data, sp.clock, LSBFIRST, data_out);
    digitalWrite(sp.latch, HIGH);
#else
    std::cout << "Shift out: " << data_out << std::endl;
#endif
}
//#endif