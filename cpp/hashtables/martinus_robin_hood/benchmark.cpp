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
    char *text = (char*)malloc(n + 1);
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

#include "robin_hood.h"
#include <string.h>
#include <stdint.h>

uint64_t hash(const void *data, size_t n_bytes){
    uint64_t hash = 7;

    const uint8_t *ptr = (const uint8_t*)data;
    for (size_t i = 0; i < n_bytes; i++) {
        hash = hash * 31 + ptr[i];
    }
    return hash;
    /*
    const uint8_t *ptr = (const uint8_t*)data;
    uint64_t h = 0x3141592653589793ull;
    uint64_t prime = 10852488427757893799ull;
    uint64_t remaining = 0;

    for (; n_bytes >= 8; n_bytes -= 8, ptr += 8) h = (h ^ *(uint64_t*)ptr) * prime;
    for (; n_bytes > 0; n_bytes--) remaining = (remaining << 8) | *ptr++;

    return (h ^ remaining) * prime;
    */
}

struct String {
    const char *string;
    size_t n;
    size_t h;

    String(const String &s): string(s.string), n(s.n), h(s.h){}

    String(const char *string, size_t n): string(string), n(n){
        h = hash(string, n);
    }
};

bool operator == (const String &a, const String &b){
    return a.n == b.n && 0 == memcmp(a.string, b.string, a.n);
}

struct Hash {
    size_t operator() (const String &s) const {
        return s.h;
    }
};

int main(){
    robin_hood::unordered_set<String, Hash> s;

    double t = sec();

    size_t n = 0;
    char *text = read_text("/home/user/data/hackernews/HNI_2010-01", &n);
    for (size_t i = 0; i < n;){
        while (i < n && !is_alpha(text[i])) i++;

        if (i >= n) break;

        size_t start = i;

        while (i < n && is_alpha(text[i])) i++;

        text[i] = '\0';

        s.insert(String(text + start, i - start));

        i++;
    }

    double dt = sec() - t;

    printf("%zu - %f seconds\n", s.size(), dt);

    free(text);

    return 0;
}
