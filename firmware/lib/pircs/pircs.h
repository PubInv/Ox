#ifndef PIRCS
#define PIRCS

struct PIRCS {
    char ack;
    int err;
    char com;
    char par;
    char in;
    int val;
    char* time[255];
};

#endif
