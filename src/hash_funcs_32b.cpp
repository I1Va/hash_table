#include <immintrin.h>
#include <stdint.h>

#include "general_structs.h"
#include "hash_funcs_32b.h"


static uint32_t crc32_tab[256];


static void init_crc32_tab() {
    uint32_t poly = 0xEDB88320U;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (poly & -(int)(crc & 1));
        }
        crc32_tab[i] = crc;
    }
}

uint32_t crc32_hash_func(char *key_32b) {
    static int initialized = 0;
    if (!initialized) {
        init_crc32_tab();
        initialized = 1;
    }

    uint32_t crc = ~0U;
    for (size_t i = 0; i < 32; ++i) {
        crc = (crc >> 8) ^ crc32_tab[(crc ^ (unsigned char)key_32b[i]) & 0xFF];
    }
    return ~crc;
}

uint32_t polinom_hash_func(char *key_32b) {
    uint32_t hash = 0;
    uint32_t hash_mult = 17;
    uint32_t hash_add = 131;
    for (size_t i = 0; i < 32; i++) {
        hash += (uint32_t) key_32b[i] * hash_mult + hash_add;
    }

    return hash;
}
