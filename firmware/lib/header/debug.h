#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

namespace PIOC_Debug {
  
  // For example, call Debug<char*>("Some text") or Debug<bool>(myBoolVar)
  // to get a debug output on Arduino or native environments
  template <class myType>
  void Debug (myType a) {
  #ifdef ARDUINO
    Serial.print(a);
  #else
    std::cout << a;
  #endif
  }

  void serialBegin(int baud);
}

#endif
