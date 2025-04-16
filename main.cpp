#include <string.h>

#include "hash_table.h"
#include "hash_functions.h"

const size_t hash_table_sz = 128;

int main() {
    hash_table_t hash_table = {};
    hash_table_t_ctor(&hash_table, hash_table_sz, polinom_hash_func);

    hash_table_t_dtor(&hash_table);
    return 0;
}
