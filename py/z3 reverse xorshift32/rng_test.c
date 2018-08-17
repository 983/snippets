#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t state = 0xf2b58529;

uint32_t xorshift32(){
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state <<  5;
    
    return state;
}

int main(){
    
    for (int i = 0; i < 5; i++){
        printf("0x%08x\n", xorshift32());
    }
    
    return 0;
}
