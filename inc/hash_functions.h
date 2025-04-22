#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include "general_structs.h"
#include <stdint.h>

typedef uint32_t (*hash_function_t) (string_t string);

uint32_t polinom_hash_func(string_t string);

#endif // HASH_FUNCTIONS_H