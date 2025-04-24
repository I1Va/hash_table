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


inline int asm_streq_32b(char *key_32b_1, char *key_32b_2) {
    return streq_32b(key_32b_1, key_32b_2);
}

inline int c_streq_32b(char *key_32b_1, char *key_32b_2) {
    return strncmp(key_32b_1, key_32b_2, 32);
}

#ifdef MY_STREQ
const streq_func_t str_eq_func = asm_streq_32b;
#else
const streq_func_t str_eq_func = c_streq_32b;
#endif // MY_STREQ


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
        int str_eq_outp = 0;

        #ifdef ASM_INSERTION
        __asm__(".intel_syntax noprefix\n"
            "vmovdqa ymm0, [%V1]\n"
            "vmovdqa ymm1, [%V2]\n"
            "vpcmpeqb ymm2, ymm0, ymm1\n"
            "vpmovmskb eax, ymm2\n"
            "not eax\n"
            : "=a"(str_eq_outp)
            : "r"(key_32b), "r"(cur_node->key)
            : "ymm0", "ymm1", "ymm2");
        #else
        str_eq_outp = str_eq_func(key_32b, cur_node->key);
        #endif // ASM_INSERTION

        if (str_eq_outp == 0) return true;
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
        int str_eq_outp = 0;

        #ifdef ASM_INSERTION
        __asm__(".intel_syntax noprefix\n"
            "vmovdqa ymm0, [%V1]\n"
            "vmovdqa ymm1, [%V2]\n"
            "vpcmpeqb ymm2, ymm0, ymm1\n"
            "vpmovmskb eax, ymm2\n"
            "not eax\n"
            : "=a"(str_eq_outp)
            : "r"(key_32b), "r"(cur_node->key)
            : "ymm0", "ymm1", "ymm2");
        #else
        str_eq_outp = str_eq_func(key_32b, cur_node->key);
        #endif // ASM_INSERTION

        if (str_eq_outp == 0) return cur_node;
        cur_node = cur_node->next;
    }

    return NULL;
}
