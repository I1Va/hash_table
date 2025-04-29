#include <cstdint>
#include <cstring>
#include <immintrin.h>
#include <assert.h>
#include <errno.h>

#include "args_proc.h"
#include "general.h"
#include "general_structs.h"
#include "hash_funcs.h"
#include "hash_table_32b.h"
#include "benchmark_funcs.h"


hash_function_t choose_hash_function(char hash_function_name[]) {
    assert(hash_function_name);

    hash_function_t DEFAULT_HASH_FUNCTION = crc32_intrinsic_hash_func;

    if (strncmp(hash_function_name, "cr32_intrinsic", MAX_CONFIG_NAME_SIZE) == 0) {
        return crc32_intrinsic_hash_func;
    }
    if (strncmp(hash_function_name, "cr32", MAX_CONFIG_NAME_SIZE) == 0) {
        return crc32_hash_func;
    }
    if (strncmp(hash_function_name, "poly", MAX_CONFIG_NAME_SIZE) == 0) {
        return polynom_hash_func;
    }
    if (strncmp(hash_function_name, "fchar", MAX_CONFIG_NAME_SIZE) == 0) {
        return first_char_hash_func;
    }
    if (strncmp(hash_function_name, "fnv", MAX_CONFIG_NAME_SIZE) == 0) {
        return fnv1a_hash;
    }
    return DEFAULT_HASH_FUNCTION;
}

time_point_t set_time_point() {
    time_point_t time_point = {};

    time_point.tick_point = __rdtsc();
    time_point.clock_point = clock();

    return time_point;
}

static volatile uint64_t dummy_variable = 0;
void measure_hashing_time(hash_function_t hash_function, tests_data_t tests_data, time_point_t *duration) {
    assert(tests_data.words_32b);
    assert(duration);
    assert(hash_function);

    time_point_t start_point = set_time_point();
    for (size_t i = 0; i < tests_data.words_cnt; i++) {
        char *key_32b = tests_data.words_32b + i * WORD_32B_NMEMB;
        dummy_variable = hash_function(key_32b, WORD_32B_NMEMB);
    }
    time_point_t end_point = set_time_point();

    duration->clock_point = end_point.clock_point - start_point.clock_point;
    duration->tick_point = end_point.tick_point - start_point.tick_point;
}

bool measure_testing_time(hash_table_32b_t *hash_table, tests_data_t tests_data, time_point_t *duration) {
    assert(hash_table);
    assert(duration);

    time_point_t start_point = {};
    time_point_t end_point = {};


    store_text_in_hash_table(tests_data, hash_table);

    start_point = set_time_point();
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

    char command[MAX_CONFIG_NAME_SIZE * 2] = {};

    snprintf(command, MAX_CONFIG_NAME_SIZE * 2, "rm -f %s", path);

    if (system(command) == -1) {
        debug("'%s' failed, errno='%s'", command, strerror(errno));
        return false;
    }

    return true;
}

bool run_hashes_benchmarks(config_t *config) {
    assert(config);

    tests_data_t tests_data = {};
    FILE *benchmarks_res_file = NULL;
    hash_table_32b_t hash_table = {};
    time_point_t duration = {};

    tests_data = load_text(TEXT_PATH);
    if (tests_data.words_32b == NULL) {
        debug("load_text '%s' failed", TEXT_PATH);
        return false;
    }

    delete_file(config->output_path);

    benchmarks_res_file = fopen(config->output_path, "w");
    if (benchmarks_res_file == NULL) {
        debug("failed to open '%s'", config->output_path);
        CLEAR_MEMORY(exit_mark)
    }

    if (!hash_table_32b_t_ctor(&hash_table, HASH_TABLE_SZ, choose_hash_function(config->hash_func_name))) {
        debug("hash_table_32b_t_ctor failed");
        CLEAR_MEMORY(exit_mark)
    }

    store_text_in_hash_table(tests_data, &hash_table);
    hash_table_32b_dump_bucket_sizes(benchmarks_res_file, &hash_table);

    measure_hashing_time(hash_table.hash_function, tests_data, &duration);
    fprintf(benchmarks_res_file, "ticks %lu\n", duration.tick_point);

    free(tests_data.words_32b);
    fclose(benchmarks_res_file);
    return true;

    exit_mark:
    hash_table_t_dtor(&hash_table);
    if (tests_data.words_32b) free(tests_data.words_32b);
    if (benchmarks_res_file) fclose(benchmarks_res_file);

    return false;
}

