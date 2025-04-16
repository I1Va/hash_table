#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string.h>

#include "general_structs.h"

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
void list_dtor(list_node_t *first_node);
void hash_table_t_dtor(hash_table_t *hash_table);
int hash_table_set_key(hash_table_t *hash_table, string_t key, void *data);
bool hash_table_read_key(hash_table_t *hash_table, string_t key, void **data);

#endif // HASH_TABLE_H