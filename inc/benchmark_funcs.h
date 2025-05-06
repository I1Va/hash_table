#ifndef BENCHMARK_FUNCS_H
#define BENCHMARK_FUNCS_H

#include <time.h>
#include <stdint.h>
#include <assert.h>

#include "hash_table_32b.h"
#include "data_functions.h"
#include "args_proc.h"


struct time_point_t {
    uint64_t    tick_point;
};

const size_t HASH_TABLE_SZ = 512;
const char TEXT_PATH[] = "data/text.txt";
const char TESTS_PATH[] = "data/tests.txt";

time_point_t set_time_point();
bool measure_testing_time(hash_table_32b_t *hash_table, tests_data_t tests_data, time_point_t *duration);
bool delete_file(const char path[]);
bool run_versions_benchmarks(config_t *config);
hash_function_t choose_hash_function(char hash_function_name[]);
bool run_hashes_benchmarks(config_t *config);
bool run_load_factor_benchmarks(config_t *config);
void measure_hashing_time(hash_function_t hash_function, tests_data_t tests_data, time_point_t *duration);
bool measure_version_time(const char path[], hash_table_32b_t *hash_table, time_point_t *duration, const size_t measure_repeats);

#endif // BENCHMARK_FUNCS_H