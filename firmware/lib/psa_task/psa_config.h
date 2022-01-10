#ifndef PSA_CONFIG_H
#define PSA_CONFIG_H

#include <valve_controller.h>

namespace OxApp
{
    #define TOTAL_CYCLE_TIME 8000

    ValveConfig psaValveConfig[NUM_VALVES] = {
        { 'A', 0, 0, 1, 100, 4000, },
        { 'B', 1, 0, 2, 4000, 8000, },
        { 'C', 2, 0, 4, 3700, 4000, },
        { 'D', 3, 0, 8, 7700, 8000, }};
}

#endif