/*
  Copyright (C) 2023 Robert L. Read

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

#include <stage2_serial_task.h>
#include <debug.h>
#include <PIRCS.h>


using namespace OxCore;

namespace OxApp
{


  bool Stage2SerialTask::_init() {

    initialization_success = false;
    input_buffer[0] = '\0';
    return initialization_success = true;
  } // Setup communication channel



  MachineConfig *Stage2SerialTask::getConfig(int i) {
    return machineConfigs[i];
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
      hal->s2heaterToControl = Int1;
      DebugLn<const char *>("Switching to controlling the Int1 Heater!\n");
      clear_buffers(input_buffer);
      return false;
      break;
    case '2':
      hal->s2heaterToControl = Ext1;
      DebugLn<const char *>("Switching to controlling the Ext1 Heater!\n");
      clear_buffers(input_buffer);
      return false;
      break;
    case '3':
      hal->s2heaterToControl = Ext2;
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
      if (DEBUG_LEVEL > 2) {
        DebugLn<const char *>(buffer);
      }
      // Could this be causing a buffer overflow?
      sc = get_set_command_from_JSON(buffer, (uint16_t)256);
      if (DEBUG_LEVEL > 2) {
        render_set_command_raw(&sc);
      }
      // Need to have this modify the correct machine
      MachineState new_ms;
      bool new_ms_set = false;
      if (sc.command == 'W') {
        if (getConfig(hal->s2heaterToControl)->ms == Off) {
          new_ms = Warmup;
          new_ms_set = true;
          Debug<const char *>("New State: Warmup!");
          delay(100);
        } else {
          Debug<const char *>("Can only enter Warmp from Off!");
        }
      } else if (sc.command == 'C') {
        if (getConfig(hal->s2heaterToControl)->ms != Off) {
          new_ms = Cooldown;
          new_ms_set = true;
          Debug<const char *>("New State: Cooldown!");
        }
      } else if (sc.command == 'E') {
        if (getConfig(hal->s2heaterToControl)->ms != Off) {
          new_ms = EmergencyShutdown;
          new_ms_set = true;
          Debug<const char *>("New State: Emergency Shutdown!");
        }
      } else if (sc.command == 'A') {
        if (getConfig(hal->s2heaterToControl)->ms == OffUserAck) {
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
      if (new_ms_set) {
        getConfig(hal->s2heaterToControl)->ms = new_ms;
        Serial.println("new MS");
        Serial.println(hal->s2heaterToControl);
        Serial.println(getConfig(hal->s2heaterToControl)->ms);
      }
    }
  }




}
