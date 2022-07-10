#include "serial.h"
#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace OxApp {

bool SerialTask::receive() {

}

bool SerialTask::sendMessage(myMessage &msg) {
  char buffer[80];
  memset(&buffer[0], '\0', 80);
  memcopy(&buffer[0], msg, 80);
  int bytesSent = SerialUSB.write(buffer, 80);
}

bool SerialTask::_init() {
  _messageCount = 0;
  _pos = 0;
  SerialUSB.begin(115200);
  while (!SerialUSB) {

  }
}

bool SerialTask::_run() {
  while (SerialUSB.available() > 0) {
    char rcv = Serial.read();
    _buffer[_pos] = rcv;
    _pos++;
  }
}

}