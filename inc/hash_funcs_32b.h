#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdint.h>

typedef uint32_t (*hash_function_32b_t) (char *key_32b);

uint32_t polinom_hash_func(char *key_32b);
uint32_t crc32_hash_func(char *key_32b);

#endif // HASH_FUNCTIONS_H