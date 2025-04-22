#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

#include "general_structs.h"
#include "general.h"
#include "hash_funcs_32b.h"
#include "hash_table_32b.h"
#include "data_functions.h"

int get_file_sz(const char path[]) {
    struct stat buf = {};
    if (stat(path, &buf) != 0) {
        debug("file stat '%s' failed", path);
        return -1;
    }

    return (int) buf.st_size;
}

tests_data_t load_text(const char path[]) {
    assert(path);

    tests_data_t tests_data = {};
    FILE *inp_file = NULL;

    inp_file = fopen(path, "r");
    if (inp_file == NULL) {
        debug("fopen '%s' failed", path);
        CLEAR_MEMORY(exit_mark)
    }

    if (fscanf(inp_file, "%lu", &tests_data.words_cnt) != 1) {
        debug("fscanf tests_data.words_cnt failed");
        CLEAR_MEMORY(exit_mark)
    }

    tests_data.words_32b = (char *) aligned_alloc(tests_data.word_nmemb, tests_data.words_cnt * tests_data.word_nmemb);
    if (tests_data.words_32b == NULL) {
        debug("aligned_alloc failed");
        CLEAR_MEMORY(exit_mark)
    }
    memset(tests_data.words_32b, 0, tests_data.words_cnt * tests_data.word_nmemb);

    for (size_t i = 0; i < tests_data.words_cnt; i++) {
        char *cur_word_ptr = tests_data.words_32b + i * tests_data.word_nmemb;
        if (fscanf(inp_file, "%31s", cur_word_ptr) != 1) {
            debug("fscanf cur_word_ptr failed");
            CLEAR_MEMORY(exit_mark)
        }
    }

    fclose(inp_file);
    return tests_data;

    exit_mark:

    if (inp_file) fclose(inp_file);
    if (tests_data.words_32b) free(tests_data.words_32b);

    return {};
}

char *find_first_alpha_ptr(char *start, char *end) {
    assert(start);
    assert(end);

    while (start != end) {
        if (isalpha(*start)) {
            return start;
        }
        start++;
    }
    return NULL;
}

size_t get_word_len(const char *start, const char *end) {
    assert(start);
    assert(end);

    const char *cur_ptr = start;
    while (cur_ptr != end && isalpha(*cur_ptr)) {
        cur_ptr++;
    }

    return (size_t) (cur_ptr - start);
}

void print_string_t(const string_t string) {
    if (string.ptr == NULL) {
        printf("NULL, len=%lu", string.len);
    } else {
         printf("'");

        for (size_t i = 0; i < string.len; i++) {
            putc(string.ptr[i], stdout);
        }
        printf("', len=%lu", string.len);
    }
}

void print_hash_of_key(hash_table_32b_t *hash_table, char *key32_b, char *extra_info) {
    printf("%s | key : '%s', hash : '%d'\n", extra_info, key32_b, hash_table->hash_function(key32_b));
}


void print_word_32b(char *word_32b) {
    printf("'%s' = [", word_32b);
    for (int i = 0; i < 32; i++) {
        printf("%.2d|", word_32b[i]);
    }
    printf("]\n");
}


bool store_text_in_hash_table(tests_data_t tests_data, hash_table_32b_t *hash_table) {
    assert(tests_data.words_32b);
    for (size_t i = 0; i < tests_data.words_cnt; i++) {
        char *key_32b = tests_data.words_32b + i * tests_data.word_nmemb;

        if (!hash_table_32b_insert_key(hash_table, key_32b)) {
            debug("hash_table_set_key failed");
            return false;
        }

    }
    return true;
}

bool run_tests(const char path[], hash_table_32b_t *hash_table) {
    assert(path);
    assert(hash_table);

    FILE    *tests_file = fopen(path, "r");
    size_t  tests_cnt = 0;

    char bufer[32] = {};
    char *key_32b  = bufer;

    if (tests_file == NULL) {
        debug("failed to open '%s'", path);
        CLEAR_MEMORY(exit_mark)
    }

    if (fscanf(tests_file, "%lu", &tests_cnt) != 1) {
        debug("tests_cnt fscanf error");
        CLEAR_MEMORY(exit_mark)
    }

    for (size_t i = 0; i < tests_cnt; i++) {
        memset(key_32b, 0, 32);
        int fscanf_res = fscanf(tests_file, "%s", key_32b);
        if (fscanf_res != 1) {
            debug("string.ptr fscanf failed : fscanf_res='%d', errno='%s'", fscanf_res, strerror(errno));
            CLEAR_MEMORY(exit_mark)
        }

        list_node_t *read_key_res = hash_table_32b_find_key(hash_table, key_32b);

        if (read_key_res == NULL) {
            debug("word '%s' hasn't found\n", key_32b);
            CLEAR_MEMORY(exit_mark)
        }
    }

    fclose(tests_file);
    return true;

    exit_mark:
    if (tests_file) fclose(tests_file);

    return false;
}
