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


#ifndef FLASH_H
#define FLASH_H

//Defines so the device can do a self reset
#define SYSRESETREQ    (1<<2)
#define VECTKEY        (0x05fa0000UL)
#define VECTKEY_MASK   (0x0000ffffUL)
#define AIRCR          (*(uint32_t*)0xe000ed0cUL) // fixed arch-defined address
#define REQUEST_EXTERNAL_RESET (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)

#include <DueFlashStorage.h>
extern DueFlashStorage dfs;

// The struct of the configuration.
struct Configuration {
  uint32_t boot_count;  //Boot count
  uint16_t watchdog_count;  //Flash write count
  uint8_t state;
};

// initialize Flash storage struct
extern Configuration configuration;

extern byte mac[6];
extern char macString[20];

uint32_t getResetCause();
uint32_t setGlobalMacAddress();

#endif
