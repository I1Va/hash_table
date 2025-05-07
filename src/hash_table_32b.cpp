#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <sys/types.h>

#include "general.h"
#include "hash_table_32b.h"
#include "hash_funcs.h"
#include "data_functions.h"


__attribute__((unused))
inline int streq_32b_inline(const char *str1, const char *str2) {
    int res = 0;
    __asm__(".intel_syntax noprefix\n"
            "vmovdqa ymm0, [%1]\n"
            "xor rax, rax\n"
            "vptest ymm0, [%2]\n"
            "seta al\n"

            : "=a"(res)
            : "r"(str1),
              "D"(str2)
            : "ymm0", "cc");

    return res;
}

__attribute__((unused))
inline uint64_t inline_crc32_intrinsic_hash_func(char *key) {
    uint64_t res = 0;

    uint64_t key_vec_u64_0 = 0;
    uint64_t key_vec_u64_1 = 0;
    uint64_t key_vec_u64_2 = 0;
    uint64_t key_vec_u64_3 = 0;

    memcpy(&key_vec_u64_0, key + 0, 8);
    memcpy(&key_vec_u64_1, key + 1, 8);
    memcpy(&key_vec_u64_2, key + 2, 8);
    memcpy(&key_vec_u64_3, key + 3, 8);

    res = _mm_crc32_u64(res, *(uint64_t*) __builtin_assume_aligned(key + 0, 32));
    res = _mm_crc32_u64(res, *(uint64_t*) __builtin_assume_aligned(key + 1, 32));
    res = _mm_crc32_u64(res, *(uint64_t*) __builtin_assume_aligned(key + 2, 32));
    res = _mm_crc32_u64(res, *(uint64_t*) __builtin_assume_aligned(key + 3, 32));

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

    #ifdef INLINE_INTINSIC_CR32
        uint64_t table_idx = inline_crc32_intrinsic_hash_func(key_32b) % hash_table->sz;
        #define HASH_OPTIMIZATION_SELECTED
    #endif // INLINE_INTINSIC_CR32

    #ifndef HASH_OPTIMIZATION_SELECTED
        uint64_t table_idx = hash_table->hash_function(key_32b, 32) % hash_table->sz;
    #else
        #undef HASH_OPTIMIZATION_SELECTED
    #endif // NOT HASH_OPTIMIZATION_SELECTED

    list_node_t *cur_node = hash_table->data[table_idx];
    list_node_t *last_node = NULL;
    while (cur_node) {
        last_node = cur_node;

        #ifdef MY_STREQ
            #define STREQ_SELECTED
            if (streq_32b(key_32b, cur_node->key) == 0) return true;
        #endif // MY_STREQ

        #ifdef INLINE_ASM_STREQ
            #define STREQ_SELECTED
            if (streq_32b_inline(key_32b, cur_node->key) == 0) return true;
        #endif // ASM_INSERTION

        #ifndef STREQ_SELECTED
            if (strncmp(key_32b, cur_node->key, 32) == 0) return true;
        #else
            #undef STREQ_SELECTED
        #endif // NOT STREQ_SELECTED

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

    #ifdef ASM_INSERTION_CR32
        uint64_t table_idx = inline_crc32_asm_hash_func(key_32b) % hash_table->sz;
    #else
        uint64_t table_idx = hash_table->hash_function(key_32b, 32) % hash_table->sz;
    #endif // ASM_INSERTION_CR32


    list_node_t *cur_node = hash_table->data[table_idx];
    while (cur_node) {
        #ifdef MY_STREQ
            #define STREQ_SELECTED
            if (streq_32b(key_32b, cur_node->key) == 0) return cur_node;
        #endif // MY_STREQ

        #ifdef INLINE_ASM_STREQ
            #define STREQ_SELECTED
            if (streq_32b_inline(key_32b, cur_node->key) == 0) return cur_node;
        #endif // INLINE_ASM_STREQ

        #ifndef STREQ_SELECTED
            if (strncmp(key_32b, cur_node->key, 32) == 0) return cur_node;
        #else
            #undef STREQ_SELECTED
        #endif // NOT STREQ_SELECTED

        cur_node = cur_node->next;
    }

    return NULL;
}
