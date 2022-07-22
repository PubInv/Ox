/*
Copyright (C) 2022 Robert Read, Ben Coombs.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <iostream>
#endif

#include <debug.h>
#include <serial_task.h>
#include <stdio.h>
#include <string.h>
#include <PIRCS.h>
#include <machine.h>

using namespace OxCore;
#define DEBUG_SERIAL_LISTEN 3

namespace OxApp
{

bool SerialTask::_init() {

  initialization_success = false;
  input_buffer[0] = '\0';
  return initialization_success = true;
} // Setup communication channel

void render_set_command_raw(SetCommand* m) {
  Debug<const char *>("Command :");
  DebugLn<char>(m->command);
  Debug<const char *>("Parameter :");
  DebugLn<char>(m->parameter);
  Debug<const char *>("Interpretation :");
  DebugLn<char>(m->interpretation);
  Debug<const char *>("Modifier :");
  DebugLn<char>(m->modifier);
  Debug<const char *>("Val :");
  DebugLn<int>(m->val);
}


  bool SerialTask::_run()
    {
      //        OxCore::Debug<const char *>("Serial_task_run");
        char buffer[256];
        SetCommand sc;
        if (listen(buffer, 256)) {
#if DEBUG_INPUT > 0
          DebugLnCC("read buffer\n");
          DebugLn<const char *>(buffer);
#endif
          sc = get_set_command_from_JSON(buffer, (uint16_t)256);
          render_set_command_raw(&sc);
          DebugLn<const char *>("command");
          // This is an over simplifcation of possible state transitions!
          // This needs to be taken out to a separate routine, probably
          // implemented in the machine
          MachineState *ms = (MachineState *) _properties.state_and_config;
          if (sc.command == 'W') {
            // Note: This is a global variable. I don't like this style much. I believe we should
            // find a way to pass the machine state into every task!
            if (*ms == Off) {
              *ms = Warmup;
              Debug<const char *>("New State: Warmup!");
            }
          } else if (sc.command == 'C') {
            if (*ms != Off) {
              *ms = Cooldown;
              Debug<const char *>("New State: Cooldown!");
            }
          }

        }
    }
// The clears out the current Serial buffer, and
// also sets the current input_buffer to null.
// This is appropriate to call when we know we
// have processed a one-character command, which
// include the emergency stop commands.
int SerialTask::clear_buffers(char buffer[]) {
#ifdef ARDUINO
  Serial.readBytesUntil('\n', buffer, INPUT_BUFFER_SIZE - 1);
#endif
  input_buffer[0] = '\0';
}


  // Note this code can be cleaned up.
bool SerialTask::one_char_command_found(int num_read, char buffer[], int k) {

  // HACK ALERT!! For debugging, we want VERY FAST commands to disable and
  // and home the ventilator. Because this is actully being
  // done by a human being, we want this to be a single, one-character command:
  // c
  // Because this routine is designed to return a buffer that
  // contains a PIRCS command, we expand the PIRCS commands,
  // and return the command in the buffer, where it will be read
  // by the PIRCS library. Then how we interpret it later happens
  // in a different part of the code.
  char c = 0;
  if ((num_read == 2 &&
       (input_buffer[k - 1] == 'w' || input_buffer[k - 1] == 'c'
        // || input_buffer[k - 1] == '1' || input_buffer[k - 1] == 'h'
        )))
    {
    c = input_buffer[k - 1];
  }
  if ((num_read == 1 && (input_buffer[k] == 'w' || input_buffer[k] == 'c'
        // || input_buffer[k] == '1' || input_buffer[k] == 'h'
                         ))) {
    c = input_buffer[k];
  }
#if DEBUG_SERIAL_LISTEN > 0
  DebugLn<const char*>("testing one character command");
#endif

  switch (c) {
    // WARMUP!
  case 'w':
    strcpy(
        buffer,
        "{\"com\":\"W\",\"par\":\"M\",\"int\":\"s\",\"mod\":\"U\",\"val\":0}");
    //      clear_buffers(input_buffer);
    DebugLn<const char *>("Returning Warmup!\n");
    return true;
    break;
    // COOLDOWN!
  case 'c':
    strcpy(
        buffer,
        "{\"com\":\"C\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
    //      clear_buffers(input_buffer);
    DebugLn<const char *>("Returning Cooldown!\n");
    return true;
    break;
  default:
        DebugLn<const char *>("Unkown command!\n");
  }
  return false;
}

bool SerialTask::listen(char buffer[], int length) {

  // If we aren't inside the Arduino environment,
  // we probably can't do a serial listen at all, so this
  // is for compiling..
#ifndef ARDUINO
  return false;
#else

  new_from_UI = false;

#if DEBUG_SERIAL_LISTEN > 3
  Debug<const char *>("Start");
  DebugLn<const int>(Serial.available());
#endif
  if (Serial.available()) {
    new_from_UI = true;
    int n = strlen(input_buffer);

    // NOTE: This code might not handle two commands that come in
    // at one time!
    // We insist that every command begin with a curly brace '{'...
    // So we delete all characters up to the first curly brace...
    char *position_ptr = strchr(input_buffer, '{');
    if (!position_ptr) {
      position_ptr = input_buffer + n;
    }
    int to_del = position_ptr - input_buffer;

    strncpy(input_buffer, position_ptr, n - to_del);
    input_buffer[n - to_del] = '\0';
    n = n - to_del;

#if DEBUG_SERIAL_LISTEN > 1
    Debug<const char *>("Starting with:");
    DebugLn<const int>(n);
#endif
    // We have to be very careful with our sizes here...
    size_t num_read = Serial.readBytesUntil('\n', input_buffer + n, 255 - n);

#if DEBUG_SERIAL_LISTEN > 1
    DebugLn<const char *>("num_read");
    DebugLn<int>(num_read);
#endif

    // Here we need to check that n+num_read+1 is less that the size of the
    // input_buffer...
    input_buffer[n + num_read] = '\0'; // a zero byte, ASCII NUL
    int k = n + num_read - 1;
#if DEBUG_SERIAL_LISTEN > 0
    DebugLn<char>(input_buffer[k]);
    DebugLn<char>(input_buffer[k - 1]);
#endif

    if (one_char_command_found(num_read, buffer, k)) {
      return true;
    }

    while (isspace(input_buffer[k])) {

#if DEBUG_SERIAL_LISTEN > 0
      DebugLn<const char *>("REMOVING CHARACTER");
#endif
      input_buffer[k] = '\0';
      k--;
    }
    // now see if a closing brace exists in the buffer...
    n = strlen(input_buffer);

#if DEBUG_SERIAL_LISTEN > 0
    DebugLn<const char *>("length after read: ");
    DebugLn<int>(n);
    // now what we really want to do is look for a closing curly brace...

    DebugLn<const char *>("input buffer");
    Debug<const char *>(input_buffer);
    DebugLn<const char *>("|");
#endif

    if (input_buffer[n - 1] == '}') {
#if DEBUG_SERIAL_LISTEN > 0
      DebugLn<const char *>("Found End of Line!");
#endif
      strcpy(buffer, input_buffer);
      input_buffer[0] = '\0';
#if DEBUG_SERIAL_LISTEN > 0
      DebugLn<const char *>("Done");
#endif
      return true;
    } else {
      // These lines don't compile and I don't know why!
#if DEBUG_SERIAL_LISTEN > 0
      DebugLn<const char *>("Available bytes:\n");
      DebugLn<const int>(Serial.available());
#endif
      return false;
    }
  }

  return new_from_UI;
#endif
} // Listening to the communication channel
}
