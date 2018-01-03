#include "aes128ctr.h"

#include <assert.h>
#include <string.h>
#include <wmmintrin.h>

#define MAKE_ROUND_KEY(i, round_constant) {\
    __m128i temp = k[i];\
    temp = _mm_xor_si128(temp, _mm_slli_si128(temp, 4));\
    temp = _mm_xor_si128(temp, _mm_slli_si128(temp, 8));\
    __m128i temp2 = _mm_aeskeygenassist_si128(k[i], round_constant);\
    k[i + 1] = _mm_xor_si128(temp, _mm_shuffle_epi32(temp2, 0xff));\
}

static void aes128_load_key_encrypt(const uint8_t *key, __m128i *k){
    k[0] = _mm_loadu_si128((const __m128i*)key);
    
    MAKE_ROUND_KEY(0, 1)
    MAKE_ROUND_KEY(1, 2)
    MAKE_ROUND_KEY(2, 4)
    MAKE_ROUND_KEY(3, 8)
    MAKE_ROUND_KEY(4, 16)
    MAKE_ROUND_KEY(5, 32)
    MAKE_ROUND_KEY(6, 64)
    MAKE_ROUND_KEY(7, 128)
    MAKE_ROUND_KEY(8, 27)
    MAKE_ROUND_KEY(9, 54)
}

static void aes128ctr_fill_keystream(struct aes128ctr *state){
    __m128i v = _mm_loadu_si128((__m128i*)state->iv);

    v = _mm_xor_si128(v, ((__m128i*)state->round_keys)[0]);
    for (int i = 1; i < 10; i++) v = _mm_aesenc_si128(v, ((__m128i*)state->round_keys)[i]);
    v = _mm_aesenclast_si128(v, ((__m128i*)state->round_keys)[10]);
    
    _mm_storeu_si128((__m128i*)state->keystream, v);

    uint8_t *counter = state->iv + 8;

    for (int i = 7; i >= 0; i--){
        counter[i]++;
        if (counter[i] != 0) break;
    }
}

void aes128ctr_init(
    struct aes128ctr *state,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *nonce,
    size_t nonce_len
){
    assert(key_len == 16);
    assert(nonce_len == 8);
    
    memcpy(state->iv + 0, nonce, 8);
    memset(state->iv + 8,     0, 8);
    
    state->pos = 0;
    
    aes128_load_key_encrypt(key, (__m128i*)state->round_keys);
    
    aes128ctr_fill_keystream(state);
}

void aes128ctr_crypt(struct aes128ctr *state, uint8_t *buffer, size_t n){
    for (size_t i = 0; i < n; i++){
        buffer[i] ^= state->keystream[state->pos++];
        
        if (state->pos >= 16){
            state->pos = 0;
            aes128ctr_fill_keystream(state);
        }
    }
}
