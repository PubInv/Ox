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

  void SerialInputTask::showParsedData(InputCommand ic) {
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
  bool SerialInputTask::listen(InputCommand &ric) {
    if (DEBUG_SERIAL > 2) {
      Serial.println("listening...");
    }
    recvWithEndMarker();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      InputCommand ic = parseCommandLine();
      ric.com_c = ic.com_c;
      ric.value_c = ic.value_c;
      ric.value_f = ic.value_f;
      newData = false;
      return true;
    }
    return false;
  }

  bool SerialInputTask::_init() {
    newData = false;
    return true;
  }

  bool OEDCSSerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      Serial.println("OEDSCSerialTask Inited");
    }
    SerialInputTask::_init();
    return true;
  }
  bool Stage2SerialInputTask::_init() {
    if (DEBUG_SERIAL > 1) {
      Serial.println("OEDSCSerialTask Inited");
    }
    SerialInputTask::_init();
    return true;
  }

  void SerialInputTask::processStateChange(InputCommand ic,MachineConfig *mc) {
    if (ic.value_c == '1') {
      if (mc->ms == Off) {
        mc->ms = Warmup;
        mc->clearErrors();
        Debug<const char *>("Turning on: New State: Warmup!");
      } else {
        Debug<const char *>("Already On.");
      }
    } else if (ic.value_c == '0') {
      if (mc->ms != Off) {
        mc->ms = Off;
        Debug<const char *>("New State: Off.");
      } else {
        Debug<const char *>("Already Off.");
      }
    } else {
    }

    // if (ic.value_c == 'W') {
    //   if (mc->ms == Off) {
    //     mc->ms = Warmup;
    //     Debug<const char *>("New State: Warmup!");
    //   }
    // } else if (ic.value_c == 'C') {
    //   if (mc->ms != Off) {
    //     mc->ms = Cooldown;
    //     Debug<const char *>("New State: Cooldown!");
    //   }
    // } else if (ic.value_c == 'E') {
    //   if (mc->ms != Off) {
    //     mc->ms = EmergencyShutdown;
    //     Debug<const char *>("New State: Emergency Shutdown!");
    //   }
    // } else if (ic.value_c == 'A') {
    //   if (mc->ms == OffUserAck) {
    //     mc->ms = Off;
    //     Debug<const char *>("New State: Off!");
    //   }
    // } else if (ic.value_c == 'I') {
    //   mc->idleOrOperate = Idle;
    //   Debug<const char *>("New SubState: Idle!");
    // } else if (ic.value_c == 'O') {
    //   mc->idleOrOperate = Operate;
    //   Debug<const char *>("New SubState: Operate");
    // }
  }

  bool SerialInputTask::executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) {
    if (DEBUG_SERIAL > 1) {
      Serial.println("Serial Input Task executeCommand");
    }

    switch(ic.com_c) {
    case 'S': // set state based on the next character
      processStateChange(ic,mc);
      break;
    case 'H':
      {
        smm->changeTargetTemp(ic.value_f);
        Serial.print("Target Temp changed to: ");
        Serial.println(ic.value_f);
        Serial.print("New state is: ");
        Serial.println(MachineConfig::MachineStateNames[mc->ms]);
      }
      break;
    case 'R':
      {
        float r = min(mc->BOUND_MAX_RAMP,ic.value_f);
        r = max(0.0,r);
        mc->change_ramp(r);
        mc->report->target_ramp_C = r;
        Serial.print("Ramp changed to: ");
        Serial.println(r);
      }
      break;
    default:
      Serial.println("Internal Error!");
    }
  }

  bool OEDCSSerialInputTask::executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) {
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }

    if (ic.com_c == 'S' ||  ic.com_c == 'H' || ic.com_c == 'R') {
      SerialInputTask::executeCommand(ic,mc,smm);
    } else {
      switch(ic.com_c) {
      case 'A':
        {
          float a = min(mc->BOUND_MAX_AMPERAGE_SETTING,ic.value_f);
          a = max(0,a);
          mc->MAX_AMPERAGE = a;
          mc->report->max_stack_amps_A =
            mc->MAX_AMPERAGE;
        Serial.print("Maximum amperage changed to: ");
        Serial.println(a);
        }
        break;
      case 'W':
        {
          float w = min(mc->BOUND_MAX_WATTAGE,ic.value_f);
          w = max(0,w);
          mc->MAX_STACK_WATTAGE = w;
          mc->report->max_stack_watts_W =
            mc->MAX_STACK_WATTAGE;
          Serial.print("Wattage changed to: ");
          Serial.println(w);
        }
      case 'F':
        {
          float f = min(1.0,ic.value_f);
          f =  max(0,f);
          mc->FAN_SPEED = f;
          mc->report->fan_pwm =
            mc->FAN_SPEED;
          Serial.print("Fan Speed changed to: ");
          Serial.println(f);
        }
        break;
      default:
          Serial.print("Unknown command.");
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
      executeCommand(ic,getConfig(),cogTask);
    }
  }

  bool Stage2SerialInputTask::executeCommand(InputCommand ic,MachineConfig* mc,StateMachineManager *smm) {
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }

    showParsedData(ic);
    if ((ic.com_c == 'S') ) {
      processStateChange(ic,mc);
    } else if ((ic.com_c == 'H') || (ic.com_c == 'R')) {
      SerialInputTask::executeCommand(ic,mc,smm);
    } else {
      switch(ic.com_c) {
      case '1':
        {
          mc->hal->s2heaterToControl = Int1;
          DebugLn<const char *>("Switching to controlling the Int1 Heater!\n");
          return false;
        }
        break;
      case '2':
        {
          mc->hal->s2heaterToControl = Ext1;
          DebugLn<const char *>("Switching to controlling the Ext1 Heater!\n");
          return false;
        }
        break;
      case '3':
        {
          mc->hal->s2heaterToControl = Ext2;
          DebugLn<const char *>("Switching to controlling the Ext2 Heater!\n");
          return false;
        }
        break;
      default: {
        Serial.print("unknown command: ");
        Serial.println(ic.com_c);
      }
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
      if (DEBUG_SERIAL > 0) {
        Serial.println("Got command");
        showParsedData(ic);
      }
      executeCommand(ic,
                     mcs[getConfig()->hal->s2heaterToControl],
                     stage2HeaterTasks[getConfig()->hal->s2heaterToControl]);
    }
  }
}
