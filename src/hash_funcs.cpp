#include <stdint.h>
#include <immintrin.h>
#include <stdint.h>

#include "hash_funcs.h"


uint64_t polynom_hash_func(char *key, const size_t len) {
    uint64_t hash = 0;
    uint64_t hash_mult = 255;
    uint64_t hash_mod = (uint64_t) 1e9 + 7;

    for (size_t i = 0; i < len; i++) {
        hash = (hash * hash_mult + (uint64_t) key[i]) % hash_mod;
    }

    return hash;
}

uint64_t first_char_hash_func(char *key, const size_t len __attribute__((unused))) {
    return (uint64_t) key[0];
}

uint64_t fnv1a_hash(char *key, const size_t len) {
    const uint64_t FNV_prime = 0x01000193;
    uint64_t hash = 0x811C9DC5;
    const uint8_t *bytes = (const uint8_t *) key;

    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= FNV_prime;
    }
    return hash;
}

uint64_t crc32_hash_func(char *key, const size_t len) {
    const uint64_t CR32_POLY = 0x11EDC6F41;
    const unsigned char *buffer = (const unsigned char*) key;
    uint64_t crc = (uint64_t) -1;

    for (size_t i = 0; i < len; i++) {
        crc = crc ^ (uint64_t) (*buffer++ << 24);
        for( int bit = 0; bit < 8; bit++ )
        {
            if( crc & (1L << 31)) crc = (crc << 1) ^ CR32_POLY;
            else                  crc = (crc << 1);
        }
    }

    return ~crc;
}

uint64_t crc32_intrinsic_hash_func(char *key, const size_t len __attribute__((unused))) {
    uint64_t crc = 0;

    crc = _mm_crc32_u64(crc, *(uint64_t*) __builtin_assume_aligned(key + 0, 32));
    crc = _mm_crc32_u64(crc, *(uint64_t*) __builtin_assume_aligned(key + 1, 32));
    crc = _mm_crc32_u64(crc, *(uint64_t*) __builtin_assume_aligned(key + 2, 32));
    crc = _mm_crc32_u64(crc, *(uint64_t*) __builtin_assume_aligned(key + 3, 32));

    return crc;
}
