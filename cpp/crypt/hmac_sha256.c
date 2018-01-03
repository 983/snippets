#include "hmac_sha256.h"

#include <string.h>
#include <assert.h>

void hmac_sha256_init(
    struct hmac_sha256 *h,
    const uint8_t *key,
    size_t key_size
){
    uint8_t key_temp[64];
    // pad temp key with zeros
    memset(key_temp, 0, sizeof(key_temp));
    
    if (key_size < 64){
        memcpy(key_temp, key, key_size);
    }else{
        // reduce key length so it fits into temp key buffer
        struct sha256 s[1];
        sha256_init(s);
        sha256_append(s, key, key_size);
        sha256_digest(s, key_temp, SHA256_HASH_SIZE);
    }
    
    // produce two new keys by xor-ing temp key with some constants
    uint8_t key_inner[64];
    uint8_t key_outer[64];
    
    for (size_t i = 0; i < 64; i++){
        key_inner[i] = key_temp[i] ^ 0x36;
        key_outer[i] = key_temp[i] ^ 0x5c;
    }
    
    sha256_init(h->inner);
    sha256_init(h->outer);
    
    sha256_append(h->inner, key_inner, sizeof(key_inner));
    sha256_append(h->outer, key_outer, sizeof(key_outer));
}

void hmac_sha256_append(
    struct hmac_sha256 *h,
    const uint8_t *message,
    size_t message_size
){
    sha256_append(h->inner, message, message_size);
}

void hmac_sha256_digest(
    struct hmac_sha256 *h,
    uint8_t *hmac,
    size_t hmac_size
){
    // sha256 produces a 256 bit hash which is 32 bytes
    assert(hmac_size == SHA256_HASH_SIZE);
    
    // inner sha256 hash is done
    uint8_t inner_result[SHA256_HASH_SIZE];
    sha256_digest(h->inner, inner_result, sizeof(inner_result));
    
    // feed outer sha256 with inner result
    sha256_append(h->outer, inner_result, sizeof(inner_result));
    sha256_digest(h->outer, hmac, hmac_size);
}
/*
#include <stdio.h>

int main(){
    uint8_t key[3] = {4, 5, 6};
    uint8_t message[4] = {1, 3, 3, 7};
    uint8_t hmac[32];
    
    struct hmac_sha256 h[1];
    
    hmac_sha256_init(h, key, sizeof(key));
    hmac_sha256_append(h, message, sizeof(message));
    hmac_sha256_digest(h, hmac, sizeof(hmac));
    
    for (size_t i = 0; i < sizeof(hmac); i++){
        printf("%i, ", (int)hmac[i]);
    }
    printf("\n");
    
    return 0;
}

#include "sha256.c"
*/
