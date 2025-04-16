#include "general_structs.h"

const unsigned long long polinom_mult   = 127;
const unsigned long long polinom_offset = 41;

unsigned long long polinom_hash_func(string_t string) {
    unsigned long long sum = 0;

    for (size_t i = 0; i < string.len; i++) {
        sum = polinom_mult * string.ptr[i] + polinom_offset;
    }

    return sum;
}
