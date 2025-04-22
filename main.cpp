#include <cassert>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

#include "general.h"
#include "hash_table_32b.h"
#include "measure_funcs.h"
#include "data_functions.h"

const size_t HASH_TABLE_SZ = 1024 * 8;
const char TEXT_PATH[] = "data/text.txt";
const char TESTS_PATH[] = "data/tests.txt";

bool run_benchmarks() {
    hash_table_32b_t hash_table = {};
    hash_table_32b_t_ctor(&hash_table, HASH_TABLE_SZ, crc32_hash_func);

    tests_data_t tests_data = load_text(TEXT_PATH);
    if (tests_data.words_32b == NULL) {
        debug("load_text '%s' failed", TEXT_PATH);
        return false;
    }

    time_point_t start_point = {};
    time_point_t end_point = {};

    start_point = set_time_point();
    store_text_in_hash_table(tests_data, &hash_table);
    printf_red("load factor: %lf\n", get_load_factor(&hash_table));

    if (!run_tests(TESTS_PATH, &hash_table)) {
        debug("run_tests failed");
        CLEAR_MEMORY(exit_mark)
    }

    end_point = set_time_point();

    printf_red("ticks:   %lu\n", end_point.tick_point - start_point.tick_point);
    printf_red("seconds: %lf\n", ((double) (end_point.clock_point - start_point.clock_point)) / CLOCKS_PER_SEC);

    hash_table_t_dtor(&hash_table);
    free(tests_data.words_32b);
    return true;

    exit_mark:

    hash_table_t_dtor(&hash_table);
    if (tests_data.words_32b) free(tests_data.words_32b);

    return false;
}

int main() {
    run_benchmarks();
}