#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string.h>
#include <stdio.h>

#include "general_structs.h"
#include "hash_functions.h"


struct list_node_t {
    string_t key;
    void *data;
    list_node_t* next;
};

struct hash_table_t {
    size_t sz;
    hash_function_t hash_function;
    list_node_t **data;
};

int hash_table_t_ctor(hash_table_t *hash_table, const size_t sz, hash_function_t hash_function);
list_node_t *list_node_t_ctor(string_t key, void *data);
void list_dtor(list_node_t *first_node);
void hash_table_t_dtor(hash_table_t *hash_table);
int hash_table_set_key(hash_table_t *hash_table, string_t key, void *data);
list_node_t *hash_table_read_key(hash_table_t *hash_table, string_t key);
double get_load_factor(hash_table_t *hash_table);
void dump_bucket_sizes(FILE *stream, hash_table_t *hash_table);
size_t get_list_len(list_node_t *first_node);
bool string_eq(string_t str1, string_t str2);


#endif // HASH_TABLE_H