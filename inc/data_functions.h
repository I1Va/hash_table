#ifndef DATA_FUNCTIONS_H
#define DATA_FUNCTIONS_H

#include <cstdlib>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#include "general_structs.h"
#include "hash_table_32b.h"

struct tests_data_t {
    char *words_32b;
    size_t words_cnt;
};


typedef int (*streq_func_t) (char *key_32b_1, char *key_32b_2);
extern "C" int streq_32b(char *key_32b_1, char *key_32b_2);

int get_file_sz(const char path[]);
tests_data_t load_text(const char path[]);
const char *next_alpha(const char *start, const char *end);
size_t get_word_len(const char *start, const char *end);
bool store_text_in_hash_table(tests_data_t tests_data, hash_table_32b_t *hash_table);
bool run_tests(const char path[], hash_table_32b_t *hash_table);
void print_string_t(const string_t string);
char *find_first_alpha_ptr(char *start, char *end);

#endif // DATA_FUNCTIONS_H