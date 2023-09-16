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


  const byte numChars = 32;
  char receivedChars[numChars];
  char tempChars[numChars];        // temporary array for use when parsing

  // variables to hold the parsed data
  char messageFromPC[numChars] = {0};
  int integerFromPC = 0;
  float floatFromPC = 0.0;

  boolean newData = false;


  void recvWithEndMarker() {
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
        receivedChars[ndx] = '\0'; // terminate the string
        ndx = 0;
        newData = true;
      }
    }
  }

  InputCommand parseCommandLine() {      // split the data into its parts
    InputCommand ic;

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,":");      // get the first part - the string
    strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
    ic.com_c = tempChars[0];

    strtokIndx = strtok(NULL, ":");
    floatFromPC = atof(strtokIndx);     // convert this part to a float
    ic.value_f = floatFromPC;
    return ic;
  }

  void showParsedData(InputCommand ic) {
    Serial.print("Command ");
    Serial.println(ic.com_c);
    Serial.print("Number ");
    Serial.println(ic.value_f);
  }


  bool SerialInputTask::_init() {
    input_buffer[0] = '\0';
    return true;
  } // Setup communication channel


  bool OEDCSSerialTask::_init() {
    if (DEBUG_SERIAL > 1) {
      Serial.println("OEDSCSerialTask Inited");
    }

    input_buffer[0] = '\0';
    return true;
  } // Setup communication channel

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
      newData = false;
      return true;
    }
    return false;
  }

  bool OEDCSSerialTask::executeCommand(InputCommand ic) {
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

  bool OEDCSSerialTask::_run()
  {
    if (DEBUG_SERIAL > 1) {
      Serial.println("executeCommand");
    }
    InputCommand ic;
    if (listen(ic)) {
      executeCommand(ic);
    }
  }
}
