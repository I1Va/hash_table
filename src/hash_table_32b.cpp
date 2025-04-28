#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "hash_table_32b.h"
#include "hash_funcs.h"
#include "data_functions.h"


#pragma GCC diagnostic ignored "-Wunused-function"
static inline int streq_32b_inline(char *str1, char *str2) {
    int res = 0;

    __asm__(".intel_syntax noprefix\n"
            "vmovdqa ymm0, [%V1]\n"
            "vmovdqa ymm1, [%V2]\n"
            "vpcmpeqb ymm2, ymm0, ymm1\n"
            "vpmovmskb eax, ymm2\n"
            "not eax\n"

            : "=a"(res)
            : "r"(str1), "r"(str2)
            : "ymm0", "ymm1", "ymm2", "cc");

    return res;
}

bool hash_table_32b_t_ctor(hash_table_32b_t *hash_table, const size_t sz, hash_function_t hash_function) {
    assert(hash_table);
    assert(hash_function);

    hash_table->sz = sz;
    hash_table->hash_function = hash_function;

    hash_table->data = (list_node_t **) calloc(sz, sizeof(list_node_t *));
    if (hash_table->data == NULL) {
        debug("hash_table->data calloc failed");
        return false;
    }

    return true;
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

void hash_table_t_dtor(hash_table_32b_t *hash_table) {
    assert(hash_table);

    for (size_t i = 0; i < hash_table->sz; i++) {
        list_dtor(hash_table->data[i]);
    }

    free(hash_table->data);
}

list_node_t *list_node_t_ctor(char *key) {
    list_node_t *list_node = (list_node_t *) calloc(1, sizeof(list_node_t));
    if (!list_node) return NULL;

    list_node->key = key;
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

double hash_table_32b_get_load_factor(hash_table_32b_t *hash_table) {
    size_t hash_table_elems = 0;

    for (size_t i = 0; i < hash_table->sz; i++) {
        hash_table_elems += get_list_len(hash_table->data[i]);
    }

    return (double) hash_table_elems / (double) (hash_table->sz);
}

void hash_table_32b_dump_bucket_sizes(FILE *stream, hash_table_32b_t *hash_table) {
    for (size_t i = 0; i < hash_table->sz; i++) {
        fprintf(stream, "%lu ", get_list_len(hash_table->data[i]));
    }
    fprintf(stream, "\n");
}

bool hash_table_32b_insert_key(hash_table_32b_t *hash_table, char *key_32b) {
    assert(hash_table);

    unsigned long long table_idx = hash_table->hash_function(key_32b, 32) % hash_table->sz;

    list_node_t *cur_node = hash_table->data[table_idx];
    list_node_t *last_node = NULL;
    while (cur_node) {
        last_node = cur_node;

        #ifdef MY_STREQ
            #ifdef ASM_INSERTION
                if (streq_32b_inline(key_32b, cur_node->key) == 0) return true;
            #else
                if (streq_32b(key_32b, cur_node->key) == 0) return true;
            #endif // ASM_INSERTION
        #else
        if (strncmp(key_32b, cur_node->key, 32) == 0) return true;
        #endif // MY_STREQ

        cur_node = cur_node->next;
    }

    list_node_t *new_node = list_node_t_ctor(key_32b);

    if (!new_node) {
        debug("list_node_t_ctor failed");
        return false;
    }

    if (last_node) {
        last_node->next = new_node;
    } else {
        hash_table->data[table_idx] = new_node;
    }

    return true;
}

list_node_t *hash_table_32b_find_key(hash_table_32b_t *hash_table, char *key_32b) {
    assert(hash_table);

    unsigned long long table_idx = hash_table->hash_function(key_32b, 32) % hash_table->sz;

    list_node_t *cur_node = hash_table->data[table_idx];
    while (cur_node) {
        #ifdef MY_STREQ
            #ifdef ASM_INSERTION
                if (streq_32b_inline(key_32b, cur_node->key) == 0) return cur_node;
            #else
                if (streq_32b(key_32b, cur_node->key) == 0) return cur_node;
            #endif // ASM_INSERTION
        #else
            if (strncmp(key_32b, cur_node->key, 32) == 0) return cur_node;
        #endif // MY_STREQ

        cur_node = cur_node->next;
    }

    return NULL;
}
