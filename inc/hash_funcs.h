#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stdint.h>
#include <string.h>


typedef uint64_t (*hash_function_t) (const char key_32b[], const size_t len);

uint64_t polynom_hash_func(const char key[], const size_t len);
uint64_t crc32_hash_func(const char key[], const size_t len);
uint64_t first_char_hash_func(const char key[], const size_t __attribute__((unused)) len);
uint64_t fnv1a_hash(const char key[], const size_t len);
uint64_t crc32_intrinsic_hash_func(const char key[], const size_t __attribute__((unused)) len);

#endif // HASH_FUNCTIONS_H
