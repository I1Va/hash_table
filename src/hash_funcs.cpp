#include <immintrin.h>
#include <stdint.h>

#include "general_structs.h"
#include "hash_funcs.h"


uint32_t polynom_hash_func(char *key, const size_t len) {
    uint32_t hash = 0;
    uint32_t hash_mult = 17;
    uint32_t hash_add = 131;
    for (size_t i = 0; i < len; i++) {
        hash += (uint32_t) key[i] * hash_mult + hash_add;
    }

    return hash;
}

uint32_t crc32_hash_func(char *key, const size_t len) {
    const uint32_t CR32_POLY = 0x04C11DB7;
    const unsigned char *buffer = (const unsigned char*) key;
    uint32_t crc = (uint32_t) -1;

    for (size_t i = 0; i < len; i++) {
        crc = crc ^ (uint32_t) (*buffer++ << 24);
        for( int bit = 0; bit < 8; bit++ )
        {
            if( crc & (1L << 31)) crc = (crc << 1) ^ CR32_POLY;
            else                  crc = (crc << 1);
        }
    }

    return ~crc;
}

uint32_t first_char_hash_func(char *key, const size_t len __attribute__((unused))) {
    return (uint32_t) key[0];
}

uint32_t fnv1a_hash(char *key, const size_t len) {
    const uint32_t FNV_prime = 0x01000193;
    uint32_t hash = 0x811C9DC5;
    const uint8_t *bytes = (const uint8_t *) key;

    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= FNV_prime;
    }
    return hash;
}

