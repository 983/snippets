#include "hmac_sha256.h"

#include <string.h>
#include <assert.h>

static void u32_to_bytes_big_endian(uint32_t x, uint8_t *bytes){
    bytes[0] = (x >> 3*8) & 0xff;
    bytes[1] = (x >> 2*8) & 0xff;
    bytes[2] = (x >> 1*8) & 0xff;
    bytes[3] = (x >> 0*8) & 0xff;
}

void pbkdf2_sha256(
    uint8_t *derived_key, size_t derived_key_size,
    const uint8_t *key, size_t key_size,
    const uint8_t *salt, size_t salt_size,
    size_t n_iterations
){
    assert(derived_key_size%SHA256_HASH_SIZE == 0);
    assert(n_iterations > 0);
    
    uint32_t loop = 1;
    for (size_t i = 0; i < derived_key_size; i += SHA256_HASH_SIZE){
        
        uint8_t loop_bytes[4];
        u32_to_bytes_big_endian(loop, loop_bytes);
        loop++;
        
        uint8_t u[SHA256_HASH_SIZE];
        
        struct hmac_sha256 h[1];
        struct hmac_sha256 h_with_key[1];
        hmac_sha256_init(h, key, key_size);
        
        memcpy(h_with_key, h, sizeof(*h_with_key));
        
        hmac_sha256_append(h, salt, salt_size);
        hmac_sha256_append(h, loop_bytes, sizeof(loop_bytes));
        hmac_sha256_digest(h, u, sizeof(u));
        
        uint8_t rkey[SHA256_HASH_SIZE];
        memcpy(rkey, u, sizeof(rkey));
        
        for (size_t j = 0; j < n_iterations - 1; j++){
            memcpy(h, h_with_key, sizeof(*h));
            hmac_sha256_append(h, u, sizeof(u));
            hmac_sha256_digest(h, u, sizeof(u));
            
            for (size_t k = 0; k < sizeof(rkey); k++){
                rkey[k] ^= u[k];
            }
        }
        
        memcpy(derived_key + i, rkey, sizeof(rkey));
    }
}
/*
#include <stdio.h>

int main(){
    uint8_t key[3] = {1, 2, 3};
    uint8_t salt[4] = {4, 5, 6, 7};
    
    uint8_t derived_key[256];
    
    pbkdf2_sha256(
        derived_key, sizeof(derived_key), 
        key, sizeof(key),
        salt, sizeof(salt),
        4096);
    
    for (size_t i = 0; i < sizeof(derived_key); i++){
        printf("%i, ", (int)derived_key[i]);
    }
    printf("\n");
    
    return 0;
}

#include "hmac_sha256.c"
#include "sha256.c"
*/
