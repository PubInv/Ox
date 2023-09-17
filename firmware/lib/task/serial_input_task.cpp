/*
  serial_input_task.cpp -- parse serial input to commands
  Copyright (C) 2023 Robert Read.


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

#include <Arduino.h>

#include <debug.h>
#include <serial_input_task.h>
#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <stage2_hal.h>

using namespace OxCore;
#define DEBUG_SERIAL_LISTEN 0
#define DEBUG_INPUT 0

namespace OxApp
{
  void SerialInputTask::recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && newData == false) {
      rc = Serial.read();
      // We'll do an echo here, we have to do it explicitly...
      Serial.print(rc);

      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0';
        ndx = 0;
        newData = true;
      }
    }
  }

  InputCommand SerialInputTask::parseCommandLine() {
    InputCommand ic;

    char * strtokIndx;

    strtokIndx = strtok(tempChars,":");
    strcpy(messageFromPC, strtokIndx);
    ic.com_c = toupper ( tempChars[0] );

    strtokIndx = strtok(NULL, ":");
    if (ic.com_c == 'S') {
      ic.value_c = toupper( strtokIndx[0]) ;
    } else {
      floatFromPC = atof(strtokIndx);
      ic.value_f = floatFromPC;
    }
    return ic;
  }

  void showParsedData(InputCommand ic) {
    Serial.print("Command ");
    Serial.println(ic.com_c);
    if (ic.com_c == 'S') {
      Serial.print("State ");
      Serial.println(ic.value_c);
    } else {
      Serial.print("Value ");
      Serial.println(ic.value_f);
    }
  }

  // true if a new command found
  bool SerialInputTask::listen(InputCommand &ic) {
    if (DEBUG_SERIAL > 2) {
      Serial.println("listening...");
    }
    recvWithEndMarker();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      InputCommand ic = parseCommandLine();
      showParsedData(ic);
      executeCommand(ic);
      newData = false;
      return true;
    }
    return false;
  }

  bool SerialInputTask::_init() {
    input_buffer[0] = '\0';
    return true;
  }

  bool OEDCSSerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      Serial.println("OEDSCSerialTask Inited");
    }

    input_buffer[0] = '\0';
    return true;
  }

  void SerialInputTask::processStateChange(InputCommand ic) {
    Serial.println("processStateChange called");
      if (ic.value_c == 'W') {
        if (getConfig()->ms == Off) {
          getConfig()->ms = Warmup;
          Debug<const char *>("New State: Warmup!");
        }
      } else if (ic.value_c == 'C') {
        if (getConfig()->ms != Off) {
          getConfig()->ms = Cooldown;
          Debug<const char *>("New State: Cooldown!");
        }
      } else if (ic.value_c == 'E') {
        if (getConfig()->ms != Off) {
          getConfig()->ms = EmergencyShutdown;
          Debug<const char *>("New State: Emergency Shutdown!");
        }
      } else if (ic.value_c == 'A') {
        if (getConfig()->ms == OffUserAck) {
          getConfig()->ms = Off;
          Debug<const char *>("New State: Off!");
        }
      } else if (ic.value_c == 'I') {
        getConfig()->idleOrOperate = Idle;
        Debug<const char *>("New SubState: Idle!");
      } else if (ic.value_c == 'O') {
        getConfig()->idleOrOperate = Operate;
        Debug<const char *>("New SubState: Operate");
      }
  }

  bool SerialInputTask::executeCommand(InputCommand ic) {
    if (DEBUG_SERIAL > 1) {
      Serial.println("Serial Input Task executeCommand");
    }

    switch(ic.com_c) {
    case 's': // set state based on the next character
      processStateChange(ic);
      break;
    case 'h':
      getConfig()->TARGET_TEMP = ic.value_f;
      getConfig()->report->target_temp_C =
        getConfig()->TARGET_TEMP;
      break;
    case 'r':
      getConfig()->change_ramp(ic.value_f);
      getConfig()->report->target_ramp_C = ic.value_f;
      break;
    }
  }

  bool OEDCSSerialInputTask::executeCommand(InputCommand ic) {
    MachineConfig *cogConfig = getConfig();
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }

    if (ic.com_c == 'S' ||  ic.com_c == 'H' || ic.com_c == 'R') {
      processStateChange(ic);
    } else {
      switch(ic.com_c) {
      case 'A':
        getConfig()->MAX_AMPERAGE = ic.value_f;
        getConfig()->report->max_stack_amps_A =
          getConfig()->MAX_AMPERAGE;
        break;
      case 'W':
        getConfig()->MAX_STACK_WATTAGE = ic.value_f;
        getConfig()->report->max_stack_watts_W =
          getConfig()->MAX_STACK_WATTAGE;
      case 'F':
        getConfig()->FAN_SPEED = ic.value_f;
        getConfig()->report->fan_pwm =
          getConfig()->FAN_SPEED;
        break;
      };
    }
  }

  bool OEDCSSerialInputTask::_run()
  {
    if (DEBUG_SERIAL > 2) {
      Serial.println("OEDCS SerialInput Taske Run");
    }
    InputCommand ic;
    if (listen(ic)) {
      executeCommand(ic);
    }
  }


  bool Stage2SerialInputTask::executeCommand(InputCommand ic) {
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }

    if (ic.com_c == 'S' ||  ic.com_c == 'H' || ic.com_c == 'R') {
      processStateChange(ic);
    } else {
      switch(ic.com_c) {
      case '1':
        hal->s2heaterToControl = Int1;
        DebugLn<const char *>("Switching to controlling the Int1 Heater!\n");
        return false;
        break;
      case '2':
        hal->s2heaterToControl = Ext1;
        DebugLn<const char *>("Switching to controlling the Ext1 Heater!\n");
        return false;
        break;
      case '3':
        hal->s2heaterToControl = Ext2;
        DebugLn<const char *>("Switching to controlling the Ext2 Heater!\n");
        return false;
        break;
      }
    }
  }

  bool Stage2SerialInputTask::_run()
  {
    if (DEBUG_SERIAL > 1) {
      Serial.println("Stage2SerialInputTask run");
    }
    InputCommand ic;
    if (listen(ic)) {
      executeCommand(ic);
    }
  }
}
