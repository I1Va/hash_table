#include <stdlib.h>
#include <cstring>
#include <ctime>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#include "general_structs.h"
#include "general.h"
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

    tests_data.words_32b = (char *) aligned_alloc(TESTS_DATA_T_ALIGNMENT, tests_data.words_cnt * WORD_32B_NMEMB);
    if (tests_data.words_32b == NULL) {
        debug("aligned_alloc failed");
        CLEAR_MEMORY(exit_mark)
    }
    memset(tests_data.words_32b, 0, tests_data.words_cnt * WORD_32B_NMEMB);

    for (size_t i = 0; i < tests_data.words_cnt; i++) {
        char *cur_word_ptr = tests_data.words_32b + i * WORD_32B_NMEMB;
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


bool store_text_in_hash_table(tests_data_t tests_data, hash_table_32b_t *hash_table) {
    assert(tests_data.words_32b);
    for (size_t i = 0; i < tests_data.words_cnt; i++) {
        char *key_32b = tests_data.words_32b + i * WORD_32B_NMEMB;

        if (!hash_table_32b_insert_key(hash_table, key_32b)) {
            debug("hash_table_set_key failed");
            return false;
        }

    }
    return true;
}
