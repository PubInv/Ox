#include <stdio.h>
#include <iostream>

namespace PIOC_Debug {

  void serialBegin(int baud){
    #ifdef Arduino
      Serial.Begin(baud);
    #else
      printf("Serial init\n");
    #endif
  }

}