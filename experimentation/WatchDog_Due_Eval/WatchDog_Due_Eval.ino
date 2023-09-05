/* WatchDog_Due_Eval.ino
 * 
 *  By:   (Forrest) Lee Erickson
 *  Date: 20230904
*/

/* Copyright (C) 2023 Forrest Lee Erickson and Public Invention

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

#define PROG_NAME "WatchDog_Due_Eval.ino"
#define VERSION "; Rev: 0.1"  //
#define BAUDRATE 115200
#define DEVICE_UNDER_TEST "Hardware: Due"

const long WATCH_DOG_TIME = 1500; // mS. Verified this has period of about 1.5 seconds
//20:25:05.732 -> Hardware: Due
//20:25:07.415 -> WatchDog_Due_Eval.ino; Rev: 0.1


//const int LED_BUILTIN = 13;
const int D7 = 7;     // An input Make low to make watchdog time out.
char *resetTypes[] = { "general", "backup", "watchdog", "software", "user" };  //Watch dog
//Functions

void watchdogSetup() {
  watchdogEnable(WATCH_DOG_TIME);
}

void checkD7() {
  while (!digitalRead(D7)); //It's a trap!
}//end checkD7

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUDRATE);
  pinMode(D7, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.print(PROG_NAME);
  Serial.println(VERSION);
  Serial.println(DEVICE_UNDER_TEST);

  //Print out the reset reason
  uint32_t resetCause = rstc_get_reset_cause(RSTC) >> RSTC_SR_RSTTYP_Pos;
  Serial.print("ResetCause: ");
  Serial.println(resetTypes[resetCause]);

  watchdogSetup();

  digitalWrite(LED_BUILTIN, LOW);   //At end of setup.
}//end setup()

void loop() {
  // put your main code here, to run repeatedly:

  //Do stuff
  checkD7();
  watchdogReset();

}//end loop()
