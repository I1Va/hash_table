#include <cstdlib>
#include <string.h>
#include <stdio.h>

#include "general.h"
#include "general_structs.h"
#include "hash_table.h"
#include "hash_functions.h"
#include "data_functions.h"

const size_t hash_table_sz = 128;
const char TEXT_PATH[] = "data/text.txt";

const size_t ALIGNMENT = 256;

int main() {
    hash_table_t hash_table = {};
    hash_table_t_ctor(&hash_table, hash_table_sz, polinom_hash_func);

    string_t text = load_text(TEXT_PATH, ALIGNMENT);
    if (text.ptr == NULL) {
        debug("load_text '%s' failed\n", TEXT_PATH);
        return EXIT_FAILURE;
    }

    store_text_in_hash_table(text, &hash_table);

    while (true) {
        char bufer[BUFSIZ] = {};
        scanf("%s", bufer);
        string_t string = {};
        string.ptr = bufer;
        string.len = strnlen(bufer, BUFSIZ);
        void *data = NULL;

        if (hash_table_read_key(&hash_table, string, &data)) {
            printf("%s found!\n", bufer);
        } else {
            printf("%s not found!\n", bufer);
        }
    }

    hash_table_t_dtor(&hash_table);
    return 0;
}
