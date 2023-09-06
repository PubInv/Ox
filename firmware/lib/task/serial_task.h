#ifndef SERIAL_TASK_H
#define SERIAL_TASK_H

#include <Arduino.h>
#include <core.h>
#include <debug.h>
#include <PIRCS.h>
#include <machine.h>
#include <stage2_hal.h>

#define SERIAL_BAUD_RATE 19200
// #define SERIAL_BAUD_RATE 9600
#define SERIAL_DELAY 1000
#define SERIAL_TIMEOUT_MAX 10000

#define INPUT_BUFFER_SIZE 256


namespace OxApp
{
  class AbstractSerialTask : public OxCore::Task {
  private:
    virtual bool one_char_command_found(int num_read, char buffer[], int k);

  public:
        bool initialization_success;
        bool new_from_UI;
    // A buffer is needed to cross boundaries that may occur
    // within a PIRCS command within the
    char input_buffer[INPUT_BUFFER_SIZE];
    bool _init() override;
    bool _run() override;
    int DEBUG_LEVEL = 0;

    virtual int clear_buffers(char buffer[]);
    virtual bool listen(char buffer[], int length);
  };

  class SerialTask : public AbstractSerialTask {
  public:
    bool one_char_command_found(int num_read, char buffer[], int k) override;
    bool _init() override;
    bool _run() override;
  };

  // WARNING! This is duplciated code. It should be moved into
  // the abstract class when that is created.
  void render_set_command_raw(SetCommand* m);

}

#endif
