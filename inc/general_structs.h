#ifndef GENERAL_STRUCTS_H
#define GENERAL_STRUCTS_H

#include <string.h>

struct string_t {
    const char *ptr;
    size_t len;
};

typedef unsigned long long (*hash_function_t) (string_t string);

#endif // GENERAL_STRUCTS_H