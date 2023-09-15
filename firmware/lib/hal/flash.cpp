// Copyright (C) 2023 Robert Read.

// This program includes free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// See the GNU Affero General Public License for more details.
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#include <Arduino.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <network_task.h>
#include <network_udp.h>
#include <flash.h>

#define FLASH_ACCESS_MODE_128    EFC_ACCESS_MODE_128
#define FLASH_ACCESS_MODE_64     EFC_ACCESS_MODE_64

#define SDCARD_CS 4

static uint32_t resetCause = 0;

#ifdef RIBBONFISH
byte mac[6] = { 0xFE, 0xED, 0x03, 0x04, 0x05, 0x06 };
#else
byte mac[6] = { 0xFE, 0xED, 0x04, 0x05, 0x06, 0x07 };
#endif

char macString[20];

DueFlashStorage dfs;
Configuration configuration;

uint32_t
getResetCause() {
  if (!resetCause)
    resetCause = rstc_get_reset_cause(RSTC) >> RSTC_SR_RSTTYP_Pos;
  return resetCause;
}

uint32_t
setGlobalMacAddress() {
  uint32_t rv = efc_init((Efc *) EFC0, FLASH_ACCESS_MODE_128, 4);
  if (rv != EFC_RC_OK) return rv;

  uint32_t uid_buf[4];
  rv = efc_perform_read_sequence((Efc *)EFC0, EFC_FCMD_STUI, EFC_FCMD_SPUI, uid_buf, 4);
  if (rv != EFC_RC_OK) return EFC_RC_ERROR;

  //  Serial.print(F(" ID = "));
  //  Serial.print(uid_buf[0]);Serial.print(F(","));
  //  Serial.print(uid_buf[1]);Serial.print(F(","));
  //  Serial.print(uid_buf[2]);Serial.print(F(","));
  //  Serial.println(uid_buf[3]);

  uint32_t hash32 = uid_buf[0];
  hash32 ^= uid_buf[1];
  hash32 ^= uid_buf[2];
  hash32 ^= uid_buf[3];

  mac[0] = 0xFE;
  mac[1] = 0xED;
  mac[2] = (hash32>>24) & 0xFF;
  mac[3] = (hash32>>16) & 0xFF;
  mac[4] = (hash32>>8) & 0xFF;
  mac[5] = (hash32>>0) & 0xFF;

  sprintf(macString, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.print(F("MAC Address: "));
  Serial.println(macString);
  return 0;
}

void
initFlashConfiguration() {
  /* Flash is erased every time new code is uploaded.
     Write the default configuration to flash if first time */
  if (dfs.read(0) != 0) { // first run
    //    Serial.print("FIRSTRUN  ");
    configuration.boot_count = 1;
    configuration.watchdog_count = 0;
    configuration.state = 0;
  } else {
    byte* b = dfs.readAddress(4); 
    memcpy(&configuration, b, sizeof(Configuration));
    // Serial.print(" update flash ");
    configuration.boot_count++;
    if ((rstc_get_reset_cause(RSTC) >> RSTC_SR_RSTTYP_Pos) == 2)
      configuration.watchdog_count++;
    configuration.state = 0;
  }
  byte b2[sizeof(Configuration)];
  memcpy(b2, &configuration, sizeof(Configuration));
  dfs.write(4, b2, sizeof(Configuration));
  if (dfs.read(0) != 0) dfs.write(0, 0); // set flash OK flash
  //  Serial.println("done");
}

void
writeFlashConfiguration() {
  byte *b = dfs.readAddress(4); 
  uint8_t needupdate = 0;
  byte b2[sizeof(Configuration)];
  memcpy(b2, &configuration, sizeof(Configuration));
  for (uint8_t i = 0; i < sizeof(Configuration); i++) {
    if (b[i] != b2[i]) {
      needupdate++;
      break;
    }
  }
  if (needupdate) dfs.write(4, b2, sizeof(Configuration));
}
