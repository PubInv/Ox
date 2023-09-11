#define BAUDRATE 9600

// this should BE longer than the delay in startup waiting for flash
const long WATCH_DOG_TIME = 1000; // mS.

#include <DueFlashStorage.h>
DueFlashStorage dfs;
byte mac[6] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// The struct of the configuration.
struct Configuration {
  uint32_t boot_count;  //Boot count
  uint16_t watchdog_count;  //Flash write count
  uint8_t state;
};

// initialize Flash storage struct
Configuration configuration;

void
safeDelay(uint16_t timeout) {
  if (timeout >= WATCH_DOG_TIME) {
    for (uint8_t i = 0; i < timeout / (WATCH_DOG_TIME / 2); i++) {
      watchdogReset();
      delay(WATCH_DOG_TIME / 2);
    }
  } else delay(timeout);
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

uint32_t
setGlobalMacAddress() {
  uint32_t rv = efc_init((Efc *) EFC0, FLASH_ACCESS_MODE_128, 4);
  if (rv != EFC_RC_OK) return rv;

  uint32_t uid_buf[4];
  rv = efc_perform_read_sequence((Efc *)EFC0, EFC_FCMD_STUI, EFC_FCMD_SPUI, uid_buf, 4);
  if (rv != EFC_RC_OK) return EFC_RC_ERROR;

   //  Serial.print(F(" ID = "));
   //  Serial.print(uid_buf[0]);Serial.print(",");
   //  Serial.print(uid_buf[1]);Serial.print(",");
   //  Serial.print(uid_buf[2]);Serial.print(",");
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

  Serial.print("MAC Address: ");
  for(uint8_t i = 0; i < 6; i++) {
    if (mac[i] < 10) Serial.print(F("0"));
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(F(":"));
  }
  Serial.println();
  return 0;
}

void watchdogSetup() {
  watchdogEnable(WATCH_DOG_TIME);
}

void setup() {
  Serial.begin(BAUDRATE);
  //CAUTION This delay was important for correct read of flash.
  safeDelay(3000);

  //Print out the reset reason
  char *resetTypes[] = { "general", "backup", "watchdog", "software", "user" };
  uint32_t resetCause = rstc_get_reset_cause(RSTC) >> RSTC_SR_RSTTYP_Pos;
  Serial.print("ResetCause: ");
  Serial.println(resetTypes[resetCause]);

  if (setGlobalMacAddress()) {
    mac[0] = 0xFE;
    mac[1] = 0xED;
    mac[2] = 0x03;
    mac[3] = 0x04;
    mac[4] = 0x05;
    mac[5] = 0x06;
  }
  initFlashConfiguration();
}

int x = 0;
void loop() {
  Serial.print("Boot #:"); Serial.print(configuration.boot_count);
  Serial.print(" Watchdog Count:"); Serial.print(configuration.watchdog_count);
  Serial.println();
  
  if ( configuration.boot_count < 5 ) while(1);

  while(1) {
    watchdogReset();
    safeDelay(1000);
  }
}
