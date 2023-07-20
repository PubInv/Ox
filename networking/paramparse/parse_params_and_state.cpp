#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char packetBuffer[2048] = "Name. Experiment#1\n\
TimeStamp. 1680759024\n\
DryRun. false\n\
Nonce. 357\n\
MaxDeltaC. 5\n\
[[ Name. Warmup\n\
[ Phases.Duration. 3600\n\
Phases.Fan.Speed. 20\n\
Phases.Heater.Temp. 400\n\
Phases.Heater.Current. 10\n\
Phases.Heater.Ramp. 40\n\
Phases.Stack.Temp. 400\n\
Phases.Stack.Current. 60\n\
Phases.Stack.Ramp. 5\n\
[ Phases.Duration. 3600\n\
Phases.Fan.Speed. 20\n\
Phases.Heater.Temp. 400\n\
Phases.Heater.Current. 10\n\
Phases.Heater.Ramp. 40\n\
Phases.Stack.Temp. 400\n\
Phases.Stack.Current. 60\n\
Phases.Stack.Ramp. 5\n\
[[ Name. Cooldown\n\
[ Phases.Duration. -1\n\
Phases.Fan.Speed. 30\n\
Phases.Heater.Temp. 35\n\
Phases.Heater.Current. 10\n\
Phases.Heater.Ramp. 4\n\
Phases.Stack.Temp. 35\n\
Phases.Stack.Current. 0\n\
Phases.Stack.Ramp. 1\n\
";

struct phase_t {
  int duration;
  int fan_speed;
  int fan_flow;
  int preheat_temp;
  int preheat_current;
  int preheat_ramp;
  int stack_temp;
  int stack_current;
  int stack_ramp;
};

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

int
parse_param(char *buffer, const char *value, char **rvalue) {
  char *item = strstr(buffer, value);
  if (!item) return 0;

  if (*rvalue) free(*rvalue);

  item += strlen(value);
  while (*item == ' ' || *item == '.') item++;
  char *end = strchr(item, '\n');
  *end = '\0';
  *rvalue = (char *)malloc(strlen(item) + 1);
  strcpy(*rvalue, item);
  *end = '\n';
  return 1;
}

int
parse_param(char *buffer, const char *value, int *rvalue) {
  char *item = strstr(buffer, value);
  if (!item) return 0;

  item += strlen(value);
  while (*item == ' ' || *item == '.') item++;
  char *end = strchr(item, '\n');
  *end = '\0';
  if (strncasecmp(item, "On", 2) == 0) *rvalue = 1;
  else if (strncasecmp(item, "Yes", 3) == 0) *rvalue = 1;
  else if (strncasecmp(item, "True", 4) == 0) *rvalue = 1;
  else if (strncasecmp(item, "Off", 3) == 0) *rvalue = 0;
  else if (strncasecmp(item, "No", 2) == 0) *rvalue = 0;
  else if (strncasecmp(item, "False", 5) == 0) *rvalue = 0;
  else *rvalue = atoi(item);
  *end = '\n';
  return 1;
}

int parse_param_state(char *buffer, const char *value, char **rvalue) {
  char str[strlen(value) + 9];
  sprintf(str, "[[ Name. %s", value);
  char *item = strcasestr(buffer, str);
  if (!item) return 0;

  if (*rvalue) free(*rvalue);

  item += strlen(str);
  while (*item == ' ' || *item == '\n') item++;

  char *end = strstr(item, "[[");
  if (end) *--end = '\0';
  else {
    end = strrchr(item, '\n');
    *end = '\0';
  }
  *rvalue = (char *) malloc(strlen(item) + 1);
  strcpy(*rvalue, item);
  if (end) *end = '\n';
  return 1;
}

int parse_state(char *state, struct phase_t *phase_list[]) {
  if (state == NULL || strlen(state) == 0) return 0;
  char *ptr  = NULL;

  int phasecnt = 0;
  for (ptr = state; *ptr; ptr++) if (*ptr == '[') phasecnt++;

  struct phase_t *pl = (struct phase_t *) malloc(sizeof (struct phase_t) * phasecnt);

  *phase_list = pl;

  char tempphase[strlen(state)];
  strcpy(tempphase, state);

  char *start = tempphase;
  int pcount = 0;
  while (ptr = strtok(start, "[")) {
    start = NULL;
    if (strlen(ptr) == 0) continue;

    char *tptr = NULL;
    if (tptr = strstr(ptr, "Phases.Duration")) {
      tptr += 15;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].duration = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Fan.Speed")) {
      tptr += 16;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].fan_speed = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Fan.Flow")) {
      tptr += 15;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].fan_flow = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Preheat.Temp")) {
      tptr += 19;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].preheat_temp = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Preheat.Current")) {
      tptr += 22;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].preheat_current = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Preheat.Ramp")) {
      tptr += 19;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].preheat_ramp = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Stack.Temp")) {
      tptr += 17;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].stack_temp = atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Stack.Current")) {
      tptr += 20;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].stack_current= atoi(tptr);
    }
    if (tptr = strstr(ptr, "Phases.Stack.Ramp")) {
      tptr += 17;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].stack_ramp = atoi(tptr);
    }
    pcount++;
  }
  return phasecnt;
}

int main(int argc, char *argv[]) {
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

  printf("Name is %s\n", gName);
  printf("Timestamp is %s\n", gTimestamp);
  printf("Dryrun is %s\n", gDryRun ? "True" : "False");
  printf("Nonce is %d\n", gNonce);
  printf("Max UP is %d\n", gMaxRampUp);
  printf("Max DOWN is %d\n", gMaxRampDown);
  printf("Warmup Script:\n%s\n----\n", gWarmUp);
  printf("Cooldown Script:\n%s\n----\n", gCooldown);
  printf("Emergency Script:\n%s\n----\n", gEmShutdown);
  printf("Operation Script:\n%s\n----\n", gOperation);

  struct phase_t *p;
  int c = parse_state(gWarmUp, &p);
  printf("warmup has %d phases\n", c);
  for (int x = 0; x < c; x++) {
    printf("Phase %d:\n", x);
    printf("  Offset %d\n", p[x].duration);
  }
}
