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
#include "hash_table.h"

int get_file_sz(const char path[]);
string_t load_text(const char path[], const size_t alignment=0);
const char *next_alpha(const char *start, const char *end);
size_t get_word_len(const char *start, const char *end);
int store_text_in_hash_table(string_t text, hash_table_t *hash_table);
int run_tests(const char path[], hash_table_t *hash_table);
size_t fill_to_intergral_multiple(size_t base, size_t divider);
void print_string_t(const string_t string);
char *find_first_alpha_ptr(char *start, char *end);

#endif // DATA_FUNCTIONS_H