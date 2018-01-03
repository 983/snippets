#include "chacha20.h"

#include <assert.h>

static uint32_t rotl32(uint32_t x, int n){
    return (x << n) | (x >> (32 - n));
}

static uint32_t bytes_to_u32(const uint8_t *a){
    return
        (uint32_t)(a[0] << 0*8) |
        (uint32_t)(a[1] << 1*8) |
        (uint32_t)(a[2] << 2*8) |
        (uint32_t)(a[3] << 3*8);
}

static void u32_to_bytes(uint32_t src, uint8_t *dst){
    dst[0] = (src >> 0*8) & 0xff;
    dst[1] = (src >> 1*8) & 0xff;
    dst[2] = (src >> 2*8) & 0xff;
    dst[3] = (src >> 3*8) & 0xff;
}

static void chacha20_fill_keystream(struct chacha20 *c){
    uint32_t x[16];
    
    for (int i = 0; i < 16; i++){
        x[i] = c->state[i];
    }
    
#define CHACHA20_QUARTERROUND(a, b, c, d) \
    x[a] += x[b]; x[d] = rotl32(x[d] ^ x[a], 16); \
    x[c] += x[d]; x[b] = rotl32(x[b] ^ x[c], 12); \
    x[a] += x[b]; x[d] = rotl32(x[d] ^ x[a], 8); \
    x[c] += x[d]; x[b] = rotl32(x[b] ^ x[c], 7);

    for (int i = 0; i < 10; i++){
        CHACHA20_QUARTERROUND(0, 4, 8, 12)
        CHACHA20_QUARTERROUND(1, 5, 9, 13)
        CHACHA20_QUARTERROUND(2, 6, 10, 14)
        CHACHA20_QUARTERROUND(3, 7, 11, 15)
        CHACHA20_QUARTERROUND(0, 5, 10, 15)
        CHACHA20_QUARTERROUND(1, 6, 11, 12)
        CHACHA20_QUARTERROUND(2, 7, 8, 13)
        CHACHA20_QUARTERROUND(3, 4, 9, 14)
    }

    for (int i = 0; i < 16; i++){
        x[i] += c->state[i];
    }

    // increment counter
    uint32_t *counter = c->state + 12;
    counter[0]++;
    if (0 == counter[0]){
        counter[1]++;
    }
    
    for (int i = 0; i < 8; i++){
        u32_to_bytes(x[i], c->keystream + i*4);
    }
}

void chacha20_init(
    struct chacha20 *c,
    const uint8_t *key,
    size_t key_size,
    const uint8_t *nonce,
    size_t nonce_size
){
    assert(nonce_size == 8);
    assert(key_size == 32);
    
    const uint8_t *magic_constant = (uint8_t*)"expand 32-byte k";
    uint32_t *state = c->state;
    
    state[ 0] = bytes_to_u32(magic_constant + 0*4);
    state[ 1] = bytes_to_u32(magic_constant + 1*4);
    state[ 2] = bytes_to_u32(magic_constant + 2*4);
    state[ 3] = bytes_to_u32(magic_constant + 3*4);
    state[ 4] = bytes_to_u32(key + 0*4);
    state[ 5] = bytes_to_u32(key + 1*4);
    state[ 6] = bytes_to_u32(key + 2*4);
    state[ 7] = bytes_to_u32(key + 3*4);
    state[ 8] = bytes_to_u32(key + 4*4);
    state[ 9] = bytes_to_u32(key + 5*4);
    state[10] = bytes_to_u32(key + 6*4);
    state[11] = bytes_to_u32(key + 7*4);
    state[12] = 0;
    state[13] = 0;
    state[14] = bytes_to_u32(nonce + 0*4);
    state[15] = bytes_to_u32(nonce + 1*4);
    
    c->position = 0;
    
    chacha20_fill_keystream(c);
}

void chacha20_crypt(struct chacha20 *c, uint8_t *bytes, size_t n_bytes){
    for (size_t i = 0; i < n_bytes; i++){
        bytes[i] ^= c->keystream[c->position++];
        if (c->position >= 64){
            c->position = 0;
            chacha20_fill_keystream(c);
        }
    }
}
