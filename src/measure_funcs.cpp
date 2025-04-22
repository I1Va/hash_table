#include <immintrin.h>
#include "measure_funcs.h"

time_point_t set_time_point() {
    time_point_t time_point = {};

    time_point.tick_point = __rdtsc();
    time_point.clock_point = clock();

    return time_point;
}
