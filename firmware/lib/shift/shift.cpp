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
  Serial.print("shift init");
  pinMode(sp.latch, OUTPUT);
  pinMode(sp.clock, OUTPUT);
  pinMode(sp.data, OUTPUT);
#else
  std::cout << "Shift init" << std::endl;
#endif
}

void shiftOutValves(uint8_t data_out)
{
 // may need to cast uint8_t to int!!!
 //for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
// take the latchPin low so
// the LEDs don't change while you're sending in bits:
digitalWrite(ST_CP, LOW);
// shift out the bits:
shiftOut(DS, SH_CP, MSBFIRST, data_out);//, numberToDisplay);
//take the latch pin high so the LEDs will light up:
digitalWrite(ST_CP, HIGH);
// pause before next value:
//delay(100);
//}
    // data_out can be max of 256 per operation
/*#ifdef ARDUINO
    Serial.print("shiftout");
    //digitalWrite(sp.latch, LOW);
    //shiftOut(sp.data, sp.clock, LSBFIRST, data_out);
    //digitalWrite(sp.latch, HIGH);
    
    digitalWrite(ST_CP, LOW);
    shiftOut(DS, SH_CP, LSBFIRST, data_out);
    digitalWrite(ST_CP, HIGH);
#else
    std::cout << "Shift out: " << data_out << std::endl;
#endif*/
}
//#endif