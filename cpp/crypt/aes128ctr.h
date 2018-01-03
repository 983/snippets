#pragma once

#include <stdint.h>

struct aes128ctr {
    uint8_t iv[16];
    uint8_t round_keys[4*4*20];
    uint8_t keystream[16];
    uint32_t pos;
};

void aes128ctr_init(
    struct aes128ctr *state,
    const uint8_t *key,
    size_t key_len,
    const uint8_t *nonce,
    size_t nonce_len
);

void aes128ctr_crypt(struct aes128ctr *state, uint8_t *buffer, size_t n);

/* Example: */
/*
#include <stdio.h>

int main(){
    uint8_t key[16];
    memset(key, 0, sizeof(key));
    
    uint8_t nonce[8];
    memset(nonce, 0, sizeof(nonce));
    
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));
    
    struct aes128ctr state[1];
    aes128ctr_init(state, key, sizeof(key), nonce, sizeof(nonce));
    
    aes128ctr_crypt(state, buffer, sizeof(buffer));
    
    for (size_t i = 0; i < sizeof(buffer); i += 16){
        for (size_t j = i; j < i + 16; j++){
            printf("%i, ", buffer[j]);
        }
        printf("\n");
    }
    
    return 0;
}
*/
