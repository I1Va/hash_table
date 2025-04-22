#include <assert.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "general_structs.h"
#include "hash_table.h"

// FIXME: вынести в отдельный cpp либо inline

int hash_table_t_ctor(hash_table_t *hash_table, const size_t sz, hash_function_t hash_function) {
    assert(hash_table);
    assert(hash_function);

    hash_table->sz = sz;
    hash_table->hash_function = hash_function;

    hash_table->data = (list_node_t **) calloc(sz, sizeof(list_node_t *));
    if (hash_table->data == NULL) {
        debug("hash_table->data calloc failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void list_dtor(list_node_t *first_node) {
    if (!first_node) return;

    list_node_t *cur_node = first_node;
    while (cur_node) {
        list_node_t *next_node = cur_node->next;
        free(cur_node);
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

// FIXME: использовать ограничение на длину строк (32)
// FIXME: выравнивать строки
bool string_eq(string_t str1, string_t str2) {
    assert(str1.ptr);
    assert(str2.ptr);

    if (str1.len != str2.len) return false;

    return (strncmp(str1.ptr, str2.ptr, str1.len) == 0);
}

list_node_t *list_node_t_ctor(string_t key, void *data) {
    list_node_t *list_node = (list_node_t *) calloc(1, sizeof(list_node_t));
    if (!list_node) return NULL;

    list_node->key = key;
    list_node->data = data;
    list_node->next = NULL;

    return list_node;
}

size_t get_list_len(list_node_t *first_node) {
    size_t len = 0;
    while (first_node) {
        len++;
        first_node = first_node->next;
    }

    return len;
}

double get_load_factor(hash_table_t *hash_table) {
    size_t hash_table_elems = 0;

    for (size_t i = 0; i < hash_table->sz; i++) {
        hash_table_elems += get_list_len(hash_table->data[i]);
    }

    return (double) hash_table_elems / (double) (hash_table->sz);
}

void dump_bucket_sizes(FILE *stream, hash_table_t *hash_table) {
    for (size_t i = 0; i < hash_table->sz; i++) {
        fprintf(stream, "%lu ", get_list_len(hash_table->data[i]));
    }
    fprintf(stream, "\n");
}


int hash_table_set_key(hash_table_t *hash_table, string_t key, void *data) {
    assert(hash_table);

    unsigned long long table_idx = hash_table->hash_function(key) % hash_table->sz;

    list_node_t *cur_node = hash_table->data[table_idx];
    list_node_t *last_node = NULL;
    while (cur_node) {
        last_node = cur_node;
        if (string_eq(cur_node->key, key)) {
            cur_node->data = data;
            return EXIT_SUCCESS;
        }
        cur_node = cur_node->next;
    }

    list_node_t *new_node = list_node_t_ctor(key, data);

    if (!new_node) {
        debug("list_node_t_ctor failed");
        return EXIT_FAILURE;
    }

    if (last_node) {
        last_node->next = new_node;
    } else {
        hash_table->data[table_idx] = new_node;
    }

    return EXIT_SUCCESS;
}

list_node_t *hash_table_read_key(hash_table_t *hash_table, string_t key) {
    assert(hash_table);

    unsigned long long table_idx = hash_table->hash_function(key) % hash_table->sz;

    list_node_t *cur_node = hash_table->data[table_idx];
    while (cur_node) {
        if (string_eq(cur_node->key, key)) return cur_node;
        cur_node = cur_node->next;
    }

    return NULL;
}