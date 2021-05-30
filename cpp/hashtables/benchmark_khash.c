#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

char* read_text(const char *path, size_t *n_bytes){
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    rewind(f);
    char *text = malloc(n + 1);
    size_t m = fread(text, 1, n, f);
    if (n != m){
        free(text);
        fclose(f);
        return NULL;
    }
    *n_bytes = n;
    text[n] = '\0';
    fclose(f);
    return text;
}

int is_alpha(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

//#include "siphash.h"
//#include "siphash.c"

uint64_t hash(const void *data, size_t n_bytes){
    /*
    uint32_t key[4] = {0x84714387, 0x78541348, 0x41354687, 0x71274584};

    uint64_t h;
    siphash(data, n_bytes, key, (uint8_t*)&h, sizeof(h));
    return h;
    */

    const uint8_t *ptr = data;
    uint64_t h = 0x3141592653589793ull;
    uint64_t prime = 10852488427757893799ull;
    uint64_t remaining = 0;

    for (; n_bytes >= 8; n_bytes -= 8, ptr += 8) h = (h ^ *(uint64_t*)ptr) * prime;
    for (; n_bytes > 0; n_bytes--) remaining = (remaining << 8) | *ptr++;

    return (h ^ remaining) * prime;
}

#include "khash.h"
KHASH_SET_INIT_STR(str)

int main(){
    khash_t(str) *h;
    int absent;
    h = kh_init(str);

    double t = sec();

    size_t n = 0;
    char *text = read_text("/home/user/data/hackernews/HNI_2010-01", &n);
    for (size_t i = 0; i < n;){
        while (i < n && !is_alpha(text[i])) i++;

        if (i >= n) break;

        char *word = text + i;

        while (i < n && is_alpha(text[i])) i++;

        text[i] = '\0';

        kh_put(str, h, word, &absent);

        i++;
    }

    double dt = sec() - t;

    printf("%u - %f seconds\n", kh_size(h), dt);

    // 93169 - comment

    free(text);
    kh_destroy(str, h);

    return 0;
}
