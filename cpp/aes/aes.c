#include <assert.h>
#include <string.h>
#include <stdint.h>
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

void encrypt_aes128_cbc(uint8_t *encrypted, const uint8_t *plaintext, size_t n_bytes, const uint8_t *key, const uint8_t *iv){
    assert(n_bytes % 16 == 0);
    __m128i current_iv = _mm_loadu_si128((__m128i*)iv);
    __m128i round_keys[20];
    aes128_load_key_encrypt(key, round_keys);
    
    for (size_t i = 0; i < n_bytes; i += 16){
        __m128i v = _mm_loadu_si128((__m128i*)(plaintext + i));
        v = _mm_xor_si128(v, current_iv);

        v = _mm_xor_si128(v, round_keys[0]);
        for (int i = 1; i < 10; i++) v = _mm_aesenc_si128(v, round_keys[i]);
        v = _mm_aesenclast_si128(v, round_keys[10]);
        
        current_iv = v;
        _mm_storeu_si128((__m128i*)(encrypted + i), v);
    }
}

void decrypt_aes128_cbc(uint8_t *decrypted, const uint8_t *encrypted, size_t n_bytes, const uint8_t *key, const uint8_t *iv){
    assert(n_bytes % 16 == 0);
    __m128i current_iv = _mm_loadu_si128((__m128i*)iv);
    __m128i round_keys[20];
    aes128_load_key_encrypt(key, round_keys);
    for (int i = 11; i < 20; i++) round_keys[i] = _mm_aesimc_si128(round_keys[20 - i]);
    
    for (size_t i = 0; i < n_bytes; i += 16){
        __m128i v = _mm_loadu_si128((__m128i*)(encrypted + i));
        __m128i temp_iv = v;

        v = _mm_xor_si128(v, round_keys[10]);
        for (int i = 11; i < 20; i++) v = _mm_aesdec_si128(v, round_keys[i]);
        v = _mm_aesdeclast_si128(v, round_keys[0]);
        
        v = _mm_xor_si128(v, current_iv);
        current_iv = temp_iv;
        _mm_storeu_si128((__m128i*)(decrypted + i), v);
    }
}

/* for testing */
#if 0

#include <stdio.h>
#include <stdlib.h>

uint8_t* load(const char *path, size_t *n_bytes){
    FILE *fp = fopen(path, "rb");
    
    if (!fp) return NULL;
    
    fseek(fp, 0, SEEK_END);
    size_t n = ftell(fp);
    rewind(fp);
    
    uint8_t *data = (uint8_t*)malloc(n);
    
    if (!data) return NULL;
    
    size_t m = fread(data, 1, n, fp);
    
    if (n != m){
        free(data);
        return NULL;
    }
    
    if (n_bytes) *n_bytes = n;
    
    return data;
}

int main(){
    size_t iv_length;
    size_t key_length;
    size_t plaintext_length;
    size_t encrypted_length;
    
    uint8_t *iv                    = load("test_data/iv", &iv_length);
    uint8_t *key                   = load("test_data/key", &key_length);
    uint8_t *plaintext             = load("test_data/plaintext", &plaintext_length);
    uint8_t *encrypted_groundtruth = load("test_data/encrypted", &encrypted_length);
    
    if (iv                    == NULL) return -1;
    if (key                   == NULL) return -1;
    if (plaintext             == NULL) return -1;
    if (encrypted_groundtruth == NULL) return -1;
    
    if (iv_length             !=               16) return -1;
    if (key_length            !=               16) return -1;
    if (plaintext_length      != encrypted_length) return -1;
    
    uint8_t *encrypted = (uint8_t*)malloc(plaintext_length);
    uint8_t *decrypted = (uint8_t*)malloc(plaintext_length);
    uint8_t *inplace   = (uint8_t*)malloc(plaintext_length);
    
    memcpy(inplace, plaintext, plaintext_length);
    
    // use like this
    encrypt_aes128_cbc(encrypted, plaintext, plaintext_length, key, iv);
    decrypt_aes128_cbc(decrypted, encrypted, encrypted_length, key, iv);
    
    if (0 != memcmp(encrypted, encrypted_groundtruth, encrypted_length)) return -1;
    if (0 != memcmp(decrypted, plaintext            , plaintext_length)) return -1;
    
    // can also be used inplace
    encrypt_aes128_cbc(inplace, inplace, plaintext_length, key, iv);
    if (0 != memcmp(encrypted_groundtruth, inplace, encrypted_length)) return -1;
    
    decrypt_aes128_cbc(inplace, inplace, encrypted_length, key, iv);
    if (0 != memcmp(plaintext, inplace, plaintext_length)) return -1;
    
    printf("encrypted and decrypted %i bytes successfully\n", (int)plaintext_length);
    
    free(iv);
    free(key);
    free(plaintext);
    free(encrypted_groundtruth);
    free(encrypted);
    free(decrypted);
    free(inplace);
    
    return 0;
}

#endif
