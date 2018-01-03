#pragma once

#include <stdint.h>

struct chacha20 {
    uint32_t state[20];
    uint8_t keystream[64];
    size_t position;
};

void chacha20_init(
    struct chacha20 *c,
    const uint8_t *key,
    size_t key_size,
    const uint8_t *nonce,
    size_t nonce_size
);

void chacha20_crypt(struct chacha20 *c, uint8_t *bytes, size_t n_bytes);

/*
int main(){
    uint8_t key[32] = {40, 15, 228, 100, 60, 151, 106, 126, 185, 39, 229, 165, 12, 141, 59, 156, 192, 13, 245, 52, 221, 216, 23, 251, 88, 244, 13, 197, 60, 107, 225, 226};
    uint8_t message[150] = {198, 235, 226, 252, 248, 77, 210, 119, 212, 29, 112, 142, 82, 42, 60, 238, 111, 176, 62, 35, 69, 126, 41, 242, 36, 178, 226, 78, 13, 130, 245, 105, 129, 104, 225, 207, 13, 137, 29, 77, 222, 5, 50, 133, 202, 178, 93, 122, 227, 169, 78, 215, 75, 131, 178, 191, 179, 158, 71, 30, 115, 57, 106, 86, 69, 124, 234, 7, 192, 58, 213, 125, 137, 80, 225, 179, 177, 41, 119, 214, 85, 203, 165, 57, 102, 143, 232, 49, 15, 0, 142, 112, 57, 244, 58, 201, 113, 167, 114, 123, 151, 15, 250, 58, 35, 143, 175, 106, 250, 156, 6, 58, 245, 41, 89, 135, 190, 83, 187, 129, 65, 225, 158, 142, 16, 148, 147, 188, 43, 145, 20, 142, 12, 184, 8, 81, 255, 146, 209, 112, 43, 244, 11, 202, 204, 72, 161, 215, 177, 227};
    uint8_t nonce[8] = {1, 151, 155, 178, 97, 176, 17, 255};
    
    struct chacha20 c[1];
    chacha20_init(c, key, sizeof(key), nonce, sizeof(nonce));
    chacha20_crypt(c, message, sizeof(message));
    
    for (size_t i = 0; i < sizeof(message); i++){
        printf("%i\n", (int)message[i]);
    }
    
    return 0;
}
*/
