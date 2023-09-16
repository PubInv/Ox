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
    ic.com_c = tempChars[0];

    strtokIndx = strtok(NULL, ":");
    floatFromPC = atof(strtokIndx);
    ic.value_f = floatFromPC;
    return ic;
  }

  void showParsedData(InputCommand ic) {
    Serial.print("Command ");
    Serial.println(ic.com_c);
    Serial.print("Number ");
    Serial.println(ic.value_f);
  }

  // true if a new command found
  bool SerialInputTask::listen(InputCommand &ic) {
    if (DEBUG_SERIAL > 1) {
      Serial.println("listining...");
    }
    recvWithEndMarker();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      // this temporary copy is necessary to protect the original data
      //   because strtok() used in parseData() replaces the commas with \0
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
  } // Setup communication channel


  bool OEDCSSerialInputTask::executeCommand(InputCommand ic) {
    MachineConfig *cogConfig = getConfig();
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }

    switch(ic.com_c) {
    case 's':
      break;
    case 'h':
      break;
    case 'r':
      break;
    };
  }

  bool OEDCSSerialInputTask::_run()
  {
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }
    InputCommand ic;
    if (listen(ic)) {
      executeCommand(ic);
    }
  }


  bool Stage2SerialInputTask::executeCommand(InputCommand ic) {
    MachineConfig *cogConfig = getConfig();
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }

    switch(ic.com_c) {
    case 's':
      break;
    case 'h':
      break;
    case 'r':
      break;
    };
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
