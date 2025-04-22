#include <cstdlib>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#include "general_structs.h"
#include "general.h"
#include "hash_table.h"
#include "data_functions.h"

int get_file_sz(const char path[]) {
    struct stat buf = {};
    if (stat(path, &buf) != 0) {
        debug("file stat '%s' failed", path);
        return -1;
    }

    return (int) buf.st_size;
}

size_t fill_to_intergral_multiple(size_t base, size_t divider) {
    return base + (divider - base %divider);
}

string_t load_text(const char path[], const size_t alignment) {
    assert(path);

    string_t aligned_text = {};
    FILE *inp_file = NULL;

    int inp_file_sz = get_file_sz(path);
    if (inp_file_sz < 0) {
        debug("get_file_sz '%s' failed", path);
        CLEAR_MEMORY(exit_mark)
    }

    inp_file = fopen(path, "r");
    if (inp_file == NULL) {
        debug("fopen '%s' failed", path);
        CLEAR_MEMORY(exit_mark)
    }

    if (alignment == 0) {
        aligned_text.ptr = (char *) calloc((size_t) inp_file_sz, sizeof(char));
        if (aligned_text.ptr == NULL) {
            debug("calloc failed");
            CLEAR_MEMORY(exit_mark)
        }
    } else {
        aligned_text.ptr = (char *) aligned_alloc(alignment, fill_to_intergral_multiple((size_t) inp_file_sz, alignment) * sizeof(char));
        if (aligned_text.ptr == NULL) {
            debug("aligned_alloc failed");
            CLEAR_MEMORY(exit_mark)
        }
    }

    aligned_text.len = (size_t) inp_file_sz;
    if (fread(((unsigned char *) aligned_text.ptr), sizeof(char), aligned_text.len, inp_file) < aligned_text.len) {
        debug("fread '%s' failed", path);
        CLEAR_MEMORY(exit_mark)
    }

    fclose(inp_file);

    return aligned_text;

    exit_mark:

    if (inp_file) fclose(inp_file);
    if (aligned_text.ptr) free((void *) aligned_text.ptr);

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

int store_text_in_hash_table(string_t text, hash_table_t *hash_table) {
    assert(text.ptr);
    assert(hash_table);

    char *cur_ptr = text.ptr;
    char *end_ptr = text.ptr + text.len;
    string_t string = {};

    while (cur_ptr != end_ptr) {
        cur_ptr = find_first_alpha_ptr(cur_ptr, end_ptr);
        if (cur_ptr == NULL) break;

        string.ptr = cur_ptr;
        string.len = get_word_len(cur_ptr, end_ptr);

        cur_ptr += string.len;

        if (hash_table_set_key(hash_table, string, NULL) != 0) {
            debug("hash_table_set_key failed");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int run_tests(const char path[], hash_table_t *hash_table) {
    assert(path);
    assert(hash_table);

    FILE    *tests_file = fopen(path, "r");
    size_t  tests_cnt = 0;

    char        bufer[BUFSIZ] = {};
    string_t    string = {};

    if (tests_file == NULL) {
        debug("failed to open '%s'", path);
        CLEAR_MEMORY(exit_mark)
    }

    string.ptr = bufer;

    if (fscanf(tests_file, "%lu", &tests_cnt) != 1) {
        debug("tests_cnt fscanf error");
        CLEAR_MEMORY(exit_mark)
    }

    for (size_t i = 0; i < tests_cnt; i++) {
        int fscanf_res = fscanf(tests_file, "%s", (char *) string.ptr);
        if (fscanf_res != 1) {
            debug("string.ptr fscanf failed : fscanf_res='%d', errno='%s'", fscanf_res, strerror(errno));
            CLEAR_MEMORY(exit_mark)
        }

        string.len = strnlen(string.ptr, BUFSIZ);

        list_node_t *read_key_res = hash_table_read_key(hash_table, string);

        if (read_key_res == NULL) return EXIT_FAILURE;
    }

    fclose(tests_file);
    return EXIT_SUCCESS;

    exit_mark:
    if (tests_file) fclose(tests_file);

    return EXIT_FAILURE;
}