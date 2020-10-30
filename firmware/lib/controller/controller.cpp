#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <controller.h>
#include <stdio.h>
#include <iostream>

//#include <valve.h>
//#include <debug.h>
//#include <config.h>
#include <inttypes.h>

//using namespace PIOC_Valve;
//using namespace PIOC_Debug;

namespace PIOC_Controller {

// Helper MACRO to display bit pattern for debugging only
//https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
// printf("Some text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


////////// FUNCTIONS //////////

  void printDebugValves(uint32_t tick, int valve_bits, bool start, char name){
#ifdef ARDUINO
      //digitalWrite(VALVES[i].pin, LOW);
      Serial.print("Valves: ");
      Serial.print(name);
      Serial.print(" ");
      Serial.print(start);
      Serial.print(" ");
      Serial.println(tick);
#else
      printf("Valves: " BYTE_TO_BINARY_PATTERN , BYTE_TO_BINARY(valve_bits));
      printf(" %c%d t_%d\n", name, start, tick);
#endif
  }

  

  uint8_t updateValve(valve *valve, uint32_t *msNow){
    std::cout << *msNow << std::endl;
    std::cout << valve->name<< std::endl;
    std::cout << valve->num << std::endl;
    std::cout << valve->pin << std::endl;
    std::cout << valve->state << std::endl;
    std::cout << valve->start<< std::endl;
    std::cout << valve->stop<< std::endl;
    
      if ((valve->state = 0) && (*msNow >= valve->start)){
        // Turn on valve
        std::cout << "valve on";
        valve->state = 1;
        return valve->pin;
      }
      else if ((valve->state = 1) && (*msNow >= valve->stop)){
        std::cout << "valve off";
        valve->state = 2;
        return valve->pin;
      }
      else {
        //std::cout << "returning nothing";
        return 0;
      }

  }

  ////////// METHODS ///////////

  bool ValveController::update(uint32_t *msNow){
    // TODO: error checking: msNow > msLast, msNow < msLast + tolerance
    
    //std::cout << *msNow << std::endl;
    for (int i = 0; i < numValves; i++){
      uint8_t x = updateValve((this->valves+i), msNow);
      std::cout << "x:" << x << std::endl;
    }

    //updateValves(msNow);

    printDebugValves(*msNow, valveBits, false, 'A');

    return true;
  }



}



  /*uint8_t ValveController::updateValves(uint32_t msNow){
    for (uint8_t i = 0; i < numValves; i++){
      if ((*(valves+i)).state = 0){
          if (msNow >= (*(valves+i)).start){
            (*(valves+i)).state == 1;
            valveBits |= (*(valves+i)).pin;
            //Debug<const char*>("Valve off!");
            //printDebugValves(msNow, v, true, valve.name);
          }
      } else if ((*(valves+i)).state = 1){
        if (msNow >= (*(valves+i)).stop){
          (*(valves+i)).state == 2;
          valveBits ^= (*(valves+i)).pin;
          //Debug<const char*>("Valve off!");
          //printDebugValves(msNow, v, false, valve.name);
        }
      }
    }
  }
*/


/*for (uint8_t i = 0; i < numValves; i++){
      switch ((*(valves+i)).state){
        case READY:
          if (msNow >= (*(valves+i)).start){
            (*(valves+i)).state == ON;
            valveBits |= (*(valves+i)).pin;
            //printDebugValves(msNow, v, true, valve.name);
          }
        break;
        case ON:

            Debug<const char*>("Valve on!");
          if (msNow >= (*(valves+i)).stop){
            (*(valves+i)).state == OFF;
            valveBits ^= (*(valves+i)).pin;
            Debug<const char*>("Valve off!");
            //printDebugValves(msNow, v, false, valve.name);
          }
        break;
        case OFF:
          // needs to be reset at the start of the cycle
        break;
        case FAULT:
          // TODO
        break;
        default:
          Debug<const char*>("Valve state fault!");
        break;
      }
    }*/

/*bool ValveController::tick(uint32_t tick) {
    for (uint8_t i = 0; i < numValves; i++){
      if ((*(valves+i)).start == tick){
        valveBits |= (*(valves+i)).pin;
        printDebugValves(tick, i, valveBits, true, (*(valves+i)).name);
      }

      if ((*(valves+i)).stop == tick){
        valveBits ^= (*(valves+i)).pin;
        printDebugValves(tick, i, valveBits, false, (*(valves+i)).name);
      }
    }

    return true;
  }*/
  

    /*bool ValveController::setupValve(valve valve, uint8_t i){
    this->valves[i] = valve;
    return true;
  }

  bool ValveController::setupValves(valve valves[], uint8_t numValves){
    for (int i = 0; i < numValves; i++){
      this->valves[i] = valves[i];
    }
    return true;
  }*/

/*switch ((*(valves+i)).state){
        case READY:
          if (msNow >= (*(valves+i)).start){
            (*(valves+i)).state == ON;
            v |= (*(valves+i)).pin;
            printDebugValves(msNow, i, v, true, (*(valves+i)).name);
          }
        break;
        case ON:
          if (msNow >= (*(valves+i)).stop){
            (*(valves+i)).state == OFF;
            v ^= (*(valves+i)).pin;
            printDebugValves(msNow, i, v, false, (*(valves+i)).name);
          }
        break;
        case OFF:
        break;
        case WAS_ON:
        break;
        case ERROR:
        break;
        default:
        break;
      }*/