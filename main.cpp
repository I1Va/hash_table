#include <cassert>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

#include "general.h"
#include "general_structs.h"
#include "hash_table.h"
#include "hash_functions.h"
#include "data_functions.h"

const size_t HASH_TABLE_SZ = 1024;
const char TEXT_PATH[] = "data/text.txt";
const char TESTS_PATH[] = "data/tests.txt";
const size_t ALIGNMENT = 256;

int main() {
    hash_table_t hash_table = {};
    hash_table_t_ctor(&hash_table, HASH_TABLE_SZ, polinom_hash_func);

    string_t text = load_text(TEXT_PATH, ALIGNMENT);
    if (text.ptr == NULL) {
        debug("load_text '%s' failed\n", TEXT_PATH);
        return EXIT_FAILURE;
    }

    store_text_in_hash_table(text, &hash_table);

    printf_red("load factor: %lf\n", get_load_factor(&hash_table));

    uint64_t res_ticks = 0;
    run_tests(TESTS_PATH, &hash_table, &res_ticks);

    printf_red("ticks: %lu\n", res_ticks);

    hash_table_t_dtor(&hash_table);
    free((char *) text.ptr);
    return 0;
}
