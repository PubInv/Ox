#include <serial_task.h>
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <fstream>
#endif

namespace OxApp {

bool SerialTask::receive() {

}

bool SerialTask::send(myMessage &msg) {
  char buffer[80];
  for (int i = 0; i < 80; i++) {
    buffer[i] = '\0';
  }
  // memcopy(&buffer[0], msg, 80);
  int bytesSent = 0;
#ifdef ARDUINO
  bytesSent = SerialUSB.write(buffer, 80);
#else
  // bytesSent = 
#endif

  return true;
}

bool SerialTask::_init() {
  _messageCount = 0;
  _pos = 0;
#ifdef ARDUINO
  SerialUSB.begin(115200);
  while (!SerialUSB) {}
#else
  // No need to init on native
#endif
  return true;
}

bool SerialTask::_run() {

#ifdef ARDUINO
  while (SerialUSB.available() > 0) {
    char rcv = Serial.read();
    _buffer[_pos] = rcv;
    _pos++;
  }
#else
  std::ofstream MyFile("testfile.txt");
  MyFile << "Writing something";
  MyFile.close();
#endif
  return true;
}

}