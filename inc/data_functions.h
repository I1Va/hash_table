#ifndef DATA_FUNCTIONS_H
#define DATA_FUNCTIONS_H

#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#include "hash_table_32b.h"

struct tests_data_t {
    char *words_32b;
    size_t words_cnt;
};


typedef int (*streq_func_t) (const char key_32b_1[], const char key_32b_2[]);
extern "C" int streq_32b(const char key_32b_1[], const char key_32b_2[]);

const size_t TESTS_DATA_T_ALIGNMENT = 32;

int get_file_sz(const char path[]);
tests_data_t load_text(const char path[]);
const char *next_alpha(const char *start, const char *end);
size_t get_word_len(const char *start, const char *end);
bool store_text_in_hash_table(tests_data_t tests_data, hash_table_32b_t *hash_table);
char *find_first_alpha_ptr(char *start, char *end);

#endif // DATA_FUNCTIONS_H