#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char udpbuffer[2048] = "Name. Test Name \n\
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

struct phase_t {
  int offset;
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

int parse_param(const char *value, char **rvalue) {
  char *item = strstr(udpbuffer, value);
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

int parse_param(const char *value, int *rvalue) {
  char *item = strstr(udpbuffer, value);
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

int parse_param_state(const char *value, char **rvalue) {
  char str[strlen(value) + 15];
  sprintf(str, "[[ StateName. %s", value);
  char *item = strstr(udpbuffer, str);
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
  *rvalue = (char *)malloc(strlen(item) + 1);
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
    if (tptr = strstr(ptr, "Phases.Offset")) {
      tptr += 13;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].offset = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Fan.Speed")) {
      tptr += 16;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].fan_speed = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Fan.Flow")) {
      tptr += 15;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].fan_flow = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Preheat.Temp")) {
      tptr += 19;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].preheat_temp = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Preheat.Current")) {
      tptr += 22;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].preheat_current = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Preheat.Ramp")) {
      tptr += 19;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].preheat_ramp = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Stack.Temp")) {
      tptr += 17;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].stack_temp = atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Stack.Current")) {
      tptr += 20;
      while (*tptr == ' ' || *tptr == '.') tptr++;
      pl[pcount].stack_current= atoi(tptr);
    } else if (tptr = strstr(ptr, "Phases.Stack.Ramp")) {
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
  parse_param("Nonce", &tempnonce);
  if (tempnonce <= gNonce) exit(1);
  gNonce = tempnonce;
  
  parse_param("Name", &gName);
  parse_param("TimeStamp", &gTimestamp);
  parse_param("DryRun", &gDryRun);
  parse_param("MaxRampUp", &gMaxRampUp);
  parse_param("MaxRampDown", &gMaxRampDown);

  parse_param_state("WarmUp", &gWarmUp);
  parse_param_state("CoolDown", &gCooldown);
  parse_param_state("EmergencyShutdown", &gEmShutdown);
  parse_param_state("Operation", &gOperation);

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
    printf("  Offset %d\n", p[x].offset);
  }
}
