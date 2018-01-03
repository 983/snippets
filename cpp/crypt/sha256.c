#include "sha256.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

inline uint32_t rotr32(uint32_t x, int n){
    return (x >> n) | (x << (32 - n));
}

void sha256_init(struct sha256 *s){
    s->state[0] = 0x6a09e667;
    s->state[1] = 0xbb67ae85;
    s->state[2] = 0x3c6ef372;
    s->state[3] = 0xa54ff53a;
    s->state[4] = 0x510e527f;
    s->state[5] = 0x9b05688c;
    s->state[6] = 0x1f83d9ab;
    s->state[7] = 0x5be0cd19;
    
    s->n_bytes = 0;
}

void sha256_process_block(struct sha256 *s){
    uint32_t w[16];
    memcpy(w, s->block, 64);
    
    for (int i = 0; i < 16; i++){
        w[i] = __builtin_bswap32(w[i]);
    }
    
    uint32_t a = s->state[0];
    uint32_t b = s->state[1];
    uint32_t c = s->state[2];
    uint32_t d = s->state[3];
    uint32_t e = s->state[4];
    uint32_t f = s->state[5];
    uint32_t g = s->state[6];
    uint32_t h = s->state[7];
      
#define SHA256_ROUND(a, b, c, d, e, f, g, h, i, constant) {\
    if (i >= 16){\
        uint32_t w1 = w[(i - 15) % 16];\
        uint32_t w2 = w[(i -  2) % 16];\
        w[(i % 16)] = w[i % 16] + (rotr32(w1,  7) ^ rotr32(w1, 18) ^ (w1 >>  3)) + w[(i-7) % 16] + (rotr32(w2, 17) ^ rotr32(w2, 19) ^ (w2 >> 10));\
    }\
    uint32_t temp1 = h + (rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25)) + ((e & f) ^ ((~e) & g)) + constant + w[i % 16];\
    uint32_t temp2 = (rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));\
    d += temp1;\
    h = temp1 + temp2;}

#if 0
    // not unrolled

    static const uint32_t sha256_round_constants[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };

    for (int i = 0; i < 64; i++){
        SHA256_ROUND(a, b, c, d, e, f, g, h, i, sha256_round_constants[i]);
        uint32_t temp = h;
        h = g;
        g = f;
        f = e;
        e = d;
        d = c;
        c = b;
        b = a;
        a = temp;
    }
#else
    // unrolled

    SHA256_ROUND(a, b, c, d, e, f, g, h,  0, 0x428a2f98)
    SHA256_ROUND(h, a, b, c, d, e, f, g,  1, 0x71374491)
    SHA256_ROUND(g, h, a, b, c, d, e, f,  2, 0xb5c0fbcf)
    SHA256_ROUND(f, g, h, a, b, c, d, e,  3, 0xe9b5dba5)
    SHA256_ROUND(e, f, g, h, a, b, c, d,  4, 0x3956c25b)
    SHA256_ROUND(d, e, f, g, h, a, b, c,  5, 0x59f111f1)
    SHA256_ROUND(c, d, e, f, g, h, a, b,  6, 0x923f82a4)
    SHA256_ROUND(b, c, d, e, f, g, h, a,  7, 0xab1c5ed5)
    SHA256_ROUND(a, b, c, d, e, f, g, h,  8, 0xd807aa98)
    SHA256_ROUND(h, a, b, c, d, e, f, g,  9, 0x12835b01)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 10, 0x243185be)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 11, 0x550c7dc3)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 12, 0x72be5d74)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 13, 0x80deb1fe)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 14, 0x9bdc06a7)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 15, 0xc19bf174)
    SHA256_ROUND(a, b, c, d, e, f, g, h, 16, 0xe49b69c1)
    SHA256_ROUND(h, a, b, c, d, e, f, g, 17, 0xefbe4786)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 18, 0x0fc19dc6)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 19, 0x240ca1cc)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 20, 0x2de92c6f)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 21, 0x4a7484aa)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 22, 0x5cb0a9dc)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 23, 0x76f988da)
    SHA256_ROUND(a, b, c, d, e, f, g, h, 24, 0x983e5152)
    SHA256_ROUND(h, a, b, c, d, e, f, g, 25, 0xa831c66d)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 26, 0xb00327c8)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 27, 0xbf597fc7)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 28, 0xc6e00bf3)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 29, 0xd5a79147)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 30, 0x06ca6351)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 31, 0x14292967)
    SHA256_ROUND(a, b, c, d, e, f, g, h, 32, 0x27b70a85)
    SHA256_ROUND(h, a, b, c, d, e, f, g, 33, 0x2e1b2138)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 34, 0x4d2c6dfc)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 35, 0x53380d13)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 36, 0x650a7354)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 37, 0x766a0abb)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 38, 0x81c2c92e)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 39, 0x92722c85)
    SHA256_ROUND(a, b, c, d, e, f, g, h, 40, 0xa2bfe8a1)
    SHA256_ROUND(h, a, b, c, d, e, f, g, 41, 0xa81a664b)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 42, 0xc24b8b70)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 43, 0xc76c51a3)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 44, 0xd192e819)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 45, 0xd6990624)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 46, 0xf40e3585)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 47, 0x106aa070)
    SHA256_ROUND(a, b, c, d, e, f, g, h, 48, 0x19a4c116)
    SHA256_ROUND(h, a, b, c, d, e, f, g, 49, 0x1e376c08)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 50, 0x2748774c)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 51, 0x34b0bcb5)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 52, 0x391c0cb3)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 53, 0x4ed8aa4a)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 54, 0x5b9cca4f)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 55, 0x682e6ff3)
    SHA256_ROUND(a, b, c, d, e, f, g, h, 56, 0x748f82ee)
    SHA256_ROUND(h, a, b, c, d, e, f, g, 57, 0x78a5636f)
    SHA256_ROUND(g, h, a, b, c, d, e, f, 58, 0x84c87814)
    SHA256_ROUND(f, g, h, a, b, c, d, e, 59, 0x8cc70208)
    SHA256_ROUND(e, f, g, h, a, b, c, d, 60, 0x90befffa)
    SHA256_ROUND(d, e, f, g, h, a, b, c, 61, 0xa4506ceb)
    SHA256_ROUND(c, d, e, f, g, h, a, b, 62, 0xbef9a3f7)
    SHA256_ROUND(b, c, d, e, f, g, h, a, 63, 0xc67178f2)
