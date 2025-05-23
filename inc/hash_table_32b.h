#ifndef HASH_TABLE_32B_H
#define HASH_TABLE_32B_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash_funcs.h"


struct list_node_t {
    const char *key;
    list_node_t* next;
};

struct hash_table_32b_t {
    size_t sz;
    hash_function_t hash_function;
    list_node_t **data;
};

bool hash_table_32b_t_ctor(hash_table_32b_t *hash_table, const size_t sz, hash_function_t hash_function);
void list_dtor(list_node_t *first_node);
void hash_table_t_dtor(hash_table_32b_t *hash_table);
list_node_t *list_node_t_ctor(const char key[]);
size_t get_list_len(list_node_t *first_node);
double hash_table_32b_get_load_factor(hash_table_32b_t *hash_table);
void hash_table_32b_dump_bucket_sizes(FILE *stream, hash_table_32b_t *hash_table);
bool hash_table_32b_insert_key(hash_table_32b_t *hash_table, const char key[]);
list_node_t *hash_table_32b_find_key(hash_table_32b_t *hash_table, const char key[]);
inline int streq_32b_inline(const char str1[], const char str2[]);


#endif // HASH_TABLE_32B_H
