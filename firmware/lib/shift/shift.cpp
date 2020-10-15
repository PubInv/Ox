#include "shift.h"

void shift()
{
  // Convert from 8 bit and output to shift register
  for (int j = 0; j < 256; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    /*digitalWrite(ST_CP, LOW);
    shiftOut(DS, SH_CP, LSBFIRST, j);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(ST_CP, HIGH);
    delay(1000);*/
  }
}
