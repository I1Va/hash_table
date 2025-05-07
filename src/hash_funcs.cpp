#include <stdint.h>
#include <immintrin.h>
#include <stdint.h>
#include <string.h>
#include "hash_funcs.h"


uint64_t polynom_hash_func(const char key[]) {
    uint64_t hash = 0;
    uint64_t hash_mult = 255;
    uint64_t hash_mod = (uint64_t) 1e9 + 7;

    for (size_t i = 0; key[i]; i++) {
        hash = (hash * hash_mult + (uint64_t) key[i]) % hash_mod;
    }

    return hash;
}

uint64_t first_char_hash_func(const char key[]) {
    return (uint64_t) key[0];
}

uint64_t fnv1a_hash(const char key[]) {
    const uint64_t FNV_prime = 0x01000193;
    uint64_t hash = 0x811C9DC5;
    const uint8_t *bytes = (const uint8_t *) key;

    for (size_t i = 0; bytes[i]; i++) {
        hash ^= bytes[i];
        hash *= FNV_prime;
    }
    return hash;
}

uint64_t crc32_hash_func(const char key[]) {
    const uint64_t CR32_POLY = 0x11EDC6F41;
    const unsigned char *buffer = (const unsigned char*) key;
    uint64_t crc = (uint64_t) -1;

    for (size_t i = 0; buffer[i]; i++) {
        crc = crc ^ (uint64_t) buffer[i];
        for(size_t j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (CR32_POLY & (-(crc & 1)));
        }
    }

    return ~crc;
}

uint64_t crc32_intrinsic_hash_func(const char key[]) {
    uint64_t res = 0;

    uint64_t key_vec_u64_0 = 0;
    uint64_t key_vec_u64_1 = 0;
    uint64_t key_vec_u64_2 = 0;
    uint64_t key_vec_u64_3 = 0;

    memcpy(&key_vec_u64_0, key + 0, 8);
    memcpy(&key_vec_u64_1, key + 1, 8);
    memcpy(&key_vec_u64_2, key + 2, 8);
    memcpy(&key_vec_u64_3, key + 3, 8);

    res = _mm_crc32_u64(res, key_vec_u64_0);
    res = _mm_crc32_u64(res, key_vec_u64_1);
    res = _mm_crc32_u64(res, key_vec_u64_2);
    res = _mm_crc32_u64(res, key_vec_u64_3);

    return res;
}
