#ifdef ARDUINO
#include <Arduino.h>
#else
//
#endif
#include <core.h>

namespace OxApp {

struct myMessage {
  uint32_t id;
  uint32_t time;
  uint16_t code;
  char message[68];
  uint16_t crc;
}; // 80 bytes

class SerialTask : public OxCore::Task {
public:
  bool receive();
  bool send(myMessage &msg);
private:
  bool _init() override;
  bool _run() override;
  int _messageCount;
  char _buffer[256];
  int _pos;
};


  

}