bool run_versions_benchmarks(config_t *config) {
    assert(config);

    tests_data_t tests_data = load_text(TEXT_PATH);
    if (tests_data.words_32b == NULL) {
        debug("load_text '%s' failed", TEXT_PATH);
        return false;
    }


    hash_table_32b_t hash_table = {};
    FILE *benchmarks_res_file = NULL;
    if (strnlen(config->output_path, MAX_CONFIG_NAME_SIZE) > 0) {
        delete_file(config->output_path);
        benchmarks_res_file = fopen(config->output_path, "a");
        if (benchmarks_res_file == NULL) {
            debug("failed to open '%s'", config->output_path);
            CLEAR_MEMORY(exit_mark)
        }
    }



    for (int i = 0; i < config->measures_cnt; i++) {
        hash_table = {};
        if (!hash_table_32b_t_ctor(&hash_table, HASH_TABLE_SZ, choose_hash_function(config->hash_func_name))) {
            debug("hash_table_32b_t_ctor failed");
            CLEAR_MEMORY(exit_mark_free_hash_table)
        }

        time_point_t duration = {};
        if (!measure_testing_time(&hash_table, tests_data, &duration)) {
            debug("test iteration <%d> failed", i);
            CLEAR_MEMORY(exit_mark_free_hash_table)
        }

        if (benchmarks_res_file) fprintf(benchmarks_res_file, "%lu\n", duration.tick_point);
        if (config->print_state) printf("ticks : %lu\n", duration.tick_point);

        hash_table_t_dtor(&hash_table);
    }

    free(tests_data.words_32b);
    if (benchmarks_res_file) fclose(benchmarks_res_file);
    return true;


    exit_mark_free_hash_table:
    hash_table_t_dtor(&hash_table);
    exit_mark:
    if (tests_data.words_32b) free(tests_data.words_32b);
    if (benchmarks_res_file) fclose(benchmarks_res_file);

    return false;
}

bool run_load_factor_benchmarks(config_t *config) {
    assert(config);

    tests_data_t tests_data = {};
    FILE *benchmarks_res_file = NULL;
    hash_table_32b_t hash_table = {};
    double load_factor = 0;

    tests_data = load_text(TEXT_PATH);
    if (tests_data.words_32b == NULL) {
        debug("load_text '%s' failed", TEXT_PATH);
        return false;
    }

    if (strnlen(config->output_path, MAX_CONFIG_NAME_SIZE) > 0) {
        delete_file(config->output_path);
        benchmarks_res_file = fopen(config->output_path, "a");
        if (benchmarks_res_file == NULL) {
            debug("failed to open '%s'", config->output_path);
            CLEAR_MEMORY(exit_mark)
        }
    }

    if (!hash_table_32b_t_ctor(&hash_table, HASH_TABLE_SZ, choose_hash_function(config->hash_func_name))) {
        debug("hash_table_32b_t_ctor failed");
        CLEAR_MEMORY(exit_mark)
    }

    store_text_in_hash_table(tests_data, &hash_table);

    load_factor = hash_table_32b_get_load_factor(&hash_table);
    if (benchmarks_res_file) fprintf(benchmarks_res_file, "%lf\n", load_factor);
    if (config->print_state) printf("load_factor : %lf\n", load_factor);

    free(tests_data.words_32b);
    fclose(benchmarks_res_file);
    return true;

    exit_mark:
    hash_table_t_dtor(&hash_table);
    if (tests_data.words_32b) free(tests_data.words_32b);
    if (benchmarks_res_file) fclose(benchmarks_res_file);

    return false;
}
