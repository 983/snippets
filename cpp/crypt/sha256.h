#pragma once

#include <stdint.h>

#define SHA256_HASH_SIZE 32

struct sha256 {
    uint64_t n_bytes;
    uint32_t state[8];
    uint8_t block[64];
};

void sha256_init(struct sha256 *s);
void sha256_append(struct sha256 *s, const void *bytes_ptr, size_t n_bytes);
void sha256_digest(struct sha256 *s, uint8_t *hash, size_t hash_size);
