#include <cassert>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

#include "benchmark_funcs.h"

int main(const int argc, const char *argv[]) {
    config_t config = {};
    scan_argv(&config, argc, argv);
    run_benchmarks(&config);
}
