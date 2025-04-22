#include <cassert>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

#include "general.h"
#include "general_structs.h"
#include "hash_table.h"
#include "measure_funcs.h"
#include "data_functions.h"

const size_t HASH_TABLE_SZ = 1024 * 8;
const char TEXT_PATH[] = "data/text.txt";
const char TESTS_PATH[] = "data/tests.txt";
const size_t TEXT_ALIGNMENT = 1;

int main() {
    hash_table_t hash_table = {};
    hash_table_t_ctor(&hash_table, HASH_TABLE_SZ, NULL);

    debug("DEBUG ON");

    string_t text = load_text(TEXT_PATH, TEXT_ALIGNMENT);
    if (text.ptr == NULL) {
        debug("load_text '%s' failed", TEXT_PATH);
        return EXIT_FAILURE;
    }

    store_text_in_hash_table(text, &hash_table);

    printf_red("load factor: %lf\n", get_load_factor(&hash_table));

    time_point_t start_point = set_time_point();
    if (run_tests(TESTS_PATH, &hash_table) == EXIT_FAILURE) {
        debug("run_tests failed");
        return EXIT_FAILURE;
    }

    time_point_t end_point = set_time_point();

    printf_red("ticks:   %lu\n", end_point.tick_point - start_point.tick_point);
    printf_red("seconds: %lf\n", ((double) (end_point.clock_point - start_point.clock_point)) / CLOCKS_PER_SEC);

    hash_table_t_dtor(&hash_table);
    free((char *) text.ptr);
    return 0;
}
