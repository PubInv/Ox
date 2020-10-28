#ifndef SHIFT_H
#define SHIFT_H

/*#define DS 13 // 747HC pin 14 - serial data
#define ST_CP 12 // 747HC pin 12 - storage register clock (latch)
#define SH_CP 27 // 747HC pin 11 - shift register clock*/
struct shift_pins {
    int latch;
    int clock;
    int data;
};

void shift();

#endif
