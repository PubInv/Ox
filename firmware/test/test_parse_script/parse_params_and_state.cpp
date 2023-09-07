#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <debug.h>
#include <machine_script.h>

using namespace OxCore;

char packetBuffer[2048] = "Name. Test Name \n\
TimeStamp. 1680759024\n\
DryRun. TRUE\n\
Nonce. 123\n\
MaxRampUp. 10\n\
MaxRampDown. 10\n\
[[ StateName. WarmUp\n\
[ Phases.Offset. 10\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[ Phases.Offset. 11\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[[ StateName. Cooldown\n\
[ Phases.Offset. 0\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[ Phases.Offset. 0\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[[ StateName. EmergencyShutdown\n\
[ Phases.Offset. 0\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[ Phases.Offset. 0\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[[ StateName. Operation\n\
[ Phases.Offset. 0\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
[ Phases.Offset. 0\n\
Phases.Fan.Speed. 0\n\
Phases.Fan.Flow. 0\n\
Phases.Preheat.Temp. 0\n\
Phases.Preheat.Current. 0\n\
Phases.Preheat.Ramp. 0\n\
Phases.Stack.Temp. 0\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 0\n\
";

char *gName = NULL;
char *gTimestamp = NULL;
int gDryRun = 1;
int gNonce = -1;
int gMaxRampUp = 1;
int gMaxRampDown = 2;
char *gWarmUp = NULL;
char *gCooldown = NULL;
char *gEmShutdown = NULL;
char *gOperation = NULL;


int process() {
  int tempnonce = 0;
  parse_param(packetBuffer, "Nonce", &tempnonce);
  if (tempnonce <= gNonce) exit(1);
  gNonce = tempnonce;

  parse_param(packetBuffer, "Name", &gName);
  parse_param(packetBuffer, "TimeStamp", &gTimestamp);
  parse_param(packetBuffer, "DryRun", &gDryRun);
  parse_param(packetBuffer, "MaxRampUp", &gMaxRampUp);
  parse_param(packetBuffer, "MaxRampDown", &gMaxRampDown);

  parse_param_state(packetBuffer, "WarmUp", &gWarmUp);
  parse_param_state(packetBuffer, "CoolDown", &gCooldown);
  parse_param_state(packetBuffer, "EmergencyShutdown", &gEmShutdown);
  parse_param_state(packetBuffer, "Operation", &gOperation);

  Debug<const char *>("Name is \n");
  Debug<const char *>(gName);
  Debug<const char *>("Timestamp is \n");
  Debug<const char *>(gTimestamp);
  Debug<const char *>("Dryrun is \n");
  Debug<const char *>( gDryRun ? "True" : "False");
  Debug<const char *>("Nonce is n");
  Debug<int>(gNonce);
  Debug<const char *>("Max UP is %d\n");
  Debug<int>(gMaxRampUp);
  Debug<const char *>("Max DOWN is n");
  Debug<int>(gMaxRampDown);
  Debug<const char *>("Warmup Script:\n");
  Debug<const char *>(gWarmUp);
  Debug<const char *>("\n----\n");
  Debug<const char *>("Cooldown Script:\n");
  Debug<const char *>("\n----\n");
  Debug<const char *>(gCooldown);
  Debug<const char *>("Emergency Script:\n");
  Debug<const char *>("\n----\n");
  Debug<const char *>(gEmShutdown);
  Debug<const char *>("Operation Script:\n");
  Debug<const char *>("\n----\n");
  Debug<const char *>(gOperation);
  Debug<const char *>("\n----\n");

  struct phase_t *p;
  int c = parse_state(gWarmUp, &p);
  Debug<const char *>("warmup has %d phases\n");
  DebugLn<int>(c);
  for (int x = 0; x < c; x++) {
    Debug<const char *>("Phase :\n");
    DebugLn<int>(x);
    Debug<const char *>("  Offset \n");
    DebugLn<int>(p[x].offset);
  }
}

#ifdef ARDUINO
#include <Arduino.h>

void setup() {
#ifdef RIBBONFISH
  OxCore::serialBegin(115200UL);
#endif
    process();
}
void loop() {
    Debug<const char *>("\n\nScript Ending As Expected.\n");
    abort();
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif
