#include <assert.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>

#include "general.h"

struct string_t {
    char *ptr;
    size_t len;
};

typedef unsigned long long (*hash_function_t) (string_t string);

struct list_node_t {
    string_t string;
    list_node_t* next;
};

struct hash_table_t {
    size_t sz;
    hash_function_t hash_function;
    list_node_t **data;
};

int hash_table_t_ctor(hash_table_t *hash_table, const size_t sz, hash_function_t hash_function) {
    assert(hash_table);
    assert(hash_function);

    hash_table->sz = sz;
    hash_table->hash_function = hash_function;

    hash_table->data = (list_node_t **) calloc(sz, sizeof(list_node_t *));
    if (hash_table->data == NULL) {
        debug("hash_table->data calloc failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void list_dtor(list_node_t *first_node) {
    if (!first_node) return;

    list_node_t *cur_node = first_node->next;
    while (cur_node) {
        free(cur_node);
        list_node_t *next_node = cur_node->next;
        cur_node = next_node;
    }
}

void hash_table_t_dtor(hash_table_t *hash_table) {
    assert(hash_table);

    for (size_t i = 0; i < hash_table->sz; i++) {
        list_dtor(hash_table->data[i]);
    }

    free(hash_table->data);
}
