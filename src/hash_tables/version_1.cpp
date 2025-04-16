#include <assert.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include "general.h"

typedef struct {
    char *ptr;
    size_t len;
} string_t;

typedef unsigned long long (*hash_function_t) (string_t string);

typedef struct {
    string_t string;
    string_t* next;
} list_node_t;

typedef struct {
    size_t sz;
    list_node_t *data;
    hash_function_t hash_function;
} hash_table_t;

int hash_table_t_ctor(hash_table_t *hash_table, const size_t sz, hash_function_t hash_function) {
    assert(hash_table);
    assert(hash_function);

    hash_table->sz = sz;
    hash_table->hash_function = hash_function;

    hash_table->data = (list_node_t *) calloc(sz, sizeof(list_node_t));
    if (hash_table->data == NULL) {
        debug("hash_table->data calloc failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
