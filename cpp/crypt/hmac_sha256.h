#pragma once

#include "sha256.h"

struct hmac_sha256 {
    struct sha256 inner[1];
    struct sha256 outer[1];
};

void hmac_sha256_init(
    struct hmac_sha256 *h,
    const uint8_t *key,
    size_t key_size
);

void hmac_sha256_append(
    struct hmac_sha256 *h,
    const uint8_t *message,
    size_t message_size
);

void hmac_sha256_digest(
    struct hmac_sha256 *h,
    uint8_t *hmac,
    size_t hmac_size
);
