#include <stdint.h>
#include <stdio.h>

// find some values for xorshift PRNG which cover the full 32 bit range
int main(){
    uint32_t s0, s1, s2;
    
    // found values for shifts:
    // 5 9 7
    // 11 17 13
    
    for (s0 = 10; s0 < 20; s0++)
    for (s1 = 10; s1 < 20; s1++)
    for (s2 = 10; s2 < 20; s2++)
    {
        uint32_t state = 1;
        
        uint32_t n = 0;
        
        do {
            state ^= state << s0;
            state ^= state >> s1;
            state ^= state << s2;
            
            n++;
        
        } while (state != 1);
        
        printf("0x%08x, %u %u %u\n", n, s0, s1, s2);
        
        if (n == 0xffffffff) return;
    }
}
