#ifndef MEASURE_FUNCS_H
#define MEASURE_FUNCS_H

#include <time.h>
#include <stdint.h>


struct time_point_t {
    uint64_t    tick_point;
    clock_t     clock_point;
};

time_point_t set_time_point();


#endif // MEASURE_FUNCS_H