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
#define DEBUG_SERIAL_LISTEN 1
#define DEBUG_INPUT 3

namespace OxApp
{

  bool AbstractSerialTask::_init() {

    initialization_success = false;
    input_buffer[0] = '\0';
    return initialization_success = true;
  } // Setup communication channel


  bool SerialTask::_init() {

    initialization_success = false;
    input_buffer[0] = '\0';
    return initialization_success = true;
  } // Setup communication channel


  bool Stage2SerialTask::_init() {

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


  bool AbstractSerialTask::listen(char buffer[], int length) {

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

#if DEBUG_SERIAL_LISTEN > 3
      DebugLn<const char *>("length after read: ");
      DebugLn<int>(n);
      // now what we really want to do is look for a closing curly brace...

      DebugLn<const char *>("input buffer");
      Debug<const char *>(input_buffer);
      DebugLn<const char *>("|");
#endif

      if (input_buffer[n - 1] == '}') {
#if DEBUG_SERIAL_LISTEN > 3
        DebugLn<const char *>("Found End of Line!");
#endif
        strcpy(buffer, input_buffer);
        input_buffer[0] = '\0';
#if DEBUG_SERIAL_LISTEN > 3
        DebugLn<const char *>("Done");
#endif
        return true;
      } else {
        // These lines don't compile and I don't know why!
#if DEBUG_SERIAL_LISTEN > 3
        DebugLn<const char *>("Available bytes:\n");
        DebugLn<const int>(Serial.available());
#endif
        return false;
      }
    }

    return new_from_UI;
#endif
  } // Listening to the communication channel


  // The clears out the current Serial buffer, and
  // also sets the current input_buffer to null.
  // This is appropriate to call when we know we
  // have processed a one-character command, which
  // include the emergency stop commands.
  int AbstractSerialTask::clear_buffers(char buffer[]) {
#ifdef ARDUINO
    Serial.readBytesUntil('\n', buffer, INPUT_BUFFER_SIZE - 1);
#endif
    input_buffer[0] = '\0';
    return 0;
  }



    bool SerialTask::_run()
  {
    //        OxCore::Debug<const char *>("Serial_task_run");
    char buffer[256];
    SetCommand sc;
    if (listen(buffer, 256)) {
#if DEBUG_INPUT > 2
      DebugLn("read buffer\n");
#endif
      sc = get_set_command_from_JSON(buffer, (uint16_t)256);
#if DEBUG_INPUT > 2
      //      render_set_command_raw(&sc);
#endif
      DebugLn<const char *>("rendered command");
      delay(100);

      // This is an over simplifcation of possible state transitions!
      // This needs to be taken out to a separate routine, probably
      // implemented in the machine
      MachineConfig *cogConfig = getConfig();
      // TODO: This would probably be better handled by setting
      // the most recent command into the state, and having the
      // the cog_task remove it. Then all state changes would be made in
      // one place!


      // These are the STATE CHANGE COMMANDS
      if (sc.command == 'W') {
        if (cogConfig->ms == Off) {
          cogConfig->ms = Warmup;
          Debug<const char *>("New State: Warmup!");
        }
      } else if (sc.command == 'C') {
        if (cogConfig->ms != Off) {
          cogConfig->ms = Cooldown;
          Debug<const char *>("New State: Cooldown!");
        }
      } else if (sc.command == 'E') {
        if (cogConfig->ms != Off) {
          cogConfig->ms = EmergencyShutdown;
          Debug<const char *>("New State: Emergency Shutdown!");
        }
      } else if (sc.command == 'A') {
        if (cogConfig->ms == OffUserAck) {
          cogConfig->ms = Off;
          Debug<const char *>("New State: Off!");
        }
      } else if (sc.command == 'I') {
        cogConfig->idleOrOperate = Idle;
        Debug<const char *>("New SubState: Idle!");
      } else if (sc.command == 'O') {
        cogConfig->idleOrOperate = Operate;
        Debug<const char *>("New SubState: Operate");
      } else if (sc.command == 'P') {
        // HERE BEGIN THE PARAMETER SET Commands.
        if (sc.parameter == 'T') {
          DebugLn<const char *>("Changing Temperature!");
          if (sc.interpretation != 'T') {
            Debug<const char *>("Can only recognize \"Target\" interpretation at present!");
          } else {
            int new_temp_C = sc.val;
            // now a little sanity check...
            if (new_temp_C < 20 || new_temp_C > 1000) {
              Debug<const char *>("Temperature out of range!");
            } else {
              DebugLn<const char *>("WARING! NOT YET IMPLEMENTED! THIS COMMAND CHANGES NOTHING YET!");


              //                  cogConfig->OPERATING_TEMPERATURE = new_temp_C;
              //                  Debug<const char *>("Post Stack Temp changed to: ");
              //                  Debug<float>(cogConfig->OPERATING_TEMPERATURE);
              //                  DebugLn<const char *>(" degrees C");
            }
          }
        } if (sc.parameter == 'C') {
          Debug<const char *>("Changing Current!");
          if (sc.interpretation != 'T') {
            DebugLn<const char *>("Can only recognize \"Target\" interpretation at present!");
          } else {
            int new_current_mA = sc.val;
            // now a little sanity check... We should probably regularize these
            if (new_current_mA < 0 || new_current_mA > getConfig()->MAXIMUM_STACK_AMPS*1000.0) {
              Debug<const char *>("Amperage out of range!");
            } else {
              cogConfig->MAXIMUM_STACK_AMPS = new_current_mA;
              Debug<const char *>("Stack Current Amperage changed to: ");
              Debug<float>(cogConfig->MAXIMUM_STACK_AMPS);
              DebugLn<const char *>(" milliamps");
            }
          }
        }
        else {
          Debug<const char *>("Unrecognized parameter type!");
        }
      }
    }
  }


