#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

uint64_t hash(const void *data, size_t n_bytes){
    const uint8_t *ptr = data;
    uint64_t h = 0x3141592653589793ull;
    uint64_t prime = 10852488427757893799ull;
    uint64_t remaining = 0;

    for (; n_bytes >= 8; n_bytes -= 8) h = (h ^ remaining) * prime;
    for (; n_bytes > 0; n_bytes--) remaining = (remaining << 8) | *ptr++;

    return (h ^ remaining) * prime;
}

typedef struct {
    const char *key;
    uint64_t hash;
    void *value;
} DictEntry;

typedef struct Dict {
    DictEntry *entries;
    size_t size;
    size_t capacity;
} Dict;

void dict_init2(Dict *d, size_t capacity){
    d->entries = malloc(sizeof(*d->entries) * capacity);
    d->size = 0;
    d->capacity = capacity;
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
    size_t n = strlen(key);
    uint64_t h = hash(key, n);

    for (size_t i = 0; i < d->size; i++){
        DictEntry *e = &d->entries[i];

        if (h == e->hash && 0 == memcmp(key, e->key, n)) return e;
    }

    return NULL;
}

DictEntry* dict_put(Dict *d, const char *key, void *value){
    size_t n = strlen(key);
    uint64_t h = hash(key, n);

    for (size_t i = 0; i < d->size; i++){
        DictEntry *e = &d->entries[i];

        if (h == e->hash && 0 == memcmp(key, e->key, n)){
            e->value = value;
            return e;
        }
    }

    if (d->size >= d->capacity){
        size_t new_capacity = d->capacity * 3 / 2 + 1;

        // TODO handle OOM
        d->entries = realloc(d->entries, new_capacity * sizeof(*d->entries));
        d->capacity = new_capacity;
    }

    DictEntry *e = &d->entries[d->size++];
    e->key = key;
    e->hash = h;
    e->value = value;
    return e;
}

void* dict_get(Dict *d, const char *key){
    DictEntry *e = dict_find(d, key);

    return e ? e->value : NULL;
}
