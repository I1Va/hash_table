#include <string.h>
#include <stdio.h>

#include "general_structs.h"
#include "hash_table.h"
#include "hash_functions.h"

const size_t hash_table_sz = 128;

int main() {
    hash_table_t hash_table = {};
    hash_table_t_ctor(&hash_table, hash_table_sz, polinom_hash_func);

    string_t str1 = {};
    str1.ptr = "sdsda";
    str1.len = strlen(str1.ptr);
    int a = 11231;

    hash_table_set_key(&hash_table, str1, &a);

    void *read = NULL;
    printf("read : %d\n", hash_table_read_key(&hash_table, str1, &read));

    printf("data : %d\n", *((int *) read));

    hash_table_t_dtor(&hash_table);
    return 0;
}
