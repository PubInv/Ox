#ifndef SERIAL_TASK_H
#define SERIAL_TASK_H

#ifdef ARDUINO
#include <Arduino.h>
#else // Native
#include <iostream>
#endif

#include <core.h>

#define SERIAL_BAUD_RATE 19200
// #define SERIAL_BAUD_RATE 9600
#define SERIAL_DELAY 1000
#define SERIAL_TIMEOUT_MAX 10000

#define INPUT_BUFFER_SIZE 256


namespace OxApp
{

  class SerialTask : public OxCore::Task {
  private:
    bool one_char_command_found(int num_read, char buffer[], int k);

  public:
    bool initialization_success;
    bool set_goal_success;
    bool set_action_success;
    bool action_success;
    bool goal_reached;
    bool new_from_UI;
    // A buffer is needed to cross boundaries that may occur
    // within a PIRCS command within the
    char input_buffer[INPUT_BUFFER_SIZE];

    int number_of_goals;
    size_t goal_number;

    int number_of_actions;
    size_t action_number;
  private:
    bool _init() override;
    bool _run() override;

    virtual int clear_buffers(char buffer[]);

    virtual bool listen(char buffer[], int length);
  };
}

#endif