#endif

    s->state[0] += a;
    s->state[1] += b;
    s->state[2] += c;
    s->state[3] += d;
    s->state[4] += e;
    s->state[5] += f;
    s->state[6] += g;
    s->state[7] += h;
}

void sha256_finalize(struct sha256 *s){
    // append 0x80 0...0 n_bytes
    
    size_t i = s->n_bytes % 64;
    
    s->block[i++] = 0x80;
    
    // if block is too small to hold 8-byte n_bytes
    if (i > 56){
        while (i < 64) s->block[i++] = 0;
        sha256_process_block(s);
        i = 0;
    }
    
    while (i < 56){
        s->block[i++] = 0x00;
    }
    
    uint64_t n_bits = s->n_bytes*8;
    
    for (int shift = 7*8; shift >= 0; shift -= 8){
        s->block[i++] = n_bits >> shift;
    }
    
    sha256_process_block(s);
}

void sha256_append(struct sha256 *s, const void *bytes_ptr, size_t n_bytes){
    const uint8_t *bytes = (const uint8_t*)bytes_ptr;
    
    size_t i = s->n_bytes % 64;
    
    for (size_t j = 0; j < n_bytes; j++){
        s->block[i++] = bytes[j];
        
        if (i >= 64){
            sha256_process_block(s);
            i = 0;
        }
    }
    
    s->n_bytes += n_bytes;
}

void sha256_get_hash(struct sha256 *s, uint8_t *hash, size_t hash_size){
    assert(hash_size == SHA256_HASH_SIZE);
    
    uint32_t *dst = (uint32_t*)hash;
    for (int i = 0; i < 8; i++){
        dst[i] = __builtin_bswap32(s->state[i]);
    }
}

void sha256_digest(struct sha256 *s, uint8_t *hash, size_t hash_size){
    sha256_finalize(s);
    sha256_get_hash(s, hash, hash_size);
}

/*
void sha256_print_hash(struct sha256 *s){
    char result[64 + 1];
    snprintf(result, sizeof(result), "%08x%08x%08x%08x%08x%08x%08x%08x", s->state[0], s->state[1], s->state[2], s->state[3], s->state[4], s->state[5], s->state[6], s->state[7]);
    printf("%s\n", result);
}

int main(){
    struct sha256 s[1];
    sha256_init(s);
    const char *text = "asdf";
    sha256_append(s, text, strlen(text));
    sha256_finalize(s);
    
    sha256_print_hash(s);
    printf("f0e4c2f76c58916ec258f246851bea091d14d4247a2fc3e18694461b1816e13b\n");
    
    return 0;
}
*/
