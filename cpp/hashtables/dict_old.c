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
    //~ uint32_t key[4] = {0x84714387, 0x78541348, 0x41354687, 0x71274584};
    //~ uint64_t h;
    //~ siphash(data, n_bytes, key, (uint8_t*)&h, sizeof(h));
    //~ return h;

    const uint8_t *ptr = data;
    uint64_t h = 0x3141592653589793ull;
    uint64_t prime = 10852488427757893799ull;
    uint64_t remaining = 0;

    for (; n_bytes >= 8; n_bytes -= 8, ptr += 8) h = (h ^ *(uint64_t*)ptr) * prime;
    for (; n_bytes > 0; n_bytes--) remaining = (remaining << 8) | *ptr++;

    return (h ^ remaining) * prime;
}

/*
uint64_t hash(const void *data, size_t n_bytes){
    uint64_t h = 0x3141592653589793ull;

    const uint8_t *ptr = (const uint8_t*)data;
    for (size_t i = 0; i < n_bytes; i++) {
        h = h * 10852488427757893799ull + ptr[i];
    }
    return h;
}
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define my_assert(cond) do {\
    if ((!(cond))){\
        fprintf(stderr, "ASSERTION FAILED!\nFile: %s\nLine: %d\n%s\n", __FILE__, __LINE__, #cond);\
        size_t addr = rand() == 0;\
        *(int*)addr = 0;\
    }\
} while (0)

typedef struct {
    const char *key;
    uint64_t hash;
    void *value;
    size_t probes;
} DictEntry;

typedef struct Dict {
    DictEntry *entries;
    size_t size;
    size_t capacity;
} Dict;

size_t probes_total = 0;

void dict_init2(Dict *d, size_t capacity){
    d->entries = malloc(sizeof(*d->entries) * capacity);
    d->size = 0;
    d->capacity = capacity;
    for (size_t i = 0; i < capacity; i++){
        d->entries[i].key = NULL;
    }
}

void dict_init(Dict *d){
    d->entries = NULL;
    d->size = 0;
    d->capacity = 0;
}

void dict_free(Dict *d){
    free(d->entries);
    d->size = 0;
    d->capacity = 0;
}

DictEntry *dict_find(Dict *d, const char *key){
    if (d->size == 0) return NULL;

    //printf("%zu %zu\n", d->size, d->capacity);
    my_assert(d->size < d->capacity);

    size_t n = strlen(key);
    uint64_t h = hash(key, n);
    size_t i = h % d->capacity;
    size_t probes = 0;
    while (1){
        probes++;
        my_assert(i < d->capacity);
        DictEntry *e = &d->entries[i];

        //printf("find probe %zu of %zu, key: %p\n", i, d->capacity, (void*)e->key);

        if (!e->key) return NULL;

        if (h == e->hash && 0 == memcmp(key, e->key, n)){
            e->probes = probes;
            return e;
        }

        i++;
        if (i >= d->capacity) i = 0;
    }
}


DictEntry* dict_put(Dict *d, const char *key, void *value);

DictEntry* dict_entry(Dict *d, const char *key, int *newly_created){
    if (d->size * 3 >= d->capacity * 2){
        size_t new_capacity = d->capacity * 7 / 4 + 5;

        printf("resize from %zu to %zu\n", d->capacity, new_capacity);

        Dict tmp[1];
        dict_init2(tmp, new_capacity);

        for (size_t i = 0; i < d->capacity; i++){
            DictEntry *e = &d->entries[i];

            if (e->key){
                dict_put(tmp, e->key, e->value);
            }
        }

        dict_free(d);
        memcpy(d, tmp, sizeof(*d));
    }

    uint64_t h = hash(key, strlen(key));
    size_t i = h % d->capacity;
    size_t probes = 0;

    while (1){
        probes++;
        my_assert(i < d->capacity);
        DictEntry *e = &d->entries[i];

        if (e->key){
            if (e->hash == h && 0 == strcmp(e->key, key)) return e;
        }else{
            if (newly_created) *newly_created = 1;
            d->size++;
            e->key = key;
            e->hash = h;
            e->probes = probes;
            return e;
        }

        probes_total++;

        i++;
        if (i >= d->capacity) i = 0;
    }
}

DictEntry* dict_put(Dict *d, const char *key, void *value){
    DictEntry *e = dict_entry(d, key, NULL);

    if (e){
        e->value = value;
    }

    return e;
}

void* dict_get(Dict *d, const char *key){
    DictEntry *e = dict_find(d, key);

    return e ? e->value : NULL;
}

int main(){
    //printf("%" PRIx64 "\n", hash("Hello", 5));

    Dict d[1];
    dict_init(d);

    dict_put(d, "key1", "value1");
    dict_put(d, "key2", "value2");
    dict_put(d, "key3", "value3");
    dict_put(d, "key4", "value4");

    for (size_t i = 0; i < d->capacity; i++){
        DictEntry *e = &d->entries[i];

        if (e->key){
            printf("%s - %s\n", e->key, (char*)e->value);
        }
    }

    printf("%s\n", (char*)dict_get(d, "key2"));

    dict_free(d);

    dict_init(d);

    double t = sec();

    size_t n = 0;
    size_t n_words = 0;
    char *text = read_text("/home/user/data/hackernews/HNI_2010-01", &n);
    for (size_t i = 0; i < n;){
        while (i < n && !is_alpha(text[i])) i++;

        if (i >= n) break;

        char *word = text + i;

        while (i < n && is_alpha(text[i])) i++;

        text[i] = '\0';

        int newly_created = 0;
        DictEntry *e = dict_entry(d, word, &newly_created);
        e->value = NULL;

        if (newly_created){
            printf("%zu unique words - %zu probes total - %zu %% fill - %6zu probes - %6zu hash - %zu capacity - word %s\n", d->size, probes_total, d->size * 100 / d->capacity, e->probes, hash(word, strlen(word)) % d->capacity, d->capacity, word);
        }

        n_words++;

        i++;
    }

    double dt = sec() - t;

    printf("%zu unique words - %zu words total - %f seconds\n", d->size, n_words, dt);

    // 93169 - comment

    free(text);

    dict_free(d);

    return 0;
}