  bool SerialTask::one_char_command_found(int num_read, char buffer[], int k) {
    const int NUM_ONE_CHAR_COMMANDS = 6;
    // "warmup"
    // "cooldown"
    // "emergency shutdown"
    // "acknowledge"
    // "idle"
    // "operate"
    const char one_char_commands[NUM_ONE_CHAR_COMMANDS] = {'w','c','e','a','i','o'};

    // Because this routine is designed to return a buffer that
    // contains a PIRCS command, we expand the PIRCS commands,
    // and return the command in the buffer, where it will be read
    // by the PIRCS library. Then how we interpret it later happens
    // in a different part of the code.
    char c = 0;
    if (num_read == 2) {
      for(int i = 0; !c && i < NUM_ONE_CHAR_COMMANDS; i++) {
        if (input_buffer[k-1] == one_char_commands[i])
          c = input_buffer[k - 1];
      }
    }
    if (num_read == 1) {
      for(int i = 0; !c && i < NUM_ONE_CHAR_COMMANDS; i++) {
        if (input_buffer[k] == one_char_commands[i])
          c = input_buffer[k];
      }
    }
#if DEBUG_SERIAL_LISTEN > 3
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
    case 'e':
      strcpy(
             buffer,
             "{\"com\":\"E\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Emergency Shutdown!\n");
      return true;
      break;
    case 'a':
      strcpy(
             buffer,
             "{\"com\":\"A\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Acknowledge!\n");
      return true;
      break;
    case 'i':
      strcpy(
             buffer,
             "{\"com\":\"I\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Idle!\n");
      return true;
      break;
    case 'o':
      strcpy(
             buffer,
             "{\"com\":\"O\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Operate Normally!\n");
      return true;
      break;
    default:
      DebugLn<const char *>("Unkown command!\n");
    }
    return false;
  }

    bool Stage2SerialTask::one_char_command_found(int num_read, char buffer[], int k) {
    const int NUM_ONE_CHAR_COMMANDS = 9;
    // Control the int1 heater
    // Control the ext1 heater
    // Control the ext2 heater
    // "warmup"
    // "cooldown"
    // "emergency shutdown"
    // "acknowledge"
    // "idle"
    // "operate"
    const char one_char_commands[NUM_ONE_CHAR_COMMANDS] = {'1','2','3','w','c','e','a','i','o'};

    // c
    // Because this routine is designed to return a buffer that
    // contains a PIRCS command, we expand the PIRCS commands,
    // and return the command in the buffer, where it will be read
    // by the PIRCS library. Then how we interpret it later happens
    // in a different part of the code.
    char c = 0;
    if (num_read == 2) {
      for(int i = 0; !c && i < NUM_ONE_CHAR_COMMANDS; i++) {
        if (input_buffer[k-1] == one_char_commands[i])
          c = input_buffer[k - 1];
      }
    }
    if (num_read == 1) {
      for(int i = 0; !c && i < NUM_ONE_CHAR_COMMANDS; i++) {
        if (input_buffer[k] == one_char_commands[i])
          c = input_buffer[k];
      }
    }
#if DEBUG_SERIAL_LISTEN > 3
    DebugLn<const char*>("testing one character command");
#endif

    switch (c) {
    case '1':
      getConfig()->s2heaterToControl = Int1;
      DebugLn<const char *>("Switching to controlling the Int1 Heater!\n");
      clear_buffers(input_buffer);
      return false;
      break;
    case '2':
      getConfig()->s2heaterToControl = Ext1;
      DebugLn<const char *>("Switching to controlling the Ext1 Heater!\n");
      clear_buffers(input_buffer);
      return false;
      break;
    case '3':
      getConfig()->s2heaterToControl = Ext2;
      DebugLn<const char *>("Switching to controlling the Ext2 Heater!\n");
      clear_buffers(input_buffer);
      return false;
      break;

      // WARMUP!
    case 'w': {
      strcpy(
             buffer,
             "{\"com\":\"W\",\"par\":\"M\",\"int\":\"s\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      return true;
    }
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
    case 'e':
      strcpy(
             buffer,
             "{\"com\":\"E\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Emergency Shutdown!\n");
      return true;
      break;
    case 'a':
      strcpy(
             buffer,
             "{\"com\":\"A\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Acknowledge!\n");
      return true;
      break;
    case 'i':
      strcpy(
             buffer,
             "{\"com\":\"I\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Idle!\n");
      return true;
      break;
    case 'o':
      strcpy(
             buffer,
             "{\"com\":\"O\",\"par\":\"M\",\"int\":\"c\",\"mod\":\"U\",\"val\":0}");
      //      clear_buffers(input_buffer);
      DebugLn<const char *>("Returning Operate Normally!\n");
      return true;
      break;
    default:
      DebugLn<const char *>("Unkown command!\n");
    }
    return false;
  }

  bool Stage2SerialTask::_run()
  {
    char buffer[256];
    SetCommand sc;
    if (listen(buffer, 256)) {
#if DEBUG_INPUT > 2
      DebugLn<const char *>(buffer);
#endif
      // Could this be causing a buffer overflow?
      sc = get_set_command_from_JSON(buffer, (uint16_t)256);
#if DEBUG_INPUT > 2
      render_set_command_raw(&sc);
#endif
      // Need to have this modify the correct machine
      MachineState new_ms;
      bool new_ms_set = false;
      if (sc.command == 'W') {
        if (getConfig()->ms == Off) {
          new_ms = Warmup;
          new_ms_set = true;
          Debug<const char *>("New State: Warmup!");
          delay(100);
        }
      } else if (sc.command == 'C') {
        if (getConfig()->ms != Off) {
          new_ms = Cooldown;
          new_ms_set = true;
          Debug<const char *>("New State: Cooldown!");
        }
      } else if (sc.command == 'E') {
        if (getConfig()->ms != Off) {
          new_ms = EmergencyShutdown;
          new_ms_set = true;
          Debug<const char *>("New State: Emergency Shutdown!");
        }
      } else if (sc.command == 'A') {
        if (getConfig()->ms == OffUserAck) {
          new_ms = Off;
          new_ms_set = true;
          Debug<const char *>("New State: Off!");
        }
      } else if (sc.command == 'P') {
        // HERE BEGIN THE PARAMETER SET Commands.
        if (sc.parameter == 'T') {
          DebugLn<const char *>("Changing Temperature!");
          if (sc.interpretation != 'T') {
            Debug<const char *>("Can only recognize \"Target\" interpretation at present!");
          } else {
            int new_temp_C = sc.val;
            // now a little sanity check...
            if (new_temp_C < 20 || new_temp_C > 1000) {
              Debug<const char *>("Temperature out of range!");
            } else {
              DebugLn<const char *>("WARING! NOT YET IMPLEMENTED! THIS COMMAND CHANGES NOTHING YET!");

            }
          }
        }
        else {
          Debug<const char *>("Unrecognized parameter type!");
        }
      }
      getConfig()->s2sr->ms[getConfig()->s2heaterToControl] = new_ms;
    }
  }
}
