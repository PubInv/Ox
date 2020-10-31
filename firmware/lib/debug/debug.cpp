#include <stdio.h>
#include <iostream>
#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace PIOC_Debug {

  void serialBegin(int baud){
    #ifdef ARDUINO
      Serial.begin(baud);
    #else
      printf("Serial init\n");
    #endif
  }

}