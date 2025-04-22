#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "args_proc.h"
#include "benchmark_funcs.h"

int main(const int argc, const char *argv[]) {
    config_t config = {};
    scan_argv(&config, argc, argv);

    if (strncmp(config.benchmark, "hash", MAX_CONFIG_NAME_SIZE) == 0) {
        if (!run_hashes_benchmarks(&config)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (strncmp(config.benchmark, "ver", MAX_CONFIG_NAME_SIZE) == 0) {
        if (!run_versions_benchmarks(&config)) {
             return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (strncmp(config.benchmark, "load", MAX_CONFIG_NAME_SIZE) == 0) {
        if (!run_load_factor_benchmarks(&config)) {
             return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
