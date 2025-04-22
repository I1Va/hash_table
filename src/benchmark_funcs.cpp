#include <cstring>
#include <immintrin.h>
#include <assert.h>
#include <errno.h>

#include "args_proc.h"
#include "general.h"
#include "hash_funcs_32b.h"
#include "benchmark_funcs.h"


hash_function_32b_t choose_hash_function(char hash_function_name[]) {
    assert(hash_function_name);

    hash_function_32b_t DEFAULT_HASH_FUNCTION = crc32_hash_func;

    if (strncmp(hash_function_name, "cr32", MAX_CONFIG_NAME_SIZE)) {
        return crc32_hash_func;
    }
    if (strncmp(hash_function_name, "poli", MAX_CONFIG_NAME_SIZE)) {
        return polinom_hash_func;
    }

    return DEFAULT_HASH_FUNCTION;
}

time_point_t set_time_point() {
    time_point_t time_point = {};

    time_point.tick_point = __rdtsc();
    time_point.clock_point = clock();

    return time_point;
}

bool measure_testing_time(hash_table_32b_t *hash_table, tests_data_t tests_data, time_point_t *duration) {
    assert(hash_table);
    assert(duration);

    time_point_t start_point = {};
    time_point_t end_point = {};

    start_point = set_time_point();
    store_text_in_hash_table(tests_data, hash_table);

    if (!run_tests(TESTS_PATH, hash_table)) {
        debug("run_tests failed");
        return false;
    }

    end_point = set_time_point();

    duration->clock_point = end_point.clock_point - start_point.clock_point;
    duration->tick_point = end_point.tick_point - start_point.tick_point;

    return true;
}

bool delete_file(const char path[]) {
    assert(path);

    char command[MAX_CONFIG_NAME_SIZE] = {};

    snprintf(command, MAX_CONFIG_NAME_SIZE, "rm -f %s", path);

    if (system(command) == -1) {
        debug("'%s' failed, errno='%s'", command, strerror(errno));
        return false;
    }

    return true;
}

bool run_benchmarks(config_t *config) {
    assert(config);

    tests_data_t tests_data = load_text(TEXT_PATH);
    if (tests_data.words_32b == NULL) {
        debug("load_text '%s' failed", TEXT_PATH);
        return false;
    }

    delete_file(config->output_path);
    FILE *benchmarks_res_file = fopen(config->output_path, "a");

    hash_table_32b_t hash_table = {};

    if (benchmarks_res_file == NULL) {
        debug("failed to open '%s'", config->output_path);
        CLEAR_MEMORY(exit_mark)
    }

    for (int i = 0; i < config->measures_cnt; i++) {
        hash_table = {};
        hash_table_32b_t_ctor(&hash_table, HASH_TABLE_SZ, choose_hash_function(config->hash_func_name));

        time_point_t duration = {};
        if (!measure_testing_time(&hash_table, tests_data, &duration)) {
            debug("test iteration <%d> failed", i);
            CLEAR_MEMORY(exit_mark_free_hash_table)
        }

        fprintf(benchmarks_res_file, "%lu\n", duration.tick_point);

        hash_table_t_dtor(&hash_table);
    }

    free(tests_data.words_32b);
    fclose(benchmarks_res_file);
    return true;


    exit_mark_free_hash_table:
    hash_table_t_dtor(&hash_table);
    exit_mark:
    if (tests_data.words_32b) free(tests_data.words_32b);
    if (benchmarks_res_file) fclose(benchmarks_res_file);

    return false;
}
