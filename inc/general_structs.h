#ifndef GENERAL_STRUCTS_H
#define GENERAL_STRUCTS_H

#include <string.h>


struct string_t {
    char *ptr;
    size_t len;
};

const size_t word_32b_nmemb = 32;


#endif // GENERAL_STRUCTS_H