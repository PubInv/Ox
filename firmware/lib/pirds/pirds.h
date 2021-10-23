#ifndef PIRDS
#define PIRDS

struct PIRDS {
    char event;
    char type;
    char loc;
    int num;
    int ms;
    int val;
    int sht;
    char* pid[255];
    char* time[255];
};

#endif